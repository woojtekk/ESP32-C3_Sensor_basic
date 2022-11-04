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
#include <GyverINA.h>
<<<<<<< HEAD

=======
#include "Version.h"
///////////////////////
///////////////////////
///////////////////////
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d

hw_timer_t * timer = NULL;
INA219 ina;                       // Стандартный набор параметров для Arduino модуля (0.1, 3.2, 0x40)



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

void IRAM_ATTR onTimer_HB(){
  digitalWrite(PIN_ALARM_LED, !digitalRead(PIN_ALARM_LED));
  delayMicroseconds(200);
  digitalWrite(PIN_ALARM_LED, !digitalRead(PIN_ALARM_LED));
}

void IRAM_ATTR onTimer(){

  uint32_t tmp = micros();
  // Serial.printf(">> %d\n",tmp-lastTimer);
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

void init_Timer(){
  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(1, 80, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &onTimer, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, 1000000, true);

  /* Start an alarm */
  timerAlarmEnable(timer);
  }

void init_Timer_HB(){
  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(1, 80, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &onTimer_HB, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, (1000000), true);

  /* Start an alarm */
  timerAlarmEnable(timer);
  }



void Sensor_UpTimeUpdate(){
  float_t tmp = millis();
  
}


float getR(){
  float u = abs(ina.getVoltage());
  float i = abs(ina.getCurrent());
  float r = u/i;


  return r*100.0;
}
long getRR(uint8_t nn){
  int n = nn;
 
  long RR = 0;
  long LR = 0;
  long SR = 0;

  while(n==0){
    if(RR==0 and LR==0) RR=LR=getR();
    else RR=getR();

    if (abs( (long)(RR-LR) )  <= LR *2){  
      SR = SR + RR;

      LR=RR;
      n--;
    }
  }  

  SR = ( RR / nn );
  Serial.println(SR);
  return SR;
}


<<<<<<< HEAD
uint32_t tmp2=0;
=======

>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
void Sensor_ResistanceUpdate(){
  // Serial.println(":: [F] Sensor_ResistanceUpdate()");
  digitalWrite(PIN_ALARM_BEEP, !digitalRead(PIN_ALARM_BEEP));
  delayMicroseconds(100);
  digitalWrite(PIN_ALARM_BEEP, !digitalRead(PIN_ALARM_BEEP));
<<<<<<< HEAD
  // uint32_t tmp =SensorInfo.Resistance+5;// (getR()+getR()+getR())/3;
  // Serial.println(tmp);
 int sensorValue = analogRead(A1);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (2.6 / 4095.0);
  float voltage2 = analogReadMilliVolts(A1);
  float vv=0;
  for(int x=0;x<100;x++){
    vv+=analogReadMilliVolts(A1);
  }
  voltage2=vv/100;

  float Rx = voltage / 0.104;
  float Rx2 = voltage2 / 0.104;


  tmp2+=11;
  uint32_t tmp = tmp2;



=======
  uint32_t tmp = (getR()+getR()+getR())/3;
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
  if (SensorInfo.Resistance != tmp){
    SensorInfo.Resistance = tmp;

    if(SensorInfo.isArmed){
      if(SensorInfo.Resistance >= SensorInfo.ResistanceMAX) ALARM_MODE=1;
      else if(SensorInfo.Resistance >= SensorInfo.ResistanceMAX*0.8 ) ALARM_MODE=2;
      else ALARM_MODE=0;

    }else{
      ALARM_MODE=0;
    }

    Serial.print(millis());
    Serial.print(" :: ");
    Serial.print(tmp);
    Serial.print(" ::: ");
    Serial.print(SensorInfo.ResistanceMAX,0);
    Serial.print(" ::: ");
    Serial.println(ALARM_MODE);
    
    
    
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
  // Serial.println(":: [F] Sensor_UpdateAdvertisingData()");
<<<<<<< HEAD
  // char txString[8];
  // dtostrf(SensorInfo.Resistance, 1, 2, txString);
  Sesnosr_BatteryUpdate();
  
  Characteristic_Sensor_Value.setValue(SensorInfo.Resistance);
  Characteristic_Sensor_Value.notify();
  uint32_t uptime = millis()/1000;
  Characteristic_Sensor_UpTime.setValue(uptime);
=======
  char txString[8];
  dtostrf(SensorInfo.Resistance, 1, 2, txString);
  float oo= SensorInfo.Resistance/100;
  Characteristic_Sensor_Value.setValue(oo);
  Characteristic_Sensor_Value.notify();
  

  oo = millis()/1000;
  Characteristic_Sensor_UpTime.setValue(oo);
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
  Characteristic_Sensor_UpTime.notify();

  
  if(ALARM_MODE==2)  Characteristic_Sensor_Status.setValue("WARNING");
  if(ALARM_MODE==1)  Characteristic_Sensor_Status.setValue("ALARM");
  if(ALARM_MODE==0)  Characteristic_Sensor_Status.setValue("NORMAL");
  Characteristic_Sensor_Status.notify();
  
<<<<<<< HEAD
  
  Characteristic_Sensor_Value_Set.setValue(SensorInfo.ResistanceMAX);
=======
  oo = SensorInfo.ResistanceMAX/100;
  Characteristic_Sensor_Value_Set.setValue(oo);
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
  Characteristic_Sensor_Value_Set.notify();

}

void IRAM_ATTR isr() {
  if(ALARM_MODE!=0) ALARM_MODE=0;
<<<<<<< HEAD
  // // SensorInfo.isArmed=!SensorInfo.isArmed;
  // if(SensorInfo.isArmed){
  //   if(ALARM_MODE!=0) ALARM_MODE=0;
  //   SensorInfo.isArmed=false;
  // }else{
  //   SensorInfo.isArmed=true;
  // }
}

void init_pin(){
  // pinMode(PIN_ALARM_LED  , OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BEEP  , OUTPUT);
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_ALARM_LED,OUTPUT);
  // pinMode(A0  , OUTPUT);
=======
}

void init_pin(){
  pinMode(PIN_ALARM_LED  , OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BEEP  , OUTPUT);
  pinMode(A0  , OUTPUT);
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
  // adc_power_off();

digitalWrite(PIN_ALARM_BEEP,HIGH);
digitalWrite(PIN_ALARM_LED,HIGH);
delay(100);
digitalWrite(PIN_ALARM_BEEP,LOW);
digitalWrite(PIN_ALARM_LED,LOW);
attachInterrupt(PIN_BUTTON, isr, FALLING);
  
}

void init_flash(){
  digitalWrite(PIN_BEEP,HIGH);
  // digitalWrite(PIN_ALARM_LED,HIGH);  
  delay(250);
  digitalWrite(PIN_BEEP,LOW);
  // digitalWrite(PIN_ALARM_LED,LOW);  
}

void init_Sensor(){
  SensorInfo.Status        = 0;   // uint8_t 0 --> 255
  SensorInfo.ALARM         = 0;
  SensorInfo.dR            = 0; 
  SensorInfo.Resistance    = 0;
<<<<<<< HEAD
  SensorInfo.ResistanceMAX = 10*1000;
=======
  SensorInfo.ResistanceMAX = 10*100;
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
  SensorInfo.Temperature   = 0;
  SensorInfo.BatteryLevel  = 0;
  SensorInfo.isArmed       = true;

}


void PrintSensor(){
  Serial.printf(":: Resistance: %d \t %d \n", SensorInfo.Resistance, SensorInfo.ResistanceMAX);
}


//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println(":: BLE Connect");
    // digitalWrite(PIN_ALARM_LED,HIGH);
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println(":: BLE Disconnect");
    Serial.println(":: >> Restart Advertising ..... ");
    pServer->getAdvertising()->start();
    // digitalWrite(PIN_ALARM_LED,LOW);

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
      std::string value = pCharacteristic->getValue();
      uint32_t val1 = ((value[3]<<24))|((value[2]<<16))|((value[1]<<8))|((value[0]));
      
      Serial.print(":: Sensor_Value_Set_Callbacks >> onWrite >> ");
<<<<<<< HEAD
      Serial.println(val1);
      SensorInfo.ResistanceMAX = val1;

=======
      Serial.println(*received_data);
      SensorInfo.ResistanceMAX = *received_data*100;
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
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
<<<<<<< HEAD
   String name=(String)(ESP.getEfuseMac()) ;
   String SS ="SENSOR_"+(name.substring(name.length()-4,name.length()));
   char tmp[25];
  SS.toCharArray(tmp,25);
  BLEDevice::init(tmp);
=======
   char name[25];
   String SS ="SENSOR"+(String)(ESP.getEfuseMac()) ;
  SS.toCharArray(name,25);
  BLEDevice::init(name);
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d

  // Create the BLE Server
  BLEServer  *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(BLE_UUID_SENSOR_SERVICE);


  // Create BLE Characteristics and Create a BLE Descriptor
    // STATUS
  // pService->addCharacteristic(&Characteristic_Sensor_Battery);
  // Characteristic_Sensor_Battery.setValue("0");

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
  // advert.setName("ESP32-new");
  // pAdvertising->setAdvertisementData(advert);


  pAdvertising->start();

}




