#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// Wi-Fi credentials
#define WIFI_SSID "iPhone od: Sead"
#define WIFI_PASSWORD "sejo1976"

// Firebase credentials
#define API_KEY "AIzaSyB5eHvhPD2izioqz1uABDReKhJ84FkgQPs"
#define DATABASE_URL "https://esp8266-firebase-demo-6e68a-default-rtdb.europe-west1.firebasedatabase.app/"

// DHT11 configuration
#define DHT_PIN D7
#define DHT_TYPE DHT11
DHT dht_sensor(DHT_PIN, DHT_TYPE);

// Soil moisture sensor configuration
#define SOIL_PIN A0

// HC-SR04 configuration
#define TRIG_PIN D5
#define ECHO_PIN D6
float tankHeight = 7.5; // Default tank height in cm

// LED Pins for progress bar
#define RED_LED1 D4
#define YELLOW_LED1 D3
#define YELLOW_LED2 D2
#define GREEN_LED1 D1

// Relay Pin for water pump
#define PUMP_RELAY_PIN D8
#define RELAY_ON LOW
#define RELAY_OFF HIGH

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Timing variables
unsigned long sendDataPrevMillis = 0;
const unsigned long DATA_SEND_INTERVAL = 15000;

// Initialization flags
bool signupOK = false;

// System control variables
bool systemEnabled = false;
struct {
  float soilMoisture = 20.0;
  float waterLevel = 20.0;
  bool automaticMode = true;  // Pump control mode
  bool pumpManualState = false;  // Manual pump control state
} thresholds;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// LED change variables
#define LED_UPDATE_INTERVAL 50
unsigned long lastLedCheck = 0;

const long gmtOffset_sec = 3600;  // GMT+1 (3600 seconds = 1 hour)
const char* ntpServer = "pool.ntp.org";

void setAllLEDs(bool state) {
  digitalWrite(RED_LED1, state);
  digitalWrite(YELLOW_LED1, state);
  digitalWrite(YELLOW_LED2, state);
  digitalWrite(GREEN_LED1, state);
}

// Function to initialize Wi-Fi
void initWiFi() {
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println("\nConnected with IP: " + WiFi.localIP().toString() + ", to: " + WIFI_SSID);

    // Initialize NTP with correct timezone
    timeClient.begin();
    timeClient.setTimeOffset(gmtOffset_sec);
}

