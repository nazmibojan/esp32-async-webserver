#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define EEPROM_SIZE 128
#define SSID_ADDRESS 0
#define PASS_ADDRESS 32
#define OPTCELL_ADDRESS 64

const char *ap_ssid = "i-CANE-0000";
// const char *ap_pass = "123456789";
const char *ssid;
const char *password;
const char *optCell;
const size_t capacity = JSON_OBJECT_SIZE(3) + 60;
char readData[32], receivedData[32];

DynamicJsonDocument doc(capacity);
AsyncWebServer server(80);

void eepromReadAll();
void eepromRead(int address, char *data);
void eepromWrite(int address, char *data);

void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);

  // Set ESP32 as SoftAP device
  WiFi.softAP(ap_ssid, NULL);
  delay(5000);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("i-CANE IP Address: ");
  Serial.println(ip);

  // Get existing data
  server.on("/getAll", HTTP_GET, [](AsyncWebServerRequest *request) {
    String JsonString;

    eepromRead(SSID_ADDRESS, (char *)ssid);
    eepromRead(PASS_ADDRESS, (char *)password);
    eepromRead(OPTCELL_ADDRESS, (char *)optCell);

    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["operator"] = optCell;
    serializeJson(doc, JsonString);

    request->send(200, "application/json", JsonString);
  });

  // Post data to ESP32 in JSON format
  server.on("/updateData", HTTP_POST, [](AsyncWebServerRequest *request) {},
            NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
               size_t index, size_t total) {

              const size_t capacity = JSON_OBJECT_SIZE(3) + 60;
              DynamicJsonDocument doc(capacity);

              // Print JSON Data
              for (size_t i = 0; i < len; i++) {
                Serial.write(data[i]);
              }
              Serial.println();

              deserializeJson(doc, data);
              ssid = doc["ssid"];
              password = doc["password"];
              optCell = doc["operator"];

              // Write data to EEPROM
              eepromWrite(SSID_ADDRESS, (char *)ssid);
              eepromWrite(PASS_ADDRESS, (char *)password);
              eepromWrite(OPTCELL_ADDRESS, (char *)optCell);

              request->send(200, "application/json",
                            String("{\"result\":\"success\"}"));
            });

  server.begin();
}

void loop() {}

void eepromReadAll() {}

void eepromRead(int address, char *data) {
  Serial.println("[EEPROM] Read data from EEPROM");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    data[i] = EEPROM.read(address + i);
  }
}

void eepromWrite(int address, char *data) {
  Serial.println("[EEPROM] Write data to EEPROM");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.commit();
}
