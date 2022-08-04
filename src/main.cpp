#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <EEPROM.h>
#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"
#include "driver/adc.h"
#include <stdlib.h>

hw_timer_t * timer = NULL;



template <class T> int EEPROM_writeAnything(int ee, const T& value){
  ee+=0;
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
      EEPROM.write(ee++, *p++);
  return i;
}
template <class T> int EEPROM_readAnything(int ee, T& value){
  ee+=0;
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
      *p++ = EEPROM.read(ee++);
  return i;
}

uint32_t ii=0;

struct Info SensorInfo;

void IRAM_ATTR onTimer(){
  uint32_t tmp = micros();
  Serial.printf(">> %d\n",tmp-lastTimer);
  lastTimer = tmp;

  SensorInfo.UpTime = millis()/1000;

  if(Sensor_Resistance_CheckPeriod <=0 || Sensor_Resistance_CheckPeriod >= 3600) Sensor_Resistance_CheckPeriod = 1;
  if(Sensor_Battery_CheckPeriod    <=0 || Sensor_Battery_CheckPeriod    >= 3600) Sensor_Battery_CheckPeriod    = 1;
  
  if(Sensor_Resistance_LastCheck >= Sensor_Resistance_CheckPeriod-1 ){
    Sensor_Resistance_LastCheck = 0;
    Sensor_Resistance_Value_expired = true;
  }else{
    Sensor_Resistance_LastCheck++;
  }

  if( Sensor_Battery_LastCheck >= Sensor_Battery_CheckPeriod-1){
    Sensor_Battery_LastCheck = 0;
    Sensor_Battery_Value_expired = true;
  }else{
    Sensor_Battery_LastCheck++;
  }

  if( Sensor_Temperature_LastCheck >= Sensor_Temperature_CheckPeriod-1){
    Sensor_Temperature_LastCheck = 0;
    Sensor_Temperature_Value_expired = true;
  }else{
    Sensor_Temperature_LastCheck++;
  }
}


void Sensor_UpTimeUpdate(){
  float_t tmp = millis();
  
}

uint32_t getRR(uint8_t nn){
  int n = nn;
 
  uint32_t RR = 0;
  uint32_t LR = 0;
  uint32_t SR = 0;

  while(n==0){
    if(RR==0 and LR==0) RR=LR=random();
    else RR=random();

    if (abs( (long)(RR-LR) )  <= LR *2){  
      SR = SR + RR;

      LR=RR;
      n--;
    }
  }  

  SR = ( RR / nn );
  return SR;
}

void Sensor_ResistanceUpdate(){
  Serial.println(":: [F] Sensor_ResistanceUpdate()");
  SensorInfo.R = getRR(5);
  SensorInfo.dt = (millis()/1000) - SensorInfo.t_last;
  SensorInfo.dR = (SensorInfo.R - SensorInfo.R_Last)/(SensorInfo.dt);
  float_t tmp = random(0,100);
  if (SensorInfo.Resistance != tmp){
    SensorInfo.Resistance = tmp;
    Characteristic_Sensor_Value.setValue(tmp);
    Characteristic_Sensor_Value.notify();
    Sensor_Resistance_Value_expired = false; 
  }
}

void Sesnosr_BatteryUpdate(){
  Serial.println(":: [F] Sesnosr_BatteryUpdate()");
  float_t tmp = random(0,100);
  if (SensorInfo.BatteryLevel != tmp){
    SensorInfo.BatteryLevel = tmp;
    Sensor_Battery_Value_expired = false; 
  }
}

void Sensor_TemperatureUpdate(){
  Serial.println(":: [F] Sensor_TemperatureUpdate()");
  float_t tmp = 99;// temperatureRead();
  if (SensorInfo.Temperature != tmp){
    SensorInfo.Temperature = tmp;
    Sensor_Temperature_Value_expired = false; 
  }
}

void UpdateAdvertisingData(){
  Serial.println(":: [F] Sensor_UpdateAdvertisingData()");
}

void init_pin(){
  // pinMode(PIN_ALARM_LED  , OUTPUT);
  // pinMode(PIN_BUTTON, INPUT_PULLUP);
  // pinMode(PIN_BEEP  , OUTPUT);
  pinMode(A0  , OUTPUT);



  
}

