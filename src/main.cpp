#include <M5Core2.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT20.h>
#include <ArduinoJson.h>

const char *ssid = "RD-SEAI_2.4G";
const char *password = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DHT20 dht20;

#define SDA_PIN 32
#define SCL_PIN 33

void notifyClients(float temp, float hum)
{
  StaticJsonDocument<128> doc;
  doc["temperature"] = temp;
  doc["humidity"] = hum;

  String jsonStr;
  serializeJson(doc, jsonStr);
  ws.textAll(jsonStr);
}

float tempOffset = 0.0;
float humOffset = 0.0;

void updateLCD(float tempOffset, float humOffset)
{
  M5.Lcd.fillRect(0, 26, 240, 96, BLACK); // clear old values
  M5.Lcd.setCursor(0, 30);
  M5.Lcd.printf("Temp cal : (%.2f)\n", tempOffset);
  M5.Lcd.printf("Hum  cal : (%.2f)\n", humOffset);
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
        tempOffset = doc["tempOffset"] | 0.0;
        humOffset = doc["humOffset"] | 0.0;
        Serial.printf("Calibration updated: tempOffset=%.2f, humOffset=%.2f\n", tempOffset, humOffset);
        //updateLCD(tempOffset, humOffset);
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
  M5.begin(true, false,true,true,kMBusModeOutput,false);
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);


  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Starting WebSocket...");

  SPIFFS.begin(true);
  setupWiFi();

  //dht20.begin(Wire); // Default SDA/SCL

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

    updateLCD(tempOffset, humOffset);
    notifyClients(temp, hum);
  }
}
