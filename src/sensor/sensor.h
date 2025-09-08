#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include "global.h"

#define SO2_ADDRESS_ID 0x20
#define O3_ADDRESS_ID 0x21
#define NO2_ADDRESS_ID 0x22
#define CO_ADDRESS_ID 0x23
#define PM_ADDRESS_ID 0x24

struct SensorDatat {
    float so2 = 0.0;
    float o3 = 0.0;
    float no2 = 0.0;
    float co = 0.0;
    float pm25 = 0.0;
    float pm10 = 0.0;
    bool data_updated = false;
    bool valid = false;
};

extern SensorDatat SensorData;
extern SemaphoreHandle_t SensorSemaphore;

bool sensorsAreConnected();
void readSensorData(ModbusRTUClientClass &modbus);
void printSensorData(void);


#endif