void init_flash(){
  digitalWrite(PIN_BEEP,HIGH);
  digitalWrite(PIN_ALARM_LED,HIGH);  
  delay(250);
  digitalWrite(PIN_BEEP,LOW);
  digitalWrite(PIN_ALARM_LED,LOW);  
}

void init_Sensor(){
  SensorInfo.Status        = 0;   // uint8_t 0 --> 255
  SensorInfo.ALARM         = false;
  SensorInfo.dR            = 0; 
  SensorInfo.Resistance    = 0;
  SensorInfo.ResistanceMAX = 0;
  SensorInfo.Temperature   = 0;
  SensorInfo.BatteryLevel  = 0;
}


void PrintSensor(){
  Serial.printf(":: Resistance: %d \t %d \n", SensorInfo.Resistance, SensorInfo.ResistanceMAX);
}


//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println(":: BLE Connect");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println(":: BLE Disconnect");
    Serial.println(":: >> Restart Advertising ..... ");
    pServer->getAdvertising()->start();
  }
};


class Sensor_Status_Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      uint8_t* received_data = pCharacteristic->getData();
      Serial.print(":: Sensor_Status_Callbacks >> onRead >> ");
      Serial.println(*received_data);
    }
    void onRead(BLECharacteristic *pCharacteristic)
    {
      uint8_t* received_data = pCharacteristic->getData();
      Serial.print(":: Sensor_Status_Callbacks >> onRead >> ");
      Serial.println(*received_data);
    }
};

class Sensor_Value_Set_Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      uint8_t* received_data = pCharacteristic->getData();
      Serial.print(":: Sensor_Value_Set_Callbacks >> onWrite >> ");
      Serial.println(*received_data);
    }
};

class Sensor_Reset_Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      uint8_t* received_data = pCharacteristic->getData();
      Serial.print(":: Sensor_Reset_Callbacks >> onRead >> ");
      Serial.println(*received_data);
    }
};

void init_BLE(){
   // Create the BLE Device 
  BLEDevice::init("MyESP32");

  // Create the BLE Server
  BLEServer  *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(BLE_UUID_SENSOR_SERVICE);


  // Create BLE Characteristics and Create a BLE Descriptor
  // STATUS
  pService->addCharacteristic(&Characteristic_Sensor_Status);
  Descripto_Sensor_Status.setValue("Sensor Status");
  Characteristic_Sensor_Status.addDescriptor(&Descripto_Sensor_Status);
  Characteristic_Sensor_Status.setValue("1");
  
  // Value
  pService->addCharacteristic(&Characteristic_Sensor_Value);
  Descripto_Sensor_Value.setValue("Sensor Value");
  Characteristic_Sensor_Value.addDescriptor(&Descripto_Sensor_Value);
  Characteristic_Sensor_Value.setValue("124");

  // Value SET
  pService->addCharacteristic(&Characteristic_Sensor_Value_Set);
  Descripto_Sensor_Value_Set.setValue("Sensor Value Set");
  Characteristic_Sensor_Value_Set.addDescriptor(&Descripto_Sensor_Value_Set);
  Characteristic_Sensor_Value_Set.setCallbacks(new Sensor_Value_Set_Callbacks());
  Characteristic_Sensor_Value_Set.setValue("666");

  // // ALARM
  // pService->addCharacteristic(&Characteristic_Sensor_Alarm);
  // Descripto_Sensor_Alarm.setValue("Sensor Alarm");
  // Characteristic_Sensor_Alarm.addDescriptor(&Descripto_Sensor_Alarm);

  // // Value
  // pService->addCharacteristic(&Characteristic_Sensor_Alarm_Reset);
  // Descripto_Sensor_Alarm_Reset.setValue("Sensor Alarm Reset");
  // Characteristic_Sensor_Alarm_Reset.addDescriptor(&Descripto_Sensor_Alarm_Reset);

  // // Update Period
  // pService->addCharacteristic(&Characteristic_Sensor_Update_Period);
  // Descripto_Sensor_Update_Period.setValue("Sensor Update Period");
  // Characteristic_Sensor_Update_Period.addDescriptor(&Descripto_Sensor_Update_Period);

  // Reset
  pService->addCharacteristic(&Characteristic_Sensor_Reset);
  Descripto_Sensor_Reset.setValue("Sensor Alarm Reset");
  Characteristic_Sensor_Reset.addDescriptor(&Descripto_Sensor_Reset);
  Characteristic_Sensor_Reset.setCallbacks(new Sensor_Reset_Callbacks());


  // Temperature
  // pService->addCharacteristic(&Characteristic_Sensor_Temperature);
  // Descripto_Sensor_Temperature.setValue("Sensor Temperature");
  // Characteristic_Sensor_Temperature.addDescriptor(&Descripto_Sensor_Temperature);

  // UpTime
  pService->addCharacteristic(&Characteristic_Sensor_UpTime);
  Descripto_Sensor_UpTime.setValue("Sensor UpTime");
  Characteristic_Sensor_UpTime.addDescriptor(&Descripto_Sensor_UpTime);

  // Start the service
  pService->start();

  // Start advertising
  // BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  // pAdvertising->addServiceUUID(BLE_UUID_SENSOR_SERVICE);
  // pServer->getAdvertising()->start();

  pAdvertising = pServer->getAdvertising();
  advert.setName("ESP32-new");
  pAdvertising->setAdvertisementData(advert);
  pAdvertising->start();
}



