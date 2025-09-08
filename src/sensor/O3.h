#ifndef O3_H
#define O3_H

// #include "./global.h"
#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define O3_ADDRESS_ID 0x23
#define O3_FUNCTION_CODE 0x03
#define O3_ADDRESS_CHECK 0x0100
#define O3_ADDRESS_DATA 0x0006

struct O3t
{
    float o3_value = 0.0;

    bool data_updated = false;
    bool valid = false;
};

extern O3t O3;
extern SemaphoreHandle_t O3Semaphore;

bool O3isConnected();
void O3ReadData(ModbusRTUClientClass &modbus);
void O3PrintData(void);

#endif