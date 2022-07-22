#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "sys/time.h"

#include "sensor.h"
#include <EEPROM.h>


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
  SensorInfo.ALARM        = false;
  SensorInfo.Resistance   = 0;
  SensorInfo.Temperature  = 0;
  SensorInfo.BatteryLevel = 0;

}
void setup() {
  Serial.begin(9600);
  Serial.println(":: >>> Boot <<<");
  Serial.printf(":: ESP.getChipCores:     %d\n", ESP.getChipCores());
  Serial.printf(":: ESP.getChipModel:     %d\n", ESP.getChipModel());
  Serial.printf(":: ESP.getCpuFreqMHz:    %d\n", ESP.getCpuFreqMHz());
  Serial.printf(":: ESP.getCycleCount:    %d\n", ESP.getCycleCount());
  Serial.printf(":: ESP.getEfuseMac:      %d\n", ESP.getEfuseMac());
  Serial.printf(":: ESP.getFlashChipMode: %d\n", ESP.getFlashChipMode());
  Serial.printf(":: ESP.getFlashChipSize: %d\n", ESP.getFlashChipSize());
}

int64_t time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}
void loop() {

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


  delay(10);
}