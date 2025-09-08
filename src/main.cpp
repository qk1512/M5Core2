#include <M5Core2.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT20.h>
#include <ArduinoJson.h>
#include "Preferences.h"

const char *ssid = "QUANG KHANH_2.4G";
const char *password = "15122002";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DHT20 dht20;

Preferences prefs;

#define SDA_PIN 32
#define SCL_PIN 33

void notifyClients(float temp, float hum, float so2, float co, float no2, float o3)
  StaticJsonDocument<128> doc;
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["so2"] = so2;
  doc["co"] = co;
  doc["no2"] = no2;  
  doc["o3"] = o3;

  String jsonStr;
  serializeJson(doc, jsonStr);
  ws.textAll(jsonStr);
}

float tempOffset = 0.0;
float humOffset = 0.0;


void updateLCD(float tempOffset, float humOffset, float temp, float hum)
{
  M5.Lcd.fillRect(0, 26, 240, 96, BLACK); // clear old values
  M5.Lcd.setCursor(0, 30);
  M5.Lcd.printf("Temp: %.2f C\n", temp);
  M5.Lcd.printf("Hum : %.2f %%\n", hum);
  M5.Lcd.printf("Temp cal : (%.2f)\n", tempOffset);
  M5.Lcd.printf("Hum  cal : (%.2f)\n", humOffset);
}

float rateTemp = 0.0;
float rateHum = 0.0;

void calibrationValue(float &rateTemp, float &rateHum)
{
  dht20.read();
  float temp = dht20.getTemperature();
  float hum = dht20.getHumidity();

  rateTemp = tempOffset / temp;
  rateHum = humOffset / hum;
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
      data[len] = 0; // Ensure null-terminated
      StaticJsonDocument<128> doc;
      DeserializationError err = deserializeJson(doc, (char *)data);
      if (!err && doc["type"] == "calibrate")
      {
        JsonObject offsets = doc["offsets"];

        prefs.begin("calibration", false);

        if(offsets.containsKey("value1"))
        {
          tempOffset = offsets["value1"];
          prefs.putFloat("tempOffset", tempOffset);
          Serial.printf("Received tempOffset: %.2f\n", tempOffset);
        }
        if(offsets.containsKey("value2"))
        {
          humOffset = offsets["value2"];
          prefs.putFloat("humOffset", humOffset);
          Serial.printf("Received humOffset: %.2f\n", humOffset);
        }
        prefs.end();
        //calibrationValue(rateTemp, rateHum);
        //Serial.printf("Calibration rate: rateTemp=%.4f, rateHum=%.4f\n", rateTemp, rateHum);
      }
    }
  }
}

void setupWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void setupWebServer()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Not found"); });

  server.begin();
}



void setup()
{
  M5.begin(true, false, true, true, kMBusModeOutput, false);
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Starting WebSocket...");

  SPIFFS.begin(true);
  setupWiFi();

  // dht20.begin(Wire); // Default SDA/SCL
  prefs.begin("calibration", true);
  tempOffset = prefs.getFloat("tempOffset", 0.0); 
  humOffset = prefs.getFloat("humOffset", 0.0);
  prefs.end();
  setupWebServer();

}

void loop()
{
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 2000)
  {
    lastSend = millis();
    dht20.read();

    float temp = dht20.getTemperature();
    float hum = dht20.getHumidity();
    float so2 = random(0, 20);
    float co = random(0, 20);  
    float no2 = random(0, 20);
    float o3 = random(0, 20);
    
    if(rateTemp != 0.0 && rateHum != 0.0)
    {
      //Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n", temp, hum);
      temp = dht20.getTemperature() * rateTemp;
      hum = dht20.getHumidity() * rateHum;
    }

    updateLCD(tempOffset, humOffset,temp,hum);
    notifyClients(temp, hum, 0.0, 0.0, 0.0, 0.0);
  }
}