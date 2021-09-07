
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#define NONE 0
#define MS 1
#define HS 2
#define TF 3

float angVelX,angVelY,angVelZ;

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
int index_avg = 0; // index in avg_array
int avg_array[16]; //last 16 avg ang vel, we send it in bluetooth
int last_5_array[5];// 0 is current, 1 is prev, 2 is 2prev
int last_times[5];
unsigned long currentMillis;
unsigned long previousMillis = 0;

typedef struct __attribute__( ( packed ) )
{
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
} m5600_data_t;

typedef union
{
  m5600_data_t values;
  uint8_t bytes[ 64 ];
} m5600_data_ut;

m5600_data_ut m5600Data;

// Declare Bluetooth service name, and characteristics. All are standard GATT services.
BLEService gaitDetectionService("181A");

BLECharacteristic allCharacteristic( "0011", BLERead | BLENotify, sizeof m5600Data.bytes ); // on the peripheral side

// Error pulse of death. Loops forever. Only used if there are problems.
void error_pulse() {
  while (1) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(167);
      digitalWrite(LED_BUILTIN, LOW);
      delay(167);
    }
    delay(1001);
  }
}

void setup() {
  // Illuminate LED for 5sec to indicate startup.
  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(1000);

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
  BLE.setAdvertisedService(gaitDetectionService);

  gaitDetectionService.addCharacteristic(allCharacteristic);

  // Make the service available.
  BLE.addService(gaitDetectionService);
  BLE.setConnectable(true);
  Serial.println("Advertising services.");
  BLE.advertise();

  // Turn off LED to indicate startup is complete.
  //digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
      currentMillis = millis();
      if(currentMillis - previousMillis > 1000){
          digitalWrite(LED_BUILTIN, LOW);
        }
  // Wait for a connection from a central.
  //BLEDevice central = BLE.central();

  // When a connection is made, activate LED and write address to serial for debug.
  //if (central) {
   // Serial.print("Incoming connection from: ");
    //Serial.println(central.address());

    //while (central.connected()) {
      // Get readings from sensors and update the charcteristic values.


  //--------------------------------ALGORYTHEM---------------------------------------
        if(cou==4){
          avg_array[index_avg] = sum_avg_vel/4.0;
      
          last_5_array[4]=last_5_array[3];
          last_5_array[3]=last_5_array[2];
          last_5_array[2]=last_5_array[1];
          last_5_array[1]=last_5_array[0];
          last_5_array[0] = sum_avg_vel/4.0;

          last_times[4]=last_times[3];
          last_times[3]=last_times[2];
          last_times[2]=last_times[1];
          last_times[1]=last_times[0];
          last_times[0] = sum_time/4.0;
          
          sum_avg_vel = 0,sum_time=0,cou = 0;
          min_samples++;
          // the algorithm
          if(min_samples >= 5){//array is full
            if((last_times[2] - restart_time > RESTART_THRESH) and step_state == 3){
              restart_time = last_times[2];
              prev_MS = INITIAL_AMPLITUDE;
              prev_HS = INITIAL_AMPLITUDE;
              TFtoMS = INITIAL_TIME_BETWEEN_STATES;
              MStoHS = INITIAL_TIME_BETWEEN_STATES;
              HStoTF = INITIAL_TIME_BETWEEN_STATES;
            }

            if((last_times[2] - prev_step_time > step_thresh)  or ((last_5_array[2] > STATE_SWITCH_THRESH*prev_MS)
            and step_state == 2)){
              step_state = 3;
            }
//---------------------------------MS-----------------------------------------
            if((last_5_array[2] > last_5_array[4]) and (last_5_array[2] > last_5_array[3])// MS condition
            and (last_5_array[2] > last_5_array[1]) and (last_5_array[2] > last_5_array[0])
            and (last_5_array[2] > VALUE_FLEX * prev_MS) and (last_times[2] - prev_state_time > TIME_FLEX*TFtoMS)){
              step_state = 1;
              prev_MS = last_5_array[2];
              prev_step_time = last_times[2];
              if (TFtoMS * (1- VALID_STEP_RATIO) < (last_times[2] - prev_state_time) 
              and (last_times[2] - prev_state_time < TFtoMS * (1 + VALID_STEP_RATIO))){
                 TFtoMS = ((1 - AVG_CHANGE_RATE) * TFtoMS + AVG_CHANGE_RATE * (last_times[2] - prev_state_time));
              }
              prev_state_time = last_times[2];
              //digitalWrite(LED_BUILTIN, HIGH);
              previousMillis = currentMillis;
            }
//---------------------------------HS-----------------------------------------------
            else if((last_5_array[3] > last_5_array[2]) and (last_5_array[1] > last_5_array[2])
            and step_state == 1 and ((-1*CORRELATION_RATIO) * prev_MS < last_5_array[2]) and (last_5_array[2] < 0)
            and (last_times[2] - prev_state_time >= TIME_FLEX*MStoHS)){
              digitalWrite(LED_BUILTIN, HIGH);
              step_state = 2;
              prev_HS = last_5_array[2];
              if (MStoHS * (1 - VALID_STEP_RATIO) < (last_times[2] - prev_state_time) and
              (last_times[2] - prev_state_time < MStoHS * (1 + VALID_STEP_RATIO))){
                 MStoHS = ((1 - AVG_CHANGE_RATE) * MStoHS + AVG_CHANGE_RATE * (last_times[2] - prev_state_time));
              }
              prev_state_time = last_times[2];
              previousMillis = currentMillis;
            }

//---------------------------------TF-----------------------------------------------
            else if((last_5_array[4] > last_5_array[2]) and (last_5_array[3] > last_5_array[2])
            and (last_5_array[1] > last_5_array[2]) and (last_5_array[0] > last_5_array[2])
            and step_state == 2 and (last_5_array[2] < VALUE_FLEX * prev_HS)
            and (last_times[2] - prev_state_time >= TIME_FLEX*HStoTF)){
              step_state = 3;
              if (HStoTF * (1 - VALID_STEP_RATIO) < (last_times[2] - prev_state_time) and
              (last_times[2] - prev_state_time < HStoTF * (1 + VALID_STEP_RATIO))){
                 HStoTF = ((1 - AVG_CHANGE_RATE) * HStoTF + AVG_CHANGE_RATE * (last_times[2] - prev_state_time));
              }
              prev_state_time = last_times[2];
              digitalWrite(LED_BUILTIN, LOW);
           }
//--------------------------------------------------------------------------------------------------
         }
      }
        
        if(index_avg == 16){///send data , bluetooth
          m5600Data.values.angVelY1 = avg_array[0];
          m5600Data.values.angVelY2 = avg_array[1];
          m5600Data.values.angVelY3 = avg_array[2];
          m5600Data.values.angVelY4 = avg_array[3];
          m5600Data.values.angVelY5 = avg_array[4];
          m5600Data.values.angVelY6 = avg_array[5];
          m5600Data.values.angVelY7 = avg_array[6];
          m5600Data.values.angVelY8 = avg_array[7];
          m5600Data.values.angVelY9 = avg_array[8];
          m5600Data.values.angVelY10 = avg_array[9];
          m5600Data.values.angVelY11 = avg_array[10];
          m5600Data.values.angVelY12 = avg_array[11];
          m5600Data.values.angVelY13 = avg_array[12];
          m5600Data.values.angVelY14 = avg_array[13];
          m5600Data.values.angVelY15 = avg_array[14];
          m5600Data.values.angVelY16 = avg_array[15];
          //allCharacteristic.writeValue( m5600Data.bytes, 64 );
          index_avg = 0;
        }
        if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(angVelX, angVelY, angVelZ); 
        }
        sum_avg_vel = sum_avg_vel + angVelY;
        sum_time = sum_time + currentMillis;
        index_avg++,cou++;
        delay(10);
    }
    //Serial.println("Connection terminated.");
  //}
//}

  
