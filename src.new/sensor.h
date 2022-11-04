
uint32_t bootcount;    // remember number of boots in RTC Memory
RTC_DATA_ATTR static time_t last;           // remember last boot in RTC Memory

volatile uint32_t lastTimer=0;

volatile uint16_t Vin = 3000; // mV  Main voltage supply on voltage divider; 
volatile uint16_t Sensor_Resistance_CheckPeriod   = 1 ;   // evry 1 second
volatile uint16_t Sensor_Resistance_LastCheck     = 0;    
volatile bool     Sensor_Resistance_Value_expired = true;

volatile uint16_t Sensor_Battery_CheckPeriod   = 10;      // every 30 sec. 
volatile uint16_t Sensor_Battery_LastCheck     = 0;
volatile bool     Sensor_Battery_Value_expired = true;
 
volatile uint16_t Sensor_Temperature_CheckPeriod   = 15;      // every 30 sec. 
volatile uint16_t Sensor_Temperature_LastCheck     = 0;
volatile bool     Sensor_Temperature_Value_expired = true;

volatile bool     Sensor_UpdateAdvertisingData = false;
volatile int      ALARM_MODE = 0;



#define PIN_ALARM_BEEP  A2
#define PIN_ALARM_LED   A0
#define PIN_BUTTON TX
#define PIN_BEEP   SCL

volatile uint32_t lastAlarm = 0;


//define BLE UUID characteristics

#define BLE_UUID_SENSOR_SERVICE       "3460afd3-92e4-4bf8-b6dd-c239277a46b4"    //
#define BLE_UUID_SENSOR_STATUS        "2881687f-bf2e-4b0a-b0f5-15a65cccb432"
#define BLE_UUID_SENSOR_VALUE         "5f7087eb-785b-4906-8a62-460c9a379cc1"
#define BLE_UUID_SENSOR_VALUE_SET     "0d872907-0fc2-4e81-beb2-238f4ef3ae8d"    // 
#define BLE_UUID_SENSOR_ALARM         "4aaa4483-adc6-414b-aca7-f3dabe253c36"
#define BLE_UUID_SENSOR_ALARM_RESET   "c7d2ded0-26aa-4a54-bfe1-18553aa774d8"
#define BLE_UUID_SENSOR_UPDATE_PERIOD "db68db01-e0dd-411d-859d-32a2ef68811f"
#define BLE_UUID_SENSOR_RESET         "db68db01-e0dd-411d-859d-32a2ef68811g"
#define BLE_UUID_SENSOR_UPTIME        "c2493ab8-2d30-4b8e-adef-7a023fd1d57c"
#define BLE_UUID_SENSOR_TEMPERATURE   "0x2A1E"


BLEAdvertisementData advert;
BLEAdvertising *pAdvertising;

// Create BLE Descriptor
// STATUS
BLECharacteristic Characteristic_Sensor_Status(BLE_UUID_SENSOR_STATUS, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor     Descripto_Sensor_Status(BLEUUID((uint16_t)0x2902));
// Value
BLECharacteristic Characteristic_Sensor_Value(BLE_UUID_SENSOR_VALUE, BLECharacteristic::PROPERTY_NOTIFY|BLECharacteristic::PROPERTY_WRITE|BLECharacteristic::PROPERTY_READ);
BLEDescriptor     Descripto_Sensor_Value(BLEUUID((uint16_t)0x2902));
// Value_set
BLECharacteristic Characteristic_Sensor_Value_Set(BLE_UUID_SENSOR_VALUE_SET, BLECharacteristic::PROPERTY_NOTIFY|BLECharacteristic::PROPERTY_WRITE|BLECharacteristic::PROPERTY_READ);
BLEDescriptor     Descripto_Sensor_Value_Set(BLEUUID((uint16_t)0x2902));
// ALARM
BLECharacteristic Characteristic_Sensor_Alarm(BLE_UUID_SENSOR_ALARM, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor     Descripto_Sensor_Alarm(BLEUUID((uint16_t)0x2902));
// ALARM
BLECharacteristic Characteristic_Sensor_Alarm_Reset(BLE_UUID_SENSOR_ALARM_RESET, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor     Descripto_Sensor_Alarm_Reset(BLEUUID((uint16_t)0x2902));
// UPDATE_PERIOD
BLECharacteristic Characteristic_Sensor_Update_Period(BLE_UUID_SENSOR_UPDATE_PERIOD, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor     Descripto_Sensor_Update_Period(BLEUUID((uint16_t)0x2902));
// RESET
BLECharacteristic Characteristic_Sensor_Reset(BLE_UUID_SENSOR_RESET, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor     Descripto_Sensor_Reset(BLEUUID((uint16_t)0x2902));
// TEMPERATURE
BLECharacteristic Characteristic_Sensor_Temperature(BLE_UUID_SENSOR_TEMPERATURE, BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor     Descripto_Sensor_Temperature(BLEUUID((uint16_t)0x2902));
// UPTIME
BLECharacteristic Characteristic_Sensor_UpTime(BLE_UUID_SENSOR_UPTIME, BLECharacteristic::PROPERTY_NOTIFY|BLECharacteristic::PROPERTY_READ);
BLEDescriptor     Descripto_Sensor_UpTime(BLEUUID((uint16_t)0x2902));

bool deviceConnected = false;






//define EEPROM size
#define EEPROMSize  512

struct Info{
    volatile uint8_t  Status;
    volatile uint32_t dR;               // pochodna zmiany rezystancji 
    volatile uint32_t SdR;              // średnia z osttanich N pomiarów
    volatile uint32_t R;                // wartość rezystancji z obecnego pomiaru
    
    volatile uint32_t R_Last;           // wartość rezystancji z osttaniego pomiaru
    volatile uint32_t SR;               // średnia z osttanich N pomiarów
    volatile uint8_t  N;                // ilość pomiarów do wyliczania średniej SR
    volatile uint32_t nn;               // ilośc pomiarów do wyliczania wartosci R
    volatile uint32_t t;                // wartość czasu z obecnego pomiaru
    volatile uint32_t t_last;           // wartość czasu z obecnego pomiaru
    volatile uint32_t dt;               // czas pomiedzy pomiarami
    volatile uint32_t t_lastlast;       // wartośc czasu z ostatniego pomiaru
    
    bool isArmed;
    double Resistance;
    double ResistanceZero;
    double Resistance_percentage;
    double ResistanceMAX;
    volatile float_t Temperature;
    volatile uint32_t BatteryLevel;
    volatile uint32_t UpTime;
    volatile bool ALARM;
} ;