#include "web.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Preferences prefs;

float O3_Offset = 0.0;
float CO_Offset = 0.0;
float NO2_Offset = 0.0;
float SO2_Offset = 0.0;
float PM2_Offset = 0.0;
float PM10_Offset = 0.0;

void notifyClients(float so2, float o3, float no2, float co, float pm2, float pm10)
{
    StaticJsonDocument<256> doc;
    
    doc["so2"] = so2;
    doc["co"] = co;
    doc["no2"] = no2;
    doc["o3"] = o3;
    doc["pm2"] = pm2;
    doc["pm10"] = pm10;

    String jsonStr;
    serializeJson(doc, jsonStr);
    ws.textAll(jsonStr);
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

                if (offsets.containsKey("value1"))
                {
                    SO2_Offset = offsets["value1"];
                    prefs.putFloat("SO2Offset", SO2_Offset);
                    Serial.printf("Received tempOffset: %.2f\n", SO2_Offset);
                }
                if (offsets.containsKey("value2"))
                {
                    O3_Offset = offsets["value2"];
                    prefs.putFloat("O3Offset", O3_Offset);
                    Serial.printf("Received humOffset: %.2f\n", O3_Offset);
                }
                if (offsets.containsKey("value3"))
                {
                    NO2_Offset = offsets["value3"];
                    prefs.putFloat("NO2Offset", NO2_Offset);
                    Serial.printf("Received humOffset: %.2f\n", NO2_Offset);
                }
                if (offsets.containsKey("value4"))
                {
                    CO_Offset = offsets["value4"];
                    prefs.putFloat("COOffset", CO_Offset);
                    Serial.printf("Received humOffset: %.2f\n", CO_Offset);
                }  
                if (offsets.containsKey("value5"))
                {
                    PM2_Offset = offsets["value5"];
                    prefs.putFloat("PM2Offset", PM2_Offset);
                    Serial.printf("Received humOffset: %.2f\n", PM2_Offset);
                }
                if (offsets.containsKey("value6"))
                {
                    PM10_Offset = offsets["value6"];
                    prefs.putFloat("PM10Offset", PM10_Offset);
                    Serial.printf("Received humOffset: %.2f\n", PM10_Offset);
                }
                prefs.end();
            }
        }
    }
}

void loadOffset(float &SO2_Offset, float &O3_Offset, float &NO2_Offset, float &CO_Offset, float &PM2_Offset, float &PM10_Offset)
{
    prefs.begin("calibration", true);
    SO2_Offset = prefs.getFloat("SO2Offset", 0.0);
    O3_Offset = prefs.getFloat("O3Offset", 0.0);
    NO2_Offset = prefs.getFloat("NO2Offset", 0.0);
    CO_Offset = prefs.getFloat("COOffset", 0.0);
    PM2_Offset = prefs.getFloat("PM2Offset", 0.0);
    PM10_Offset = prefs.getFloat("PM10Offset", 0.0);
    prefs.end();
}