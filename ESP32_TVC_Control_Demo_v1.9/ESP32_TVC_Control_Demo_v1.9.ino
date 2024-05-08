#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <WebServer.h>
#include "web_server.h"
#include "web_page.h"
#include <Tic.h>

MPU6050 mpu;
TicI2C tic1(14); // Define Tic1 object for I2C communication -- X axis TIC249
//TicI2C tic2(15); // Define Tic1 object for I2C communication -- Y axis TIC249

const char* existingSSID = ""; // SSID of your existing network
const char* existingPassword = ""; // Password of your existing network

const char* apSSID = "ESP32AP"; // SSID for the ESP32 Access Point
const char* apPassword = "password"; // Password for the ESP32 Access Point

int connectToExistingNetwork = 0; // Set to 1 to connect to an existing network, 0 to set up an access point

int led = 2;
bool ledState = LOW;

const int xAxisPin = 35;
const int yAxisPin = 34;

const int MaxADcel = 75000;
const int MaxSpeed = 7500000;
const int MAX_POSITION = 1000;  // Define the maximum allowed position
const int MIN_POSITION = -1000; // Define the minimum allowed position


const float alpha = 0.1; // MPU6050 Smoothing factor (adjust as needed)
const int numReadings = 3;
int xReadings[numReadings];
int yReadings[numReadings];
int xIndex = 0;
int yIndex = 0;
int xTotal = 0;
int yTotal = 0;
float esp32_xValue = 0.0; // Changed to float
float esp32_yValue = 0.0; // Changed to float

int middleX = 0;    // range is -1000 to + 1000
int middleY = 0;    // range is -1000 to + 1000

unsigned long previousMillis0 = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis4 = 0;
const long interval = 10;

bool resetThumbstick = false;
bool commandControl = false;

WebServer server(80);

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupWebServer();
  
  Wire.begin(); // Initialize I2C bus
  mpu.initialize(); // Initialize MPU6050
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250); // Set gyroscope full scale range
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // Set accelerometer full scale range
  
  // Give the Tic some time to start up.
  delay(50);
  tic1.haltAndSetPosition(0);
  tic1.exitSafeStart();
  // tic2.haltAndSetPosition(0);
  // tic2.exitSafeStart();

  // tic1 dynamic limints
  tic1.setMaxAccel(MaxADcel); // Set the maximum acceleration to 750 steps per second squared
  tic1.setMaxDecel(MaxADcel); // Set the maximum deceleration to 750 steps per second squared
  tic1.setMaxSpeed(MaxSpeed); // Set the maximum speed to 750 steps per second

  // // tic2 dynamic limints
  // tic2.setMaxAccel(MaxADcel); // Set the maximum acceleration to 750 steps per second squared
  // tic2.setMaxDecel(MaxADcel); // Set the maximum deceleration to 750 steps per second squared
  // tic2.setMaxSpeed(MaxSpeed); // Set the maximum speed 750 1000 steps per second
  delayWhileResettingCommandTimeout(3000);
}

void loop() {
  unsigned long currentMillis0 = millis();
  if (currentMillis0 - previousMillis0 >= (interval / 4)) {
    previousMillis0 = currentMillis0;
    server.handleClient(); // Handle client requests
  }

  // Send MPU6050 data to client
  handleMPU6050Data();
  
  handleThumbstick();
  tic1.getCurrentPosition();
  //tic1.setTargetPosition(2000);
  //waitForPosition(tic1, 2000);
  // Serial.println(tic1.getCurrentPosition());
  // // Tell the Tic to move to position -100, and delay for 3000 ms
  // // to give it time to get there.
  //tic1.setTargetPosition(-2000);
  // Serial.println(tic1.getCurrentPosition());
  // // Tell the tic1 to move to position 100, and wait until it gets
  // // there.
  // // targetPos waitPos are the same values
  // tic1.setTargetPosition(1000);
  // waitForPosition(1000);
  //Serial.println(tic1.getCurrentPosition());
  // // Tell the Tic to move to position -100, and delay for 3000 ms
  // // to give it time to get there.
  // tic1.setTargetPosition(-1000);
  //Serial.println(tic1.getCurrentPosition());          // change to tic2
  delayWhileResettingCommandTimeout(30);
}

