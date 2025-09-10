#include "webserver.h"
#include "../sensor/sensor.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Preferences prefs;

float O3_Offset = -1.0;
float CO_Offset = -1.0;
float NO2_Offset = -1.0;
float SO2_Offset = -1.0;
float PM2_Offset = -1.0;
float PM10_Offset = -1.0;

float SO2_rate = 1.0;
float O3_rate = 1.0;
float NO2_rate = 1.0;
float CO_rate = 1.0;
float PM2_rate = 1.0;
float PM10_rate = 1.0;

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

/* void calculateCalibrationOffsets()
{   
    prefs.begin("rate",false);
    if(SensorData.so2 != 0 )
    {
        SO2_rate = SO2_Offset / SensorData.so2;
    }
    else if(SO2_Offset < 0)
    {
        SO2_rate = 1;
    }
    else
    {
        SO2_rate = 0.0;
    }
    prefs.putFloat("SO2Rate", SO2_rate);

    if(SensorData.o3 != 0)
    {
        O3_rate = O3_Offset / SensorData.o3;
    }
    else if (O3_Offset < 0)
    {
        O3_Offset = 1;
    }
    else
    {
        O3_rate = 0.0;
    }    
    prefs.putFloat("O3Rate", O3_rate);

    if(SensorData.no2 != 0)
    {
        NO2_rate = NO2_Offset / SensorData.no2;
    }
    else if(NO2_Offset < 0)
    {
        NO2_rate = 1.0;
    }
    else
    {
        NO2_rate = 0.0;
    }
    prefs.putFloat("NO2Rate", NO2_rate);  

    if(SensorData.co != 0)
    {
        CO_rate = CO_Offset / SensorData.co;
    }
    else if (CO_Offset < 0)
    {
        CO_rate = 1;
    }
    else
    {
        CO_rate = 0.0;
    }
    prefs.putFloat("CORate", CO_rate);

    if(SensorData.pm25 != 0)
    {
        PM2_rate = PM2_Offset / SensorData.pm25;
    }
    else if (PM2_Offset < 0)
    {
        PM2_rate = 1;
    }
    else
    {
        PM2_rate = 0.0;
    }
    prefs.putFloat("PM2Rate", PM2_rate);
    
    if(SensorData.pm10 != 0)
    {
        PM10_rate = PM10_Offset / SensorData.pm10;
    }
    else if (PM10_Offset < 0)
    {
        PM10_rate = 1;
    }
    else
    {
        PM10_rate = 0.0;
    }
    prefs.putFloat("PM10Rate", PM10_rate);

    prefs.end();


}

void printCalibrationRates()
{
    Serial.printf("SO2 Rate: %.4f \n", SO2_rate);
    Serial.printf("O3 Rate: %.4f \n", O3_rate);
    Serial.printf("NO2 Rate: %.4f \n", NO2_rate);
    Serial.printf("CO Rate: %.4f \n", CO_rate);
    Serial.printf("PM2.5 Rate: %.4f \n", PM2_rate);
    Serial.printf("PM10 Rate: %.4f \n", PM10_rate);
} */

