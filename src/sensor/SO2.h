#ifndef SO2_H
#define SO2_H

// #include "./global.h"
#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define SO2_ADDRESS_ID 0x21
#define SO2_FUNCTION_CODE 0x03
#define SO2_ADDRESS_CHECK 0x0100
#define SO2_ADDRESS_DATA 0x0006

struct SO2t
{
    float so2_value = 0.0;

    bool data_updated = false;
    bool valid = false;
};

extern SO2t SO2;
extern SemaphoreHandle_t SO2Semaphore;

bool SO2isConnected();
void SO2ReadData(ModbusRTUClientClass &modbus);
void SO2PrintData(void);

#endif