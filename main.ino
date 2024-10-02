#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "index.h"   // Our HTML webpage contents with JavaScript
#include "DHTesp.h"  // DHT11 Library for ESP

#define LED 2      // Onboard LED
#define DHTpin 14  // D5 of NodeMCU is GPIO14
#define BMP_SCK (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS (10)

DHTesp dht;
Adafruit_BMP280 bmp;

// SSID and Password of your WiFi router
const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);  // Server on port 80

String jsonData;

void handleRoot() {
  String s = MAIN_page;              // Read HTML contents
  server.send(200, "text/html", s);  // Send web page
}

void handleUpdateData() {
  if (server.method() == HTTP_POST) {
    String jsonString = server.arg("plain");
    jsonData = jsonString;
    Serial.println(jsonData);
    server.send(200, "application/json", "{\"status\":\"Data updated successfully\"}");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleGetData() {
  if (jsonData.length() > 0) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, jsonData);
    const int noRainReading = 90;
    const int maxRainReading = 1023;
    const float maxRainfallMM = 50.0;

    int rain_raw = analogRead(A0);
    int adjustedValue = rain_raw - noRainReading;
    if (adjustedValue < 0) {
      adjustedValue = 0;
    }
    // Convert the adjusted reading to millimeters of rainfall
    float rain = (adjustedValue / float(maxRainReading - noRainReading)) * maxRainfallMM;

    float humidity = dht.getHumidity()/100;
    float temperature = dht.getTemperature();
    float pressure = bmp.readPressure();
    float altitude = bmp.readAltitude(1013.25);

    Serial.print("H:");
    Serial.println(humidity);
    Serial.print("T:");
    Serial.println(temperature);  //dht.toFahrenheit(temperature));
    Serial.print("R:");
    Serial.println(rain);
    Serial.print("P:");
    Serial.println(pressure);

    doc["currentRain"] = rain;
    doc["currentHumidity"] = humidity;
    doc["currentTemperature"] = temperature;
    doc["currentPressure"] = pressure;

    roundJsonValues(doc);

    String data;
    serializeJson(doc, data);
    Serial.println(data);
    server.send(200, "application/json", data);
  } else {
    server.send(200, "application/json", "{\"status\":\"No data available\"}");
  }
}

void roundJsonValues(JsonVariant variant) {
  if (variant.is<JsonObject>()) {
    for (JsonPair keyValue : variant.as<JsonObject>()) {
      roundJsonValues(keyValue.value());
    }
  } else if (variant.is<JsonArray>()) {
    for (JsonVariant v : variant.as<JsonArray>()) {
      roundJsonValues(v);
    }
  } else if (variant.is<float>()) {
    float value = variant.as<float>();
    variant.set(round(value * 100.0) / 100.0);
  }
}

void setup() {
  Serial.begin(115200);
  delay(10000);
  dht.setup(DHTpin, DHTesp::DHT11);
  pinMode(LED, OUTPUT);

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1) delay(10);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/updateData", handleUpdateData);
  server.on("/getData", handleGetData);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
