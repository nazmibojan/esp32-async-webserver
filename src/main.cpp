#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define EEPROM_SIZE 128

const char* ssid     = "i-CANE-0000";
const char* password = "123456789";
const size_t capacity = JSON_OBJECT_SIZE(3);
char readData[32], receivedData[32];

DynamicJsonDocument doc(capacity);
AsyncWebServer server(80);

void eepromReadAll();
void eepromRead(int address, char * data);
void eepromWrite(int address, char * data);


void setup() {
  Serial.begin(9600);

  // Set ESP32 as SoftAP device
  WiFi.softAP(ssid, NULL);
  delay(5000);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("i-CANE IP Address: ");
  Serial.println(ip);

  // Get existing data
  server.on("/getAll", HTTP_GET, [](AsyncWebServerRequest *request){
    String JsonString;
    doc["ssid"] = "bukalapak";
    doc["password"] = "tokopedia";
    doc["operator"] = "telkomsel";
    serializeJson(doc, JsonString);

    request->send(200, "application/json", JsonString);
  });

  // Post data to ESP32 in JSON format
  server.on("/updateData", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    for (size_t i = 0; i < len; i++) {
      Serial.write(data[i]);
    }

    Serial.println();

    request->send(200, "application/json", String("{\"result\":\"success\"}")); 
  });

  server.begin();
}

void loop() {

}


void eepromReadAll() {

}

void eepromRead(int address, char * data) {
  Serial.println("[EEPROM] Read data from EEPROM");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    data[i] = EEPROM.read(address + i);
  }
}

void eepromWrite(int address, char * data) {
  Serial.println("[EEPROM] Write data to EEPROM");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.commit();
}