void loadCalibrationRates()
{
    prefs.begin("rate", true);
    SO2_rate = prefs.getFloat("SO2Rate", 1.0);
    O3_rate = prefs.getFloat("O3Rate", 1.0);
    NO2_rate = prefs.getFloat("NO2Rate", 1.0);   
    CO_rate = prefs.getFloat("CORate", 1.0);
    PM2_rate = prefs.getFloat("PM2Rate", 1.0);
    PM10_rate = prefs.getFloat("PM10Rate", 1.0);
    prefs.end();
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
                prefs.begin("rate", false);
                if (offsets.containsKey("value1"))
                {
                    SO2_Offset = offsets["value1"];
                    prefs.putFloat("SO2Offset", SO2_Offset);
                    Serial.printf("Received SO2 Offset: %.2f\n", SO2_Offset);

                    if (SensorData.so2 != 0 && SO2_Offset > 0)
                    {
                        SO2_rate = SO2_Offset / SensorData.so2;
                    }
                    else if (SO2_Offset < 0)
                    {
                        SO2_rate = 1;
                    }
                    else
                    {
                        SO2_rate = 0.0;
                    }
                    prefs.putFloat("SO2Rate", SO2_rate);
                    Serial.printf("Calculated SO2 Rate: %.4f\n", SO2_rate);
                }
                if (offsets.containsKey("value2"))
                {
                    O3_Offset = offsets["value2"];
                    prefs.putFloat("O3Offset", O3_Offset);
                    Serial.printf("Received O3 Offset: %.2f\n", O3_Offset);
                    if (SensorData.o3 != 0 && O3_Offset > 0)
                    {
                        O3_rate = O3_Offset / SensorData.o3;
                    }
                    else if(O3_Offset < 0)
                    {
                        O3_rate = 1;
                    }
                    else
                    {
                        O3_rate = 0.0;
                    }
                    prefs.putFloat("O3Rate", O3_rate);
                    Serial.printf("Calculated O3 Rate: %.4f\n", O3_rate);
                }
                if (offsets.containsKey("value3"))
                {
                    NO2_Offset = offsets["value3"];
                    prefs.putFloat("NO2Offset", NO2_Offset);
                    Serial.printf("Received NO2 Offset: %.2f\n", NO2_Offset);
                    if (SensorData.no2 != 0 && NO2_Offset > 0)
                    {
                        NO2_rate = NO2_Offset / SensorData.no2;
                    }
                    else if (NO2_Offset < 0)
                    {
                        NO2_rate = 1;
                    }
                    else
                    {
                        NO2_rate = 0.0;
                    }
                    prefs.putFloat("NO2Rate", NO2_rate);
                    Serial.printf("Calculated NO2 Rate: %.4f\n", NO2_rate);
                }
                if (offsets.containsKey("value4"))
                {
                    CO_Offset = offsets["value4"];
                    prefs.putFloat("COOffset", CO_Offset);
                    Serial.printf("Received CO Offset: %.2f\n", CO_Offset);
                    if (SensorData.co != 0 && CO_Offset >0)
                    {
                        CO_rate = CO_Offset / SensorData.co;
                    }
                    else if (CO_Offset < 0)
                    {
                        CO_rate = 1;
                    }
                    else
                    {
                        CO_rate = 0.0;
                    }
                    prefs.putFloat("CORate", CO_rate);
                    Serial.printf("Calculated CO Rate: %.4f\n", CO_rate);
                }  
                if (offsets.containsKey("value5"))
                {
                    PM2_Offset = offsets["value5"];
                    prefs.putFloat("PM2Offset", PM2_Offset);
                    Serial.printf("Received PM2.5 Offset: %.2f\n", PM2_Offset);
                    if (SensorData.pm25 != 0 && PM2_Offset > 0)
                    {
                        PM2_rate = PM2_Offset / SensorData.pm25;
                    }
                    else if(PM2_Offset < 0)
                    {
                        PM2_rate = 1;
                    }
                    else
                    {
                        PM2_rate = 0.0;
                    }
                    prefs.putFloat("PM2Rate", PM2_rate);
                    Serial.printf("Calculated PM2.5 Rate: %.4f\n", PM2_rate);
                }
                if (offsets.containsKey("value6"))
                {
                    PM10_Offset = offsets["value6"];
                    prefs.putFloat("PM10Offset", PM10_Offset);
                    Serial.printf("Received PM10 Offset: %.2f\n", PM10_Offset);
                    if (SensorData.pm10 != 0 && PM10_Offset > 0)
                    {
                        PM10_rate = PM10_Offset / SensorData.pm10;
                    }
                    else if(PM10_Offset < 0)
                    {
                        PM10_rate = 1;
                    }
                    else
                    {
                        PM10_rate = 0.0;
                    }
                    prefs.putFloat("PM10Rate", PM10_rate);
                    Serial.printf("Calculated PM10 Rate: %.4f\n", PM10_rate);
                }
                prefs.end();
                //calculateCalibrationOffsets();
                //printCalibrationRates();
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

void initDefaultOffsets()
{
    prefs.begin("calibration", false);
    if (!prefs.isKey("SO2Offset"))
        prefs.putFloat("SO2Offset", 0.0);
    if (!prefs.isKey("O3Offset"))
        prefs.putFloat("O3Offset", 0.0);
    if (!prefs.isKey("NO2Offset"))
        prefs.putFloat("NO2Offset", 0.0);
    if (!prefs.isKey("COOffset"))
        prefs.putFloat("COOffset", 0.0);
    if (!prefs.isKey("PM2Offset"))
        prefs.putFloat("PM2Offset", 0.0);
    if (!prefs.isKey("PM10Offset"))
        prefs.putFloat("PM10Offset", 0.0);
    prefs.end();
}
