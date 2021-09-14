#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

typedef struct __attribute__( ( packed ) )
{
  float angVelY1; unsigned long time1;
  float angVelY2; unsigned long time2;
  float angVelY3; unsigned long time3;
  float angVelY4; unsigned long time4;
  float angVelY5; unsigned long time5;
  float angVelY6; unsigned long time6;
  float angVelY7; unsigned long time7;
  float angVelY8; unsigned long time8;
  float angVelY9; unsigned long time9;
  float angVelY10; unsigned long time10;
  float angVelY11; unsigned long time11;
  float angVelY12; unsigned long time12;
  float angVelY13; unsigned long time13;
  float angVelY14; unsigned long time14;
  float angVelY15; unsigned long time15;
  float angVelY16; unsigned long time16;
  float angVelY17; unsigned long time17;
  float angVelY18; unsigned long time18;
  float angVelY19; unsigned long time19;
  float angVelY20; unsigned long time20;
  float angVelY21; unsigned long time21;
  float angVelY22; unsigned long time22;
  float angVelY23; unsigned long time23;
  float angVelY24; unsigned long time24;
  float angVelY25; unsigned long time25;
  float angVelY26; unsigned long time26;
  float angVelY27; unsigned long time27;
  float angVelY28; unsigned long time28;
  float angVelY29; unsigned long time29;
  float angVelY30; unsigned long time30;
  float angVelY31; unsigned long time31;
  float angVelY32; unsigned long time32;
  float angVelY33; unsigned long time33;
  float angVelY34; unsigned long time34;
  float angVelY35; unsigned long time35;
  float angVelY36; unsigned long time36;
  float angVelY37; unsigned long time37;
  float angVelY38; unsigned long time38;
  float angVelY39; unsigned long time39;
  float angVelY40; unsigned long time40;
  float angVelY41; unsigned long time41;
  float angVelY42; unsigned long time42;
  float angVelY43; unsigned long time43;
  float angVelY44; unsigned long time44;
  float angVelY45; unsigned long time45;
  float angVelY46; unsigned long time46;
  float angVelY47; unsigned long time47;
  float angVelY48; unsigned long time48;
  float angVelY49; unsigned long time49;
  float angVelY50; unsigned long time50;
  float angVelY51; unsigned long time51;
  float angVelY52; unsigned long time52;
  float angVelY53; unsigned long time53;
  float angVelY54; unsigned long time54;
  float angVelY55; unsigned long time55;
  float angVelY56; unsigned long time56;
  float angVelY57; unsigned long time57;
  float angVelY58; unsigned long time58;
  float angVelY59; unsigned long time59;
  float angVelY60; unsigned long time60;
  float angVelY61; unsigned long time61;
  float angVelY62; unsigned long time62;
  float angVelY63; unsigned long time63;
  float angVelY64; unsigned long time64;
} m5600_data_t;

typedef union
{
  m5600_data_t values;
  uint8_t bytes[128];
} m5600_data_ut;

m5600_data_ut m5600Data;

float val_arr[2048];
unsigned long time_arr[2048];

// create a service to expose our service to BLE Central Devices
BLEService gaitDetectionService("FA01");

BLEUnsignedCharCharacteristic batteryLevelCharacteristic("2101", BLERead | BLENotify);

BLEUnsignedCharCharacteristic msgCharacteristic("2102", BLERead | BLEWrite | BLENotify);

