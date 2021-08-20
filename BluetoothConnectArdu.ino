
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>


typedef struct __attribute__( ( packed ) )
{
  float accX;
 float accY;
 float accZ;
 float angVelX;
 float angVelY;
 float angVelZ;
} m5600_data_t;

typedef union
{
  m5600_data_t values;
  uint8_t bytes[ 24 ];
} m5600_data_ut;

m5600_data_ut m5600Data;

// Declare Bluetooth service name, and characteristics. All are standard GATT services.
BLEService environmentalSensingService("181A");

BLECharacteristic allCharacteristic( "0011", BLERead | BLENotify, sizeof m5600Data.bytes ); // on the peripheral side
int cou = 0;
// Error pulse of death. Loops forever. Only used if there are problems.
void error_pulse() {
  while (1) {
    for (int i=0; i<3; i++) {
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
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("Incoming connection from: ");
    Serial.println(central.address());

    while (central.connected()) {
      // Get readings from sensors and update the charcteristic values.

       float accX, accY, accZ;
       cou = cou + 1;
       Serial.println("counter is: ");
       Serial.println(cou);
       if (IMU.accelerationAvailable()) {
              IMU.readAcceleration(accX, accY, accZ);
              Serial.print("Accelraton: ");
              Serial.print(accX);
              Serial.print(',');
              Serial.print(accY);
              Serial.print(',');
              Serial.println(accZ);
        }
      float angVelX, angVelY, angVelZ;
      if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(angVelX, angVelY, angVelZ);
            Serial.print("Angular Velocity: ");
            Serial.print(angVelX);
            Serial.print(',');
            Serial.print(angVelY);
            Serial.print(',');
            Serial.println(angVelZ);
      }
      m5600Data.values.accX = accX;
      m5600Data.values.accY = accY;
      m5600Data.values.accZ = accZ;
      m5600Data.values.angVelX = angVelX;
      m5600Data.values.angVelY = angVelY;
      m5600Data.values.angVelZ = angVelZ;
      // Update Bluetooth characteristics with new values.
        allCharacteristic.writeValue( m5600Data.bytes, 24 );
      // Delay between updates. (Don't make too long of connections start to timeout.)
      delay(50);
    }

    // Turn off LED when connection is dropped. 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
}
