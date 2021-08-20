/*
 * Bluetooth Weather
 * A simple example of using some of the built-in sensors and LEDs in the 
 * Arduino Nano 33 Sense and transmitting values via Blutooth Low Energy.
 * Use as a learning tool or the basis of a home automation sensor.
 *
 * Copyright (c) 2020 David Horton
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>


// Declare Bluetooth service name, and characteristics. All are standard GATT services.
BLEService environmentalSensingService("181A");


BLEFloatCharacteristic accXCharacteristic("0021", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
BLEFloatCharacteristic accYCharacteristic("0022", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
BLEFloatCharacteristic accZCharacteristic("0023", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
BLEFloatCharacteristic angVelocityXCharacteristic("0031", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
BLEFloatCharacteristic angVelocityYCharacteristic("0032", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.
BLEFloatCharacteristic angVelocityZCharacteristic("0033", BLERead); // 32-bit unsigned in Pascals, 1 decimal place.


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

  

  environmentalSensingService.addCharacteristic(accXCharacteristic);
  environmentalSensingService.addCharacteristic(accYCharacteristic);
  environmentalSensingService.addCharacteristic(accZCharacteristic);
  environmentalSensingService.addCharacteristic(angVelocityXCharacteristic);
  environmentalSensingService.addCharacteristic(angVelocityYCharacteristic);
  environmentalSensingService.addCharacteristic(angVelocityZCharacteristic);

  
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
      
      // Update Bluetooth characteristics with new values.
    
      accXCharacteristic.writeValue((float) (accX)); // Shift for two decimal places.
      accYCharacteristic.writeValue((float) (accY)); // Shift for two decimal places.
      accZCharacteristic.writeValue((float) (accZ)); // Shift for two decimal places.
      angVelocityXCharacteristic.writeValue((float) (angVelX)); // Shift for two decimal places.
      angVelocityYCharacteristic.writeValue((float) (angVelY)); // Shift for two decimal places.
      angVelocityZCharacteristic.writeValue((float) (angVelZ)); // Shift for two decimal places.

      // Delay between updates. (Don't make too long of connections start to timeout.)
      delay(100);
    }

    // Turn off LED when connection is dropped. 
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
}
