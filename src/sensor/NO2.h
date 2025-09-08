#ifndef NO2_H
#define NO2_H

// #include "./global.h"
#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define NO2_ADDRESS_ID 0x23
#define NO2_FUNCTION_CODE 0x03
#define NO2_ADDRESS_CHECK 0x0100
#define NO2_ADDRESS_DATA 0x0006

struct NO2t
{
    float no2_value = 0.0;

    bool data_updated = false;
    bool valid = false;
};

extern NO2t NO2;
extern SemaphoreHandle_t NO2Semaphore;

bool NO2isConnected();
void NO2ReadData(ModbusRTUClientClass &modbus);
void NO2PrintData(void);

#endif