void setup() {
  // Serial.begin(9600);
  init_pin();
  init_BLE();
  // init_Timer();
  init_Sensor();
  // init_Timer_HB();
  // setCpuFrequencyMhz(40);
<<<<<<< HEAD
  // if (ina.begin()) {
  //   Serial.println(F("connected!"));
  // } else {
  //   Serial.println(F("not found!"));
  //   while (1);
  // }
  // Serial.print(F("Calibration value: ")); Serial.println(ina.getCalibration());
  // ina.setResolution(INA219_VSHUNT, INA219_RES_12BIT_X128);  
=======
  if (ina.begin()) {
    Serial.println(F("connected!"));
  } else {
    Serial.println(F("not found!"));
    while (1);
  }
  Serial.print(F("Calibration value: ")); Serial.println(ina.getCalibration());
  ina.setResolution(INA219_VSHUNT, INA219_RES_12BIT_X128);  
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
  
  Serial.println("");
  Serial.println(":: >>> Boot <<<");
  Serial.printf(":: ESP.getChipCores:     %d\n", ESP.getChipCores());
  Serial.printf(":: ESP.getChipModel:     %d\n", ESP.getChipModel());
  Serial.printf(":: ESP.getCpuFreqMHz:    %d\n", ESP.getCpuFreqMHz());
  Serial.printf(":: ESP.getCycleCount:    %d\n", ESP.getCycleCount());
  Serial.printf(":: ESP.getEfuseMac:      %d\n", ESP.getEfuseMac());
  Serial.printf(":: ESP.getFlashChipMode: %d\n", ESP.getFlashChipMode());
  Serial.printf(":: ESP.getFlashChipSize: %d\n", ESP.getFlashChipSize());
  
  }




