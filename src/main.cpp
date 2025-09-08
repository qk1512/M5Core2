//#include <M5Unified.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Preferences.h"
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>


#include <./websocket/web.h>
#include <./display/display.h>
#include <./sensor/CO.h>
#include <./sensor/NO2.h>
#include <./sensor/O3.h>
#include <./sensor/SO2.h>
#include <./sensor/PM.h>

const char *ssid = "QUANG KHANH_2.4G";
const char *password = "15122002";

RS485Class RS485(Serial2, 32, 33, -1, -1);


/* void setupWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
} */

void setupWiFi()
{
  const char *ssid = "M5Core2-AP";   // AP name (your choice)
  const char *password = ""; // Password (min 8 chars)

  WiFi.mode(WIFI_AP);          // Set mode to Access Point
  WiFi.softAP(ssid, password); // Start AP

  IPAddress IP = WiFi.softAPIP(); // Get AP IP
  Serial.print("Access Point started! IP: ");
  Serial.println(IP); // Usually 192.168.4.1
}

void setup()
{
  M5.begin();
  Serial.begin(115200);
  //Wire.begin(SDA_PIN, SCL_PIN);

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Starting WebSocket...");

  SPIFFS.begin(true);
  setupWiFi();

  loadOffset(SO2_Offset, O3_Offset, NO2_Offset, CO_Offset, PM2_Offset, PM10_Offset);
  setupWebServer();
  drawSensorBoxes();

}

void loop()
{
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 2000)
  {
    lastSend = millis();

    float so2 = random(0, 20);
    updateBoxValue(0, so2);  // SO2
    float o3 = random(0, 20);
    updateBoxValue(1, o3);   // O3
    float no2 = random(0, 20);
    updateBoxValue(2, no2);  // NO2
    float co = random(0, 20);  
    updateBoxValue(3, co);   // CO
    float pm25 = random(0, 20);
    updateBoxValue(4, pm25); // PM2.5
    float pm10 = random(0, 20);
    updateBoxValue(5, pm10); // PM1

    //updateLCD(SO2_Offset, O3_Offset, NO2_Offset, CO_Offset, PM2_Offset, PM10_Offset, so2, o3, no2, co, pm2, pm10);
    notifyClients(so2, o3, no2, co, pm25, pm10);
  }
}