void handleMPU6050Data() {
  unsigned long currentMillis0 = millis();
  if (currentMillis0 - previousMillis4 >= (4*interval)) {
    previousMillis4 = currentMillis0;
    // Read MPU6050 data
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);

    // Smooth accelerometer data
    static float smoothed_ax = 0, smoothed_ay = 0, smoothed_az = 0;
    smoothed_ax = alpha * ax + (1 - alpha) * smoothed_ax;
    smoothed_ay = alpha * ay + (1 - alpha) * smoothed_ay;
    smoothed_az = alpha * az + (1 - alpha) * smoothed_az;

    // Smooth gyroscope data
    static float smoothed_gx = 0, smoothed_gy = 0, smoothed_gz = 0;
    smoothed_gx = alpha * gx + (1 - alpha) * smoothed_gx;
    smoothed_gy = alpha * gy + (1 - alpha) * smoothed_gy;
    smoothed_gz = alpha * gz + (1 - alpha) * smoothed_gz;

    // Calculate pitch, roll, and yaw angles using smoothed data
    float pitch = atan2(-smoothed_ax, sqrt(smoothed_ay * smoothed_ay + smoothed_az * smoothed_az)) * 180 / M_PI;
    float roll = atan2(smoothed_ay, smoothed_az) * 180 / M_PI;
    float yaw = atan2(smoothed_az, sqrt(smoothed_ax * smoothed_ax + smoothed_ay * smoothed_ay)) * 180 / M_PI;

    // Prepare MPU6050 data string
    String mpuData = String(ax) + "," + String(ay) + "," + String(az) + "," +
                     String(gx) + "," + String(gy) + "," + String(gz) + "," +
                     String(pitch) + "," + String(roll) + "," + String(yaw);
                     
    // Send MPU6050 data to client
    server.send(200, "text/plain", mpuData);
  }
}

void setupWiFi() {
  delay(1000);
  Serial.print(" wifi type: ");
  Serial.println(connectToExistingNetwork);
  Serial.println("Configuring WiFi...");
  
  if (connectToExistingNetwork == 1) {
    Serial.println("Connecting to existing WiFi network...");
    WiFi.begin(existingSSID, existingPassword);
  } else {
    Serial.println("Configuring access point...");
    if (WiFi.softAP(apSSID, apPassword)) {
      Serial.println("Access point configured successfully");
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println("Failed to configure access point");
    }
  }

  if (connectToExistingNetwork == 1) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("\nConnected to existing network");
    Serial.println("WiFi access configured successfully");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void setupWebServer() {
  pinMode(led, OUTPUT);
  
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/thumbstick", handleThumbstick);
  server.on("/temperature", handleTemperature);
  server.on("/resetThumbstick", handleResetThumbstick);
  server.on("/mpu6050", handleMPU6050Data);
  server.on("/CommandControl", handleCommandControl); // Add this line
  server.on("/ZeroPointHome",handleZeroPointHome);
  
  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}

void handleRoot() {
  server.send(200, "text/html", HTML);
}

void handleToggle() {
  unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= interval) {
    previousMillis1 = currentMillis1;
    ledState = !ledState;
    digitalWrite(led, ledState);
    server.send(200, "text/plain", "LED state toggled");
  }
}

void handleThumbstick() {
  unsigned long currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 >= interval) {
    previousMillis2 = currentMillis2;
    int xValue = tic1.getCurrentPosition();
    if (xValue > MAX_POSITION) {
      xValue = MAX_POSITION;
    } else if (xValue < MIN_POSITION) {
      xValue = MIN_POSITION;
    }
    tic1.setTargetPosition(xValue);
    //waitForPosition(tic1, xValue);

    int yValue = tic1.getCurrentPosition();
    if (yValue > MAX_POSITION) {
      yValue = MAX_POSITION;
    } else if (yValue < MIN_POSITION) {
      yValue = MIN_POSITION;
    }
    tic1.setTargetPosition(xValue); //change to tic2
    //waitForPosition(tic1, yValue); //change to tic2

    String esp32Data = server.arg("esp32_data");

    // Check if esp32Data is not empty and contains valid data
    if (!esp32Data.isEmpty()) {
        int commaIndex = esp32Data.indexOf(',');
        if (commaIndex != -1) {
            esp32_xValue = esp32Data.substring(0, commaIndex).toFloat(); // Changed to float
            esp32_yValue = esp32Data.substring(commaIndex + 1).toFloat(); // Changed to float
        } 
    }
    if (commandControl) {
      xValue = round(77*esp32_xValue); //76.923
      yValue = round(77*esp32_yValue);
      tic1.setTargetPosition(xValue);
      Serial.println(xValue);
      //waitForPosition(tic1, xValue);
      //tic1.setTargetPosition(yValue);       // change to tic2
      //waitForPosition(tic1, yValue);        // change to tic2
    }

    if (resetThumbstick) {
      xValue = middleX;
      yValue = middleY;
      esp32_xValue = 0.0; // Changed to float
      esp32_yValue = 0.0; // Changed to float
    }

    xValue = tic1.getCurrentPosition();
    yValue = 0;//tic1.getCurrentPosition();     // change to tic2
    //xValue = xValue+10;
    //tic1.setTargetPosition(xValue);

    xTotal = xTotal - xReadings[xIndex] + xValue;
    yTotal = yTotal - yReadings[yIndex] + yValue;
    xReadings[xIndex] = xValue;
    yReadings[yIndex] = yValue;
    xIndex = (xIndex + 1) % numReadings;
    yIndex = (yIndex + 1) % numReadings;
    int averagedX = xTotal / numReadings;
    int averagedY = yTotal / numReadings;

    // Prepare thumbstick data string
    String thumbstickData = String(averagedX) + "," + String(averagedY) + "," + String(esp32_xValue) + "," + String(esp32_yValue);
    
    // Send thumbstick data to client
    server.send(200, "text/plain", thumbstickData);
  } 
}

void handleTemperature() {
  unsigned long currentMillis3 = millis();
  if (currentMillis3 - previousMillis3 >= interval) {
    previousMillis3 = currentMillis3;
    // Assuming ESP32 module has onboard temperature sensor
    float temp = temperatureRead();
    String temperatureData = String(temp) + " C";
    server.send(200, "text/plain", temperatureData);
  }
}

void handleResetThumbstick() {
  resetThumbstick = !resetThumbstick;
  server.send(200, "text/plain", "Thumbstick reset to center position");
}

void handleCommandControl() {
  commandControl = !commandControl;
  server.send(200, "text/plain", "Command control switched");
}

void handleZeroPointHome() {
  // Read current pitch and roll angles from MPU6050
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);
  
  // Calculate pitch and roll angles in degrees
  float pitch = atan2(-ay, sqrt(ax * ax + az * az)) * 180.0 / M_PI;
  float roll = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
  
  // Calculate the difference between current pitch and roll angles and 0
  float pitchDifference = 0 - pitch;
  float rollDifference = 0 - roll;
  
  // Calculate target positions for Tic stepper motor drivers based on the differences
  int targetXPosition = round(76.923 * pitchDifference); // Adjusted for rounding
  int targetYPosition = round(76.923 * rollDifference); // Adjusted for rounding

  // Lower stepper motor dynamic limtis for safety
  // tic1 dynamic limints
  tic1.setMaxAccel(100); // Set the maximum acceleration to 100 steps per second squared
  tic1.setMaxDecel(100); // Set the maximum deceleration to 100 steps per second squared
  tic1.setMaxSpeed(50); // Set the maximum speed to 50 steps per second
  // // tic2 dynamic limints
  // tic2.setMaxAccel(100); // Set the maximum acceleration to 100 steps per second squared
  // tic2.setMaxDecel(100); // Set the maximum deceleration to 100 steps per second squared
  // tic2.setMaxSpeed(50); // Set the maximum speed 50 steps per second
  
  // Set target positions for the Tic stepper motor drivers
  tic1.setTargetPosition(targetXPosition);
  // tic2.setTargetPosition(targetYPosition); // Uncomment if you have a second Tic
  
  // Wait for the motors to reach the target positions
  waitForPosition(tic1, targetXPosition);
  // waitForPosition(tic2, targetYPosition); // Uncomment if you have a second Tic
  

  // reset tic 0 positions to be 0 deg pitch and tilt.
  tic1.haltAndSetPosition(0);
  // tic2.haltAndSetPosition(0);

    // reset stepper motor dynamic limtis for testing
  // tic1 dynamic limints
  tic1.setMaxAccel(MaxADcel); // Set the maximum acceleration to 750 steps per second squared
  tic1.setMaxDecel(MaxADcel); // Set the maximum deceleration to 750 steps per second squared
  tic1.setMaxSpeed(MaxSpeed); // Set the maximum speed to 750 steps per second
  // // tic2 dynamic limints
  // tic2.setMaxAccel(MaxADcel); // Set the maximum acceleration to 750 steps per second squared
  // tic2.setMaxDecel(MaxADcel); // Set the maximum deceleration to 750 steps per second squared
  // tic2.setMaxSpeed(MaxSpeed); // Set the maximum speed 750 1000 steps per second

  // Send response to the client indicating successful home positioning
  server.send(200, "text/plain", "Tic stepper motors positioned to zero pitch and roll");
}


// Sends a "Reset command timeout" command to the Tic.  We must
// call this at least once per second, or else a command timeout
// error will happen.  The Tic's default command timeout period
// is 1000 ms, but it can be changed or disabled in the Tic
// Control Center.
void resetCommandTimeout()
{
  tic1.resetCommandTimeout();
  tic1.resetCommandTimeout();
}

// Delays for the specified number of milliseconds while
// resetting the Tic's command timeout so that its movement does
// not get interrupted by errors.
void delayWhileResettingCommandTimeout(uint32_t ms)
{
  uint32_t start = millis();
  do
  {
    resetCommandTimeout();
  } while ((uint32_t)(millis() - start) <= ms);
}

// Polls the Tic, waiting for it to reach the specified target
// position.  Note that if the Tic detects an error, the Tic will
// probably go into safe-start mode and never reach its target
// position, so this function will loop infinitely.  If that
// happens, you will need to reset your Arduino.
// Polls the specified Tic, waiting for it to reach the specified target
// position.
void waitForPosition(TicI2C &tic, int32_t targetPosition)
{
  do
  {
    resetCommandTimeout();
  } while (tic.getCurrentPosition() != targetPosition);
}