int i=0;

void loop() {
  
  if(i>=Sensor_Resistance_CheckPeriod){
    Sensor_Resistance_Value_expired=true;
    i=0;
  }else i++;


  if (Sensor_Resistance_Value_expired){
    onTimer_HB();
    Sensor_ResistanceUpdate();
    Sensor_Resistance_Value_expired = false;
    Sensor_UpdateAdvertisingData = true;
  }


  if(Sensor_UpdateAdvertisingData){
    UpdateAdvertisingData();
    Sensor_UpdateAdvertisingData = false;
  }

  if(ALARM_MODE==1){
    // if alarmis on
    // blinking and make noise
    if ( (millis() - lastAlarm) > 150 ){
      digitalWrite(PIN_ALARM_LED  , !digitalRead(PIN_ALARM_LED));
      digitalWrite(PIN_ALARM_BEEP , !digitalRead(PIN_ALARM_BEEP));
      lastAlarm = millis();
    }
    
  }else if( ALARM_MODE == 0 && lastAlarm !=0 ){
    // if alarm stop
    digitalWrite(PIN_ALARM_LED, LOW);
    digitalWrite(PIN_ALARM_BEEP, LOW);
    ALARM_MODE = false;
    lastAlarm  = 0;
  }else{
    lastAlarm  = 0;
    ALARM_MODE = 0;


  }

// digitalWrite(PIN_ALARM_LED, !digitalRead(PIN_ALARM_LED));
// // esp_sleep_enable_timer_wakeup(5*1000000 );
// // delay(100);
// // esp_light_sleep_start();
  // xx--;
  // Characteristic_Sensor_Status.setValue(xx);
  // Characteristic_Sensor_Status.notify();
  delay(500);
<<<<<<< HEAD
}
=======
}
>>>>>>> 85422337c39cafaecf8b6a461934900eb3373a7d
