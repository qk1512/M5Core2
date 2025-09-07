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

#define SDA_PIN 11
#define SCL_PIN 12

void notifyClients(float temp, float hum)
{
  StaticJsonDocument<128> doc;
  doc["temperature"] = temp;
  doc["humidity"] = hum;

  String jsonStr;
  serializeJson(doc, jsonStr);
  ws.textAll(jsonStr);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.println("WebSocket connected");
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.println("WebSocket disconnected");
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
  M5.begin(115200);
  Serial.begin(115200);

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Starting WebSocket...");

  //SPIFFS.begin(true);
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

    float temp = dht20.getTemperature();
    float hum = dht20.getHumidity();

    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf("Temp: %.1f C\n", temp);
    M5.Lcd.printf("Hum:  %.1f %%\n", hum);

    notifyClients(temp, hum);
  }
}
