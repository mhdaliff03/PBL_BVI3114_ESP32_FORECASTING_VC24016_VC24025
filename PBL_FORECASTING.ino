#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "qwerty";
const char* password = "22345678";

// Google Apps Script Web App URL
const char* scriptURL = "https://https://script.google.com/u/0/home/projects/1zNL_y8DMrz9M28Db5T9Cmjev0DnGLKJcyDnNuS0YzZdfVp-3ggleh_VZ/edit";

// DHT sensor configuration
#define DHTPIN 4       // GPIO pin connected to DHT data pin
#define DHTTYPE DHT11   // Change to DHT22 if using AM2302/DHT22
DHT dht(DHTPIN, DHTTYPE);

// Consistent sampling interval (5000 ms = 5 seconds for testing; change to 60000 or 300000 for deployment)
const unsigned long sendInterval = 5000;
unsigned long previousMillis = 0;

// Validate sensor readings to prevent corrupted data from affecting forecasting
bool validateSensorReading(float temp, float hum) {
  if (isnan(temp) || isnan(hum)) {
    Serial.println("[Validation Error] Sensor returned NaN.");
    return false;
  }

  // Reasonable operating range for DHT11/DHT22
  if (temp < -40.0 || temp > 80.0) {
    Serial.println("[Validation Error] Temperature out of valid range (-40 to 80 C).");
    return false;
  }

  if (hum < 0.0 || hum > 100.0) {
    Serial.println("[Validation Error] Humidity out of valid range (0 to 100%).");
    return false;
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 IoT Sensor Data Logger & Forecasting Node ===");

  // Initialize DHT sensor
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();

  // Non-blocking periodic sampling
  if (currentMillis - previousMillis >= sendInterval) {
    previousMillis = currentMillis;

    // Read temperature and humidity
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Validate before logging and sending
    if (!validateSensorReading(temperature, humidity)) {
      Serial.println("Skipping transmission due to invalid sensor reading.");
      return;
    }

    Serial.print("Valid Reading -> Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send validated data to Google Sheets
    sendDataToGoogleSheets(temperature, humidity);
  }
}

void sendDataToGoogleSheets(float temperature, float humidity) {
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Attempting to reconnect...");
    WiFi.reconnect();
    return;
  }

  HTTPClient http;

  http.setTimeout(15000); // Tunggu hingga 15 saat
  http.begin(scriptURL);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // Paksa ikut redirect Google Apps Script
  http.addHeader("Content-Type", "application/json");

  // Build JSON payload
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  String jsonString;
  serializeJson(doc, jsonString);

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response Code: " + String(httpResponseCode));
    Serial.println("Server Response: " + response);
  } else {
    Serial.print("Error sending POST request: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}