void init_Timer(){
  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(0, 80, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &onTimer, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, 2000000, true);

  /* Start an alarm */
  timerAlarmEnable(timer);
  }


void setup() {
  Serial.begin(9600);
  init_pin();
  init_BLE();
  // init_Timer();

  Serial.println(":: >>> Boot <<<");
  Serial.printf(":: ESP.getChipCores:     %d\n", ESP.getChipCores());
  Serial.printf(":: ESP.getChipModel:     %d\n", ESP.getChipModel());
  Serial.printf(":: ESP.getCpuFreqMHz:    %d\n", ESP.getCpuFreqMHz());
  Serial.printf(":: ESP.getCycleCount:    %d\n", ESP.getCycleCount());
  Serial.printf(":: ESP.getEfuseMac:      %d\n", ESP.getEfuseMac());
  Serial.printf(":: ESP.getFlashChipMode: %d\n", ESP.getFlashChipMode());
  Serial.printf(":: ESP.getFlashChipSize: %d\n", ESP.getFlashChipSize());

uint32_t Freq = 0;
  // setCpuFrequencyMhz(10);
  Freq = getCpuFrequencyMhz();
  Serial.print("CPU Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getXtalFrequencyMhz();
  Serial.print("XTAL Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getApbFrequency();
  Serial.print("APB Freq = ");
  Serial.print(Freq);
  Serial.println(" Hz");
  // adc_power_off();
}


void loop() {
int i=0;

digitalWrite(A0,!digitalRead(A0));
//   // Characteristic_Sensor_UpTime.setValue(millis());
//   // Characteristic_Sensor_UpTime.notify();
//   // PrintSensor();
//   if (Sensor_Resistance_Value_expired){
//     Sensor_ResistanceUpdate();
//     Sensor_Resistance_Value_expired = false;
//     Sensor_UpdateAdvertisingData = true;
//   }

//   if(Sensor_Battery_Value_expired){
//     Sesnosr_BatteryUpdate();
//     Sensor_Battery_Value_expired = false;
//     Sensor_UpdateAdvertisingData = true;
//   }

//   if(Sensor_Temperature_Value_expired){
//     Sensor_TemperatureUpdate();
//     Sensor_Temperature_Value_expired = false;
//     Sensor_UpdateAdvertisingData = true;
//   }

//   if(Sensor_UpdateAdvertisingData){
//     UpdateAdvertisingData();
//     Sensor_UpdateAdvertisingData = false;
//   }

//   if(ALARM_MODE){
//     // if alarmis on
//     // blinking and make noise
//     if ( (millis() - lastAlarm) > 150 ){
//       digitalWrite(PIN_ALARM_LED  , !digitalRead(PIN_ALARM_LED));
//       digitalWrite(PIN_ALARM_BEEP , !digitalRead(PIN_ALARM_BEEP));
//       lastAlarm = millis();
//     }
    
//   }else if( ALARM_MODE == false && lastAlarm !=0 ){
//     // if alarm stop
//     digitalWrite(PIN_ALARM_LED, LOW);
//     digitalWrite(PIN_ALARM_BEEP, LOW);
//     ALARM_MODE = false;
//     lastAlarm  = 0;
//   }else{
//     lastAlarm  = 0;
//     ALARM_MODE = false;


//   }


// // esp_sleep_enable_timer_wakeup(5*1000000 );
// // delay(100);
// // esp_light_sleep_start();
  delay(1000);
}