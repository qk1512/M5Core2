//#include <M5Unified.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Preferences.h"
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#include <global.h>
#include <./websocket/webserver.h>
#include <./display/display.h>
#include <./sensor/sensor.h>

const char *ssid = "RD-SEAI_2.4G";
const char *password = "";

RS485Class RS485(Serial2, 32, 33, -1, -1);

TaskHandle_t readTaskHandle, displayTaskHandle;

void readSensorTask(void *parameter)
{
  for (;;)
  {
    readSensorData(ModbusRTUClient);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void updateDisplay()
{
  for (int i = 0; i < 6; i++)
  {
    switch (i)
    {
    case 0:
      updateBoxValue(i,"SO2", SensorData.so2, SO2_Offset, SO2_rate);
      break;
    case 1:
      updateBoxValue(i,"O3",SensorData.o3, O3_Offset, O3_rate);
      break;
    case 2:
      updateBoxValue(i,"NO2", SensorData.no2, NO2_Offset, NO2_rate);
      break;
    case 3:
      updateBoxValue(i,"CO" ,SensorData.co, CO_Offset, CO_rate);
      break;
    case 4:
      updateBoxValue(i, "PM2.5" ,SensorData.pm25, PM2_Offset, PM2_rate);
      break;
    case 5:
      updateBoxValue(i, "PM10",SensorData.pm10, PM10_Offset, PM10_rate);
      break;
    }
  }
}

void calibrateSensor()
{
  SensorData.so2 = SensorData.so2 * SO2_rate;
  SensorData.o3 = SensorData.o3 * O3_rate;
  SensorData.no2 = SensorData.no2 * NO2_rate;
  SensorData.co = SensorData.co * CO_rate;
  SensorData.pm25 = SensorData.pm25 * PM2_rate;
  SensorData.pm10 = SensorData.pm10 * PM10_rate;
}

void displayTask(void *parameter)
{
  for (;;)
  {
    if (SensorData.data_updated)
    {
      calibrateSensor();
      
      //printSensorData();
      updateDisplay();
      notifyClients(SensorData.so2, SensorData.o3, SensorData.no2, SensorData.co, SensorData.pm25, SensorData.pm10);
      SensorData.data_updated = false;
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

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
}
 */
void setupWiFi()
{
  const char *ssid = "M5Core2";   // AP name (your choice)
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

  ModbusRTUClient.begin(9600);

  ModbusRTUClient.setTimeout(2000);
  vTaskDelay(pdMS_TO_TICKS(1000));

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  SPIFFS.begin(true);
  setupWiFi();

  //initDefaultOffsets();
  loadOffset(SO2_Offset, O3_Offset, NO2_Offset, CO_Offset, PM2_Offset, PM10_Offset);
  loadCalibrationRates();

  setupWebServer();

  SensorSemaphore = xSemaphoreCreateMutex();
  
  xTaskCreatePinnedToCore(readSensorTask,"ReadSensorTask",4096 ,NULL,1,&readTaskHandle,0);
  xTaskCreatePinnedToCore(displayTask,"DisplayTask",4096 ,NULL,1,&displayTaskHandle,1);
  
}

void loop()
{
  
}