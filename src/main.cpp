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

void Sensor_ResistanceUpdate(){
  Serial.println(":: [F] Sensor_ResistanceUpdate()");
}

void Sesnosr_BatteryUpdate(){
  Serial.println(":: [F] Sesnosr_BatteryUpdate()");
  float_t tmp = 666;
  if (SensorInfo.BatteryLevel != tmp){
    SensorInfo.BatteryLevel = tmp;
    Sensor_Battery_Value_expired = true; 
  }
}

void Sensor_TemperatureUpdate(){
  Serial.println(":: [F] Sensor_TemperatureUpdate()");
  float_t tmp = temperatureRead();
  if (SensorInfo.Temperature != tmp){
    SensorInfo.Temperature = tmp;
    Sensor_Temperature_Value_expired = true; 
  }
}

void UpdateAdvertisingData(){
  Serial.println(":: [F] Sensor_UpdateAdvertisingData()");
}

void init_pin(){
  pinMode(PIN_ALARM_LED  , OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BEEP  , OUTPUT);
  
}

void init_flash(){
  digitalWrite(PIN_BEEP,HIGH);
  digitalWrite(PIN_ALARM_LED,HIGH);  
  delay(250);
  digitalWrite(PIN_BEEP,LOW);
  digitalWrite(PIN_ALARM_LED,LOW);  
}

void init_Sensor(){
  SensorInfo.ALARM         = false;
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
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    pServer->getAdvertising()->start();
  }
};
class Sesnor_Status_Callbacks: public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic *pCharacteristic)
    {
      uint32_t* received_data = pCharacteristic->getData();
      Serial.println("Sesnor_Status_Callbacks >> onWrite ");
      Serial.print("Sesnor_Status_Callbacks >>");
      Serial.println(*received_data);
      pCharacteristic.
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
  pService->addCharacteristic(&Characteristic_Sesnor_Status);
  Descripto_Sesnor_Status.setValue("Sesnor Status");
  Characteristic_Sesnor_Status.addDescriptor(&Descripto_Sesnor_Status);
  Characteristic_Sesnor_Status.setCallbacks(new Sesnor_Status_Callbacks());
  // Characteristic_Sesnor_Status.setValue("666");
  // // Value
  // pService->addCharacteristic(&Characteristic_Sesnor_Value);
  // Descripto_Sesnor_Value.setValue("Sesnor Value");
  // Characteristic_Sesnor_Value.addDescriptor(&Descripto_Sesnor_Value);

  // // Value SET
  // pService->addCharacteristic(&Characteristic_Sesnor_Value_Set);
  // Descripto_Sesnor_Value_Set.setValue("Sesnor Value Set");
  // Characteristic_Sesnor_Value_Set.addDescriptor(&Descripto_Sesnor_Value_Set);

  // // ALARM
  // pService->addCharacteristic(&Characteristic_Sesnor_Alarm);
  // Descripto_Sesnor_Alarm.setValue("Sesnor Alarm");
  // Characteristic_Sesnor_Alarm.addDescriptor(&Descripto_Sesnor_Alarm);

  // // Value
  // pService->addCharacteristic(&Characteristic_Sesnor_Alarm_Reset);
  // Descripto_Sesnor_Alarm_Reset.setValue("Sesnor Alarm Reset");
  // Characteristic_Sesnor_Alarm_Reset.addDescriptor(&Descripto_Sesnor_Alarm_Reset);

  // // Update Period
  // pService->addCharacteristic(&Characteristic_Sesnor_Update_Period);
  // Descripto_Sesnor_Update_Period.setValue("Sesnor Update Period");
  // Characteristic_Sesnor_Update_Period.addDescriptor(&Descripto_Sesnor_Update_Period);

  // // Reset
  // pService->addCharacteristic(&Characteristic_Sesnor_Reset);
  // Descripto_Sesnor_Reset.setValue("Sesnor Alarm Reset");
  // Characteristic_Sesnor_Reset.addDescriptor(&Descripto_Sesnor_Reset);

  // Temperature
  // pService->addCharacteristic(&Characteristic_Sesnor_Temperature);
  // Descripto_Sesnor_Temperature.setValue("Sesnor Temperature");
  // Characteristic_Sesnor_Temperature.addDescriptor(&Descripto_Sesnor_Temperature);


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






void setup() {
  Serial.begin(9600);
  init_pin();
  init_BLE();
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
  adc_power_off();
}


void loop() {
  // PrintSensor();
  if (Sensor_Resistance_Value_expired){
    Sensor_ResistanceUpdate();
    Sensor_Resistance_Value_expired = false;
    Sensor_UpdateAdvertisingData = true;
  }

  if(Sensor_Battery_Value_expired){
    Sesnosr_BatteryUpdate();
    Sensor_Battery_Value_expired = false;
    Sensor_UpdateAdvertisingData = true;
  }

  if(Sensor_Temperature_Value_expired){
    Sensor_TemperatureUpdate();
    Sensor_Temperature_Value_expired = false;
    Sensor_UpdateAdvertisingData = true;
  }

  if(Sensor_UpdateAdvertisingData){
    UpdateAdvertisingData();
  }

  if(ALARM_MODE){
    // if alarmis on
    // blinking and make noise
    if ( (millis() - lastAlarm) > 150 ){
      digitalWrite(PIN_ALARM_LED  , !digitalRead(PIN_ALARM_LED));
      digitalWrite(PIN_ALARM_BEEP , !digitalRead(PIN_ALARM_BEEP));
      lastAlarm = millis();
    }
    
  }else if( ALARM_MODE == false && lastAlarm !=0 ){
    // if alarm stop
    digitalWrite(PIN_ALARM_LED, LOW);
    digitalWrite(PIN_ALARM_BEEP, LOW);
    ALARM_MODE = false;
    lastAlarm  = 0;
  }else{
    lastAlarm  = 0;
    ALARM_MODE = false;


  }

// esp_sleep_enable_timer_wakeup(5*1000000 );
// delay(100);
// esp_light_sleep_start();
  delay(1000);
}