BLECharacteristic data1Characteristic( "3101", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data2Characteristic( "3102", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data3Characteristic( "3103", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data4Characteristic( "3104", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data5Characteristic( "3105", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data6Characteristic( "3106", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data7Characteristic( "3107", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side
BLECharacteristic data8Characteristic( "3108", BLEWrite | BLERead | BLENotify, 512 ); // on the peripheral side

BLEDescriptor mydescriptor1("2103", "ABC1");
BLEDescriptor mydescriptor2("2103", "ABC2");
BLEDescriptor mydescriptor3("2103", "ABC3");

float angVelX, angVelY, angVelZ;

int INITIAL_TIME_BETWEEN_STATES = 160;
int INITIAL_AMPLITUDE = 0;
int STEP_MAX_TIME = 3000;
int RESTART_THRESH = 20000;
float TIME_FLEX = 0.8;
float VALUE_FLEX = 0.7;
float AVG_CHANGE_RATE = 0.2;
float VALID_STEP_RATIO = 0.4;
float CORRELATION_RATIO = 0.7;
float STATE_SWITCH_THRESH = 0.5;
int step_state = 3;//MS=1, HS=2, TF=3

int HStoTF = INITIAL_TIME_BETWEEN_STATES;
int MStoHS = INITIAL_TIME_BETWEEN_STATES;
int TFtoMS = INITIAL_TIME_BETWEEN_STATES;

int prev_HS = INITIAL_AMPLITUDE;
int prev_MS = INITIAL_AMPLITUDE;

int prev_state_time = 0;
int prev_step_time = 0;

int step_thresh = STEP_MAX_TIME;
int restart_time = 0;

int min_samples = 0;
int sum_avg_vel = 0;
int sum_time = 0;
int cou = 0; //counting 4 samples
int index_arr = 0; // index in avg_array
int last_5_array[5];// 0 is current, 1 is prev, 2 is 2prev
int last_times[5];
unsigned long currentMillis;
unsigned long prevMillis = 0;
unsigned long startMillis = 0;

byte msgValue = 0x02;

int commandIterations = 0;

int vib_power = 2;

bool conn = false;
bool test_written = false;
bool vibration_written = false;
bool first = false;
bool test_mode = false;
int TEST_MODE_TIME = 20000;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  pinMode(LED_BUILTIN, OUTPUT);
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
  if (!IMU.begin())
  {
    Serial.println("starting IMU failed!");
    while (1);
  }

  String address = BLE.address();
  Serial.println("Our address is [" + address + "]");

  BLE.setDeviceName("IBM Garage Opener");      // this sets Characteristic 0x2a00 of Service 0x1800
  // Service 0x1800 is the Generic Access Profile
  // Characteristic 0x2a00 is the Device Name
  // Characteristic 0x2a01 is the "Appearance"
  BLE.setAppearance(384);                      // BLE_APPEARANCE_GENERIC_REMOTE_CONTROL

  BLE.setLocalName("BLE Garage Opener");       // this sets the local name for the advertising data

  // tell the world about us
  BLE.setAdvertisedService(gaitDetectionService);
  gaitDetectionService.addCharacteristic(batteryLevelCharacteristic);
  gaitDetectionService.addCharacteristic(msgCharacteristic);
  gaitDetectionService.addCharacteristic(data1Characteristic);
  gaitDetectionService.addCharacteristic(data2Characteristic);
  gaitDetectionService.addCharacteristic(data3Characteristic);
  gaitDetectionService.addCharacteristic(data4Characteristic);
  gaitDetectionService.addCharacteristic(data5Characteristic);
  gaitDetectionService.addCharacteristic(data6Characteristic);
  gaitDetectionService.addCharacteristic(data7Characteristic);
  gaitDetectionService.addCharacteristic(data8Characteristic);

  BLE.addService(gaitDetectionService);
  int battery = analogRead(A0);
  int batteryLevel = map(battery, 0, 1023, 0, 100);
  batteryLevelCharacteristic.writeValue(batteryLevel);

  // advertise to the world so we can be found
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");

  // register new connection handler
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);

  // registeer disconnect handler
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // handle Characteristic Written Handler
  msgCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, see if care, etc.
  Serial.print("Characteristic event, written: ");
  Serial.println(characteristic.uuid());

  // see if this is the Button Characteristic
  if (characteristic.uuid() == "2102")
  {
    characteristic.readValue(msgValue);
    switch (msgValue) {
      case 0x00:
        test_mode = false;
        vib_power = 0;
        break;
      case 0x01:
        test_mode = false;
        vib_power = 1;
        break;
      case 0x10:
        test_mode = true;
        vib_power = 0;
        break;
      case 0x11:
        test_mode = true;
        vib_power = 1;
        break;
      case 0x02:
        test_mode = false;
        vib_power = 2;
        break;
      case 0x12:
        test_mode = true;
        vib_power = 2;
        break;
      case 0x03:
        test_mode = false;
        vib_power = 3;
        break;
      case 0x13:
        test_mode = true;
        vib_power = 3;
        break;


    }
    conn = true;
  }
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}


void loop()
{
  currentMillis = millis();
  if (!conn) {//no one connected yet
    //wait for coonection, only when a connection is made, send initial data and start the algo
    BLEDevice central = BLE.central();
    if (central)
    {
      while (central.connected()) {
        int battery = analogRead(A0);
        int batteryLevel = map(battery, 0, 1023, 0, 100);
        batteryLevelCharacteristic.writeValue(batteryLevel);
        if (conn) {
          break;
        }
      }
      conn = true;
      Serial.print("test is ");
      Serial.println(test_mode);
      Serial.print("vibration is ");
      Serial.println(vib_power);
    }
  }
  else {
    //--------------------------------ALGORYTHEM---------------------------------------
    if (!first) {
      startMillis = currentMillis;
      first = true;
    }
    if (currentMillis - prevMillis > 1000) {
      digitalWrite(LED_BUILTIN, LOW);
    }
    if (test_mode and (currentMillis >= TEST_MODE_TIME + startMillis)) {
      conn = false;
      Serial.print("End of data, time is");
      Serial.println(millis());
      Serial.println("start to send");
      for (int i = 0; i < 8; i++) {
        m5600Data.values.angVelY1 = val_arr[64 * i + 0], m5600Data.values.time1 = time_arr[64 * i + 0];
        m5600Data.values.angVelY2 = val_arr[64 * i + 1], m5600Data.values.time2 = time_arr[64 * i + 1];
        m5600Data.values.angVelY3 = val_arr[64 * i + 2], m5600Data.values.time3 = time_arr[64 * i + 2];
        m5600Data.values.angVelY4 = val_arr[64 * i + 3], m5600Data.values.time4 = time_arr[64 * i + 3];
        m5600Data.values.angVelY5 = val_arr[64 * i + 4], m5600Data.values.time5 = time_arr[64 * i + 4];
        m5600Data.values.angVelY6 = val_arr[64 * i + 5], m5600Data.values.time6 = time_arr[64 * i + 5];
        m5600Data.values.angVelY7 = val_arr[64 * i + 6], m5600Data.values.time7 = time_arr[64 * i + 6];
        m5600Data.values.angVelY8 = val_arr[64 * i + 7], m5600Data.values.time8 = time_arr[64 * i + 7];
        m5600Data.values.angVelY9 = val_arr[64 * i + 8], m5600Data.values.time9 = time_arr[64 * i + 8];
        m5600Data.values.angVelY10 = val_arr[64 * i + 9], m5600Data.values.time10 = time_arr[64 * i + 9];
        m5600Data.values.angVelY11 = val_arr[64 * i + 10], m5600Data.values.time11 = time_arr[64 * i + 10];
        m5600Data.values.angVelY12 = val_arr[64 * i + 11], m5600Data.values.time12 = time_arr[64 * i + 11];
        m5600Data.values.angVelY13 = val_arr[64 * i + 12], m5600Data.values.time13 = time_arr[64 * i + 12];
        m5600Data.values.angVelY14 = val_arr[64 * i + 13], m5600Data.values.time14 = time_arr[64 * i + 13];
        m5600Data.values.angVelY15 = val_arr[64 * i + 14], m5600Data.values.time15 = time_arr[64 * i + 14];
        m5600Data.values.angVelY16 = val_arr[64 * i + 15], m5600Data.values.time16 = time_arr[64 * i + 15];
        m5600Data.values.angVelY17 = val_arr[64 * i + 16], m5600Data.values.time17 = time_arr[64 * i + 16];
        m5600Data.values.angVelY18 = val_arr[64 * i + 17], m5600Data.values.time18 = time_arr[64 * i + 17];
        m5600Data.values.angVelY19 = val_arr[64 * i + 18], m5600Data.values.time19 = time_arr[64 * i + 18];
        m5600Data.values.angVelY20 = val_arr[64 * i + 19], m5600Data.values.time20 = time_arr[64 * i + 19];
        m5600Data.values.angVelY21 = val_arr[64 * i + 20], m5600Data.values.time21 = time_arr[64 * i + 20];
        m5600Data.values.angVelY22 = val_arr[64 * i + 21], m5600Data.values.time22 = time_arr[64 * i + 21];
        m5600Data.values.angVelY23 = val_arr[64 * i + 22], m5600Data.values.time23 = time_arr[64 * i + 22];
        m5600Data.values.angVelY24 = val_arr[64 * i + 23], m5600Data.values.time24 = time_arr[64 * i + 23];
        m5600Data.values.angVelY25 = val_arr[64 * i + 24], m5600Data.values.time25 = time_arr[64 * i + 24];
        m5600Data.values.angVelY26 = val_arr[64 * i + 25], m5600Data.values.time26 = time_arr[64 * i + 25];
        m5600Data.values.angVelY27 = val_arr[64 * i + 26], m5600Data.values.time27 = time_arr[64 * i + 26];
        m5600Data.values.angVelY28 = val_arr[64 * i + 27], m5600Data.values.time28 = time_arr[64 * i + 27];
        m5600Data.values.angVelY29 = val_arr[64 * i + 28], m5600Data.values.time29 = time_arr[64 * i + 28];
        m5600Data.values.angVelY30 = val_arr[64 * i + 29], m5600Data.values.time30 = time_arr[64 * i + 29];
        m5600Data.values.angVelY31 = val_arr[64 * i + 30], m5600Data.values.time31 = time_arr[64 * i + 30];
        m5600Data.values.angVelY32 = val_arr[64 * i + 31], m5600Data.values.time32 = time_arr[64 * i + 31];
        m5600Data.values.angVelY33 = val_arr[64 * i + 32], m5600Data.values.time33 = time_arr[64 * i + 32];
        m5600Data.values.angVelY34 = val_arr[64 * i + 33], m5600Data.values.time34 = time_arr[64 * i + 33];
        m5600Data.values.angVelY35 = val_arr[64 * i + 34], m5600Data.values.time35 = time_arr[64 * i + 34];
        m5600Data.values.angVelY36 = val_arr[64 * i + 35], m5600Data.values.time36 = time_arr[64 * i + 35];
        m5600Data.values.angVelY37 = val_arr[64 * i + 36], m5600Data.values.time37 = time_arr[64 * i + 36];
        m5600Data.values.angVelY38 = val_arr[64 * i + 37], m5600Data.values.time38 = time_arr[64 * i + 37];
        m5600Data.values.angVelY39 = val_arr[64 * i + 38], m5600Data.values.time39 = time_arr[64 * i + 38];
        m5600Data.values.angVelY40 = val_arr[64 * i + 39], m5600Data.values.time40 = time_arr[64 * i + 39];
        m5600Data.values.angVelY41 = val_arr[64 * i + 40], m5600Data.values.time41 = time_arr[64 * i + 40];
        m5600Data.values.angVelY42 = val_arr[64 * i + 41], m5600Data.values.time42 = time_arr[64 * i + 41];
        m5600Data.values.angVelY43 = val_arr[64 * i + 42], m5600Data.values.time43 = time_arr[64 * i + 42];
        m5600Data.values.angVelY44 = val_arr[64 * i + 43], m5600Data.values.time44 = time_arr[64 * i + 43];
        m5600Data.values.angVelY45 = val_arr[64 * i + 44], m5600Data.values.time45 = time_arr[64 * i + 44];
        m5600Data.values.angVelY46 = val_arr[64 * i + 45], m5600Data.values.time46 = time_arr[64 * i + 45];
        m5600Data.values.angVelY47 = val_arr[64 * i + 46], m5600Data.values.time47 = time_arr[64 * i + 46];
        m5600Data.values.angVelY48 = val_arr[64 * i + 47], m5600Data.values.time48 = time_arr[64 * i + 47];
        m5600Data.values.angVelY49 = val_arr[64 * i + 48], m5600Data.values.time49 = time_arr[64 * i + 48];
        m5600Data.values.angVelY50 = val_arr[64 * i + 49], m5600Data.values.time50 = time_arr[64 * i + 49];
        m5600Data.values.angVelY51 = val_arr[64 * i + 50], m5600Data.values.time51 = time_arr[64 * i + 50];
        m5600Data.values.angVelY52 = val_arr[64 * i + 51], m5600Data.values.time52 = time_arr[64 * i + 51];
        m5600Data.values.angVelY53 = val_arr[64 * i + 52], m5600Data.values.time53 = time_arr[64 * i + 52];
        m5600Data.values.angVelY54 = val_arr[64 * i + 53], m5600Data.values.time54 = time_arr[64 * i + 53];
        m5600Data.values.angVelY55 = val_arr[64 * i + 54], m5600Data.values.time55 = time_arr[64 * i + 54];
        m5600Data.values.angVelY56 = val_arr[64 * i + 55], m5600Data.values.time56 = time_arr[64 * i + 55];
        m5600Data.values.angVelY57 = val_arr[64 * i + 56], m5600Data.values.time57 = time_arr[64 * i + 56];
        m5600Data.values.angVelY58 = val_arr[64 * i + 57], m5600Data.values.time58 = time_arr[64 * i + 57];
        m5600Data.values.angVelY59 = val_arr[64 * i + 58], m5600Data.values.time59 = time_arr[64 * i + 58];
        m5600Data.values.angVelY60 = val_arr[64 * i + 59], m5600Data.values.time60 = time_arr[64 * i + 59];
        m5600Data.values.angVelY61 = val_arr[64 * i + 60], m5600Data.values.time61 = time_arr[64 * i + 60];
        m5600Data.values.angVelY62 = val_arr[64 * i + 61], m5600Data.values.time62 = time_arr[64 * i + 61];
        m5600Data.values.angVelY63 = val_arr[64 * i + 62], m5600Data.values.time63 = time_arr[64 * i + 62];
        m5600Data.values.angVelY64 = val_arr[64 * i + 63], m5600Data.values.time64 = time_arr[64 * i + 63];
        if (i == 0) {
          data1Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 1) {
          data2Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 2) {
          data3Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 3) {
          data4Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 4) {
          data5Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 5) {
          data6Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 6) {
          data7Characteristic.writeValue(m5600Data.bytes, 512);
        }
        if (i == 7) {
          data8Characteristic.writeValue(m5600Data.bytes, 512);
        }
        delay(2000);
      }
    }
    if (cou == 4) {
      if (index_arr < 2048) {
        val_arr[index_arr] = sum_avg_vel / 4.0;
        time_arr [index_arr] = currentMillis;
        index_arr++;
      }
      last_5_array[4] = last_5_array[3];
      last_5_array[3] = last_5_array[2];
      last_5_array[2] = last_5_array[1];
      last_5_array[1] = last_5_array[0];
      last_5_array[0] = sum_avg_vel / 4.0;

      last_times[4] = last_times[3];
      last_times[3] = last_times[2];
      last_times[2] = last_times[1];
      last_times[1] = last_times[0];
      last_times[0] = sum_time / 4.0;

      sum_avg_vel = 0, sum_time = 0, cou = 0;
      min_samples++;
      // the algorithm
      if (min_samples >= 5) { //array is full
        if ((last_times[2] - restart_time > RESTART_THRESH) and step_state == 3) {
          restart_time = last_times[2];
          prev_MS = INITIAL_AMPLITUDE;
          prev_HS = INITIAL_AMPLITUDE;
          TFtoMS = INITIAL_TIME_BETWEEN_STATES;
          MStoHS = INITIAL_TIME_BETWEEN_STATES;
          HStoTF = INITIAL_TIME_BETWEEN_STATES;
        }

        if ((last_times[2] - prev_step_time > step_thresh)  or ((last_5_array[2] > STATE_SWITCH_THRESH * prev_MS)
            and step_state == 2)) {
          step_state = 3;
        }
        //---------------------------------MS-----------------------------------------
        if ((last_5_array[2] > last_5_array[4]) and (last_5_array[2] > last_5_array[3]) // MS condition
            and (last_5_array[2] > last_5_array[1]) and (last_5_array[2] > last_5_array[0])
            and (last_5_array[2] > VALUE_FLEX * prev_MS) and (last_times[2] - prev_state_time > TIME_FLEX * TFtoMS)) {
          step_state = 1;
          prev_MS = last_5_array[2];
          prev_step_time = last_times[2];
          if (TFtoMS * (1 - VALID_STEP_RATIO) < (last_times[2] - prev_state_time)
              and (last_times[2] - prev_state_time < TFtoMS * (1 + VALID_STEP_RATIO))) {
            TFtoMS = ((1 - AVG_CHANGE_RATE) * TFtoMS + AVG_CHANGE_RATE * (last_times[2] - prev_state_time));
          }
          prev_state_time = last_times[2];
        }
        //---------------------------------HS-----------------------------------------------
        else if ((last_5_array[3] > last_5_array[2]) and (last_5_array[1] > last_5_array[2])
                 and step_state == 1 and ((-1 * CORRELATION_RATIO) * prev_MS < last_5_array[2]) and (last_5_array[2] < 0)
                 and (last_times[2] - prev_state_time >= TIME_FLEX * MStoHS)) {
          digitalWrite(LED_BUILTIN, HIGH);
          step_state = 2;
          prev_HS = last_5_array[2];
          if (MStoHS * (1 - VALID_STEP_RATIO) < (last_times[2] - prev_state_time) and
              (last_times[2] - prev_state_time < MStoHS * (1 + VALID_STEP_RATIO))) {
            MStoHS = ((1 - AVG_CHANGE_RATE) * MStoHS + AVG_CHANGE_RATE * (last_times[2] - prev_state_time));
          }
          prev_state_time = last_times[2];
          prevMillis = currentMillis;
        }

        //---------------------------------TF-----------------------------------------------
        else if ((last_5_array[4] > last_5_array[2]) and (last_5_array[3] > last_5_array[2])
                 and (last_5_array[1] > last_5_array[2]) and (last_5_array[0] > last_5_array[2])
                 and step_state == 2 and (last_5_array[2] < VALUE_FLEX * prev_HS)
                 and (last_times[2] - prev_state_time >= TIME_FLEX * HStoTF)) {
          step_state = 3;
          if (HStoTF * (1 - VALID_STEP_RATIO) < (last_times[2] - prev_state_time) and
              (last_times[2] - prev_state_time < HStoTF * (1 + VALID_STEP_RATIO))) {
            HStoTF = ((1 - AVG_CHANGE_RATE) * HStoTF + AVG_CHANGE_RATE * (last_times[2] - prev_state_time));
          }
          prev_state_time = last_times[2];
          digitalWrite(LED_BUILTIN, LOW);
        }
        //--------------------------------------------------------------------------------------------------
      }
    }
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(angVelX, angVelY, angVelZ);
    }
    sum_avg_vel = sum_avg_vel + angVelY;
    sum_time = sum_time + currentMillis;
    cou++;
    delay(10);
  }
}
