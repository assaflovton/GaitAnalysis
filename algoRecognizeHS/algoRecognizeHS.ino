
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#define NONE 0
#define MS 1
#define HS 2
#define TF 3

int state = 1;
float accX, accY, accZ;
float angVelX, angVelY, angVelZ;
float last_ang_velY1 = 0 , last_ang_velY2 = 0;

typedef struct __attribute__( ( packed ) )
{
  //float accX;
  //float accY;
  //float accZ;
  //float angVelX;
  float angVelY1;
  float angVelY2;
  float angVelY3;
  float angVelY4;
  float angVelY5;
  float angVelY6;
  float angVelY7;
  float angVelY8;
  float angVelY9;
  float angVelY10;
  float angVelY11;
  float angVelY12;
  float angVelY13;
  float angVelY14;
  float angVelY15;
  float angVelY16;
  
  
  
  
  //float angVelZ;
} m5600_data_t;

typedef union
{
  m5600_data_t values;
  uint8_t bytes[ 64 ];
} m5600_data_ut;

m5600_data_ut m5600Data;

// Declare Bluetooth service name, and characteristics. All are standard GATT services.
BLEService environmentalSensingService("181A");

BLECharacteristic allCharacteristic( "0011", BLERead | BLENotify, sizeof m5600Data.bytes ); // on the peripheral side

// Error pulse of death. Loops forever. Only used if there are problems.
void error_pulse() {
  while (1) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(166);
      digitalWrite(LED_BUILTIN, LOW);
      delay(167);
    }
    delay(1001);
  }
}

void setup() {
  // Illuminate LED for 5sec to indicate startup.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);

  // Initialize serial for debug output.
  Serial.begin(9600);

  // Initialize atmospheric sensors.
  Serial.println("Initializing accelometer and gyroscope sensors.");
  if (!IMU.begin()) {
    Serial.println("Failed.");
    error_pulse();
  }

  // Initialize Bluetooth communication.
  Serial.println("Initializing Bluetooth communication.");
  if (!BLE.begin()) {
    Serial.println("Failed.");
    error_pulse();
  }

  // Set up Bluetooth Environmental Sensing service.
  Serial.println("Setting up service with characteristics.");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(environmentalSensingService);

  environmentalSensingService.addCharacteristic(allCharacteristic);

  // Make the service available.
  BLE.addService(environmentalSensingService);
  BLE.setConnectable(true);
  Serial.println("Advertising services.");
  BLE.advertise();

  // Turn off LED to indicate startup is complete.
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  // Wait for a connection from a central.
  BLEDevice central = BLE.central();

  // When a connection is made, activate LED and write address to serial for debug.
  if (central) {

    //digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("Incoming connection from: ");
    Serial.println(central.address());

float angVelY1,angVelY2, angVelY3, angVelY4, angVelY5, angVelY6, angVelY7, angVelY8;
float angVelY9,angVelY10, angVelY11, angVelY12, angVelY13, angVelY14, angVelY15, angVelY16;

    while (central.connected()) {
      // Get readings from sensors and update the charcteristic values.
     
        if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY1, angVelZ); }
         m5600Data.values.angVelY1 = angVelY1;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY2, angVelZ);
         }
         m5600Data.values.angVelY2 = angVelY2;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY3, angVelZ);
            m5600Data.values.angVelY3 = angVelY3;
         }
        
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY4, angVelZ);
       
         }
         m5600Data.values.angVelY4 = angVelY4;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY5, angVelZ);
       
         }
         m5600Data.values.angVelY5 = angVelY5;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY6, angVelZ);
       
         }
         m5600Data.values.angVelY6 = angVelY6;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY7, angVelZ);
       
         }
         m5600Data.values.angVelY7 = angVelY7;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY8, angVelZ);
       
         }
         m5600Data.values.angVelY8 = angVelY8;
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY9, angVelZ); }
         m5600Data.values.angVelY9 = angVelY9;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY10, angVelZ);
         }
         m5600Data.values.angVelY10 = angVelY10;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY11, angVelZ);
            m5600Data.values.angVelY11 = angVelY11;
         }
        
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY12, angVelZ);
       
         }
         m5600Data.values.angVelY12 = angVelY12;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY13, angVelZ);
       
         }
         m5600Data.values.angVelY13 = angVelY13;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY14, angVelZ);
       
         }
         m5600Data.values.angVelY14 = angVelY14;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY15, angVelZ);
       
         }
         m5600Data.values.angVelY15 = angVelY15;
         delay(10);
         if (IMU.gyroscopeAvailable()) {
          IMU.readGyroscope(angVelX, angVelY16, angVelZ);
       
         }
         m5600Data.values.angVelY16 = angVelY16;


     
     m5600Data.values.angVelY1 = angVelY1;
     m5600Data.values.angVelY2 = angVelY2;
     m5600Data.values.angVelY3 = angVelY3;
     m5600Data.values.angVelY4 = angVelY4;
     m5600Data.values.angVelY5 = angVelY5;
     m5600Data.values.angVelY6 = angVelY6;
     m5600Data.values.angVelY7 = angVelY7;
     m5600Data.values.angVelY8 = angVelY8;
     m5600Data.values.angVelY9 = angVelY9;
     m5600Data.values.angVelY10 = angVelY10;
     m5600Data.values.angVelY11 = angVelY11;
     m5600Data.values.angVelY12 = angVelY12;
     m5600Data.values.angVelY13 = angVelY13;
     m5600Data.values.angVelY14 = angVelY14;
     m5600Data.values.angVelY15 = angVelY15;
     m5600Data.values.angVelY16 = angVelY16;
     
      // Update Bluetooth characteristics with new values.

     allCharacteristic.writeValue( m5600Data.bytes, 64 );
//      // Delay between updates. (Don't make too long of connections start to timeout.)
//
//      if (angVelY < last_ang_velY1 and last_ang_velY2 < last_ang_velY1 and angVelY > 50) { //max
//        state = MS;
//      }
//      
//      if (angVelY > last_ang_velY1 and last_ang_velY2 > last_ang_velY1 and angVelY < 0 and state == MS ) { //min
//        //Serial.println("got HS");
//        state = HS;
//        digitalWrite(LED_BUILTIN, HIGH);
//        delay(100);
//        digitalWrite(LED_BUILTIN, LOW);
//        delay(100);
//
//      }
//
//      if (angVelY > last_ang_velY1 and last_ang_velY2 > last_ang_velY1 and angVelY < -50 and state == HS ) { //min
//        state = TF;
//        //Serial.println("got TF");
//      }
//
//      last_ang_velY2 = last_ang_velY1;
//      last_ang_velY1 = angVelY;
      delay(10);
    //}

    // Turn off LED when connection is dropped.


    //digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
  }
}
  
