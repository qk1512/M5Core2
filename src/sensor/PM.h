#ifndef PM_H
#define PM_H

// #include "./global.h"
#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define PM_ADDRESS_ID 0x24
#define PM_FUNCTION_CODE 0x03
#define PM_ADDRESS_CHECK 0x0100
#define PM_ADDRESS_DATA_2 0x0004
#define PM_ADDRESS_DATA_10 0x0009

struct PMt
{
    float pm2_value = 0.0;
    float pm10_value = 0.0;

    bool data_updated = false;
    bool valid = false;
};


extern PMt PM;
extern SemaphoreHandle_t PMSemaphore;

bool PMisConnected();
void PMReadData(ModbusRTUClientClass &modbus);
void PMPrintData(void);

#endif