// Function to initialize Firebase
void initFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up successful.");
    signupOK = true;
  } else {
    Serial.printf("Firebase sign-up error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// Function to measure distance with HC-SR04
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = (duration * 0.0343) / 2;  // Convert to cm
  return distance;
}

// Function to calculate water level percentage
float calculateWaterLevelPercentage(float distance) {
  if (distance > tankHeight) return 0;  // Tank is empty
  float waterHeight = tankHeight - distance;
  return constrain((waterHeight / tankHeight) * 100, 0, 100);
}

// Function to control the water pump
void controlPump(float soilMoisture, float waterLevel) {
    if (!systemEnabled) {
        digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
        return;
    }

    Serial.println("-------- Pump Control Status --------");
    Serial.printf("Mode: %s\n", thresholds.automaticMode ? "AUTO" : "MANUAL");
    Serial.printf("Soil Moisture: %.1f%% (Threshold: %.1f%%)\n", soilMoisture, thresholds.soilMoisture);
    Serial.printf("Water Level: %.1f%% (Threshold: %.1f%%)\n", waterLevel, thresholds.waterLevel);
    
    if (thresholds.automaticMode) {
        bool shouldPumpRun = soilMoisture < thresholds.soilMoisture && 
                           waterLevel > thresholds.waterLevel;
        
        digitalWrite(PUMP_RELAY_PIN, shouldPumpRun ? RELAY_ON : RELAY_OFF);
        Serial.printf("Auto Mode - Pump: %s\n", shouldPumpRun ? "ON" : "OFF");
    } else {
        digitalWrite(PUMP_RELAY_PIN, thresholds.pumpManualState ? RELAY_ON : RELAY_OFF);
        Serial.printf("Manual Mode - Pump: %s\n", thresholds.pumpManualState ? "ON" : "OFF");
    }
    Serial.println("-----------------------------------");
}

// Function to handle system control and thresholds
void handleControlUpdates() {
  if (Firebase.RTDB.getBool(&fbdo, "system/status")) {
    systemEnabled = fbdo.boolData();
    Serial.printf("System status: %s\n", systemEnabled ? "ON" : "OFF");
  }

  if (Firebase.RTDB.getFloat(&fbdo, "thresholds/soilMoisture")) {
    thresholds.soilMoisture = fbdo.floatData();
    Serial.printf("Soil moisture threshold updated: %.2f%%\n", thresholds.soilMoisture);
  }

  if (Firebase.RTDB.getFloat(&fbdo, "thresholds/waterLevel")) {
    thresholds.waterLevel = fbdo.floatData();
    Serial.printf("Water level threshold updated: %.2f%%\n", thresholds.waterLevel);
  }

  if (Firebase.RTDB.getFloat(&fbdo, "tank/height")) {
    tankHeight = fbdo.floatData();
    Serial.printf("Tank height updated: %.2f cm\n", tankHeight);
  }

  if (Firebase.RTDB.getBool(&fbdo, "pump/automaticMode")) {
        thresholds.automaticMode = fbdo.boolData();
        Serial.printf("Pump mode updated: %s\n", thresholds.automaticMode ? "AUTO" : "MANUAL");
    } else {
        Serial.printf("Failed to get pump mode: %s\n", fbdo.errorReason().c_str());
    }

    if (Firebase.RTDB.getBool(&fbdo, "pump/manualState")) {
        thresholds.pumpManualState = fbdo.boolData();
        Serial.printf("Manual pump state updated: %s\n", thresholds.pumpManualState ? "ON" : "OFF");
    } else {
        Serial.printf("Failed to get manual pump state: %s\n", fbdo.errorReason().c_str());
    }
}

// Function to send data to Firebase
void sendDataToFirebase() {
  if (!systemEnabled) {
    // Turn off all LEDs and pump when system is disabled
    setAllLEDs(LOW);
    digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
    return;
  }

  // Read DHT sensor data
  float humidity = dht_sensor.readHumidity();
  float temperature_C = dht_sensor.readTemperature();
  float temperature_F = dht_sensor.readTemperature(true);

  // Read soil moisture data
  int soilReading = analogRead(SOIL_PIN);
  float soilMoisture = 100.0 - ((soilReading / 1023.0) * 100.0);

  // Measure water level
  float distance = measureDistance();
  float waterLevelPercentage = calculateWaterLevelPercentage(distance);

  // Send current sensor data
  if (!isnan(humidity) && !isnan(temperature_C)) {
    Firebase.RTDB.setFloat(&fbdo, "sensor/humidity", humidity);
    Firebase.RTDB.setFloat(&fbdo, "sensor/temperature_C", temperature_C);
    Firebase.RTDB.setFloat(&fbdo, "sensor/temperature_F", temperature_F);
    Firebase.RTDB.setFloat(&fbdo, "sensor/soil_moisture", soilMoisture);
    Firebase.RTDB.setFloat(&fbdo, "sensor/water_level_percentage", waterLevelPercentage);
    Firebase.RTDB.setBool(&fbdo, "sensor/pump_status", digitalRead(PUMP_RELAY_PIN));
  }

  // Add timestamp and save to history
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime() - gmtOffset_sec; // Subtract the offset
    
  // Format time string
  time_t rawTime = epochTime + gmtOffset_sec;
  struct tm timeinfo;
  gmtime_r(&rawTime, &timeinfo);
    
  char formattedTime[25];
  strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
  String timestamp = String(epochTime);
  String historyPath = "history/" + timestamp;
    
  if (Firebase.RTDB.setFloat(&fbdo, historyPath + "/temperature_C", temperature_C) &&
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/humidity", humidity) &&
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/soil_moisture", soilMoisture) &&
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/water_level_percentage", waterLevelPercentage) &&
      Firebase.RTDB.setBool(&fbdo, historyPath + "/pump_status", digitalRead(PUMP_RELAY_PIN)) &&  // Add pump status to history
      Firebase.RTDB.setString(&fbdo, historyPath + "/timestamp", timestamp) &&
      Firebase.RTDB.setString(&fbdo, historyPath + "/formatted_time", formattedTime)) {
      
      Serial.println("History data saved successfully at " + String(formattedTime));
  } else {
      Serial.println("Failed to save history data: " + fbdo.errorReason());
  }

  // Control pump based on current readings
  controlPump(soilMoisture, waterLevelPercentage);

  // Check thresholds and take action
  if (soilMoisture < thresholds.soilMoisture) {
    Serial.println("Soil moisture below threshold!");
  }

  if (waterLevelPercentage < thresholds.waterLevel) {
    Serial.println("Water level below threshold!");
  }
}

void updateProgressBar() {
  if (!systemEnabled) {
    return;
  }

  // Read soil moisture data
  int soilReading = analogRead(SOIL_PIN);
  float soilMoisture = 100.0 - ((soilReading / 1023.0) * 100.0);

  // Update LED progress bar based on soil moisture
  if (soilMoisture < 10) {
    digitalWrite(RED_LED1, HIGH);
    digitalWrite(YELLOW_LED1, LOW);
    digitalWrite(YELLOW_LED2, LOW);
    digitalWrite(GREEN_LED1, LOW);
  } else if (soilMoisture >= 10 && soilMoisture < 30) {
    digitalWrite(RED_LED1, HIGH);
    digitalWrite(YELLOW_LED1, HIGH);
    digitalWrite(YELLOW_LED2, LOW);
    digitalWrite(GREEN_LED1, LOW);
  } else if (soilMoisture >= 30 && soilMoisture < 60) {
    digitalWrite(RED_LED1, HIGH);
    digitalWrite(YELLOW_LED1, HIGH);
    digitalWrite(YELLOW_LED2, HIGH);
    digitalWrite(GREEN_LED1, LOW);
  } else {
    digitalWrite(RED_LED1, HIGH);
    digitalWrite(YELLOW_LED1, HIGH);
    digitalWrite(YELLOW_LED2, HIGH);
    digitalWrite(GREEN_LED1, HIGH);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);

  // Set LED pins as outputs
  pinMode(RED_LED1, OUTPUT);
  pinMode(YELLOW_LED1, OUTPUT);
  pinMode(YELLOW_LED2, OUTPUT);
  pinMode(GREEN_LED1, OUTPUT);

  initWiFi();
  initFirebase();

  if (Firebase.ready()) {
          Firebase.RTDB.setBool(&fbdo, "pump/automaticMode", true);
          Firebase.RTDB.setBool(&fbdo, "pump/manualState", false);
          Firebase.RTDB.setBool(&fbdo, "sensor/pump_status", false);
      }

  dht_sensor.begin();
}

void loop() {
  if (Firebase.ready() && signupOK) {
    // Data updates (every 15 seconds)
    if (millis() - sendDataPrevMillis > DATA_SEND_INTERVAL) {
      sendDataPrevMillis = millis();
      handleControlUpdates();
      sendDataToFirebase();
    }

    // LED updates (50 ms)
    if (millis() - lastLedCheck > LED_UPDATE_INTERVAL) {
      lastLedCheck = millis();
      
      if (systemEnabled) {
        updateProgressBar();
      }
    }
  }
}
