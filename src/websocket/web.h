#ifndef WS_H
#define WS_H

#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <global.h>


void notifyClients(float so2, float o3, float no2, float co, float pm2, float pm10);
void setupWebServer();
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                AwsEventType type, void *arg, uint8_t *data, size_t len);
void loadOffset(float &SO2_Offset, float &O3_Offset, float &NO2_Offset, float &CO_Offset, float &PM2_Offset, float &PM10_Offset);

#endif