#ifndef CO_H
#define CO_H

// #include "./global.h"
#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define CO_ADDRESS_ID 0x23
#define CO_FUNCTION_CODE 0x03
#define CO_ADDRESS_CHECK 0x0100
#define CO_ADDRESS_DATA 0x0006

struct COt
{
    float co_value = 0.0;

    bool data_updated = false;
    bool valid = false;
};

extern COt CO;
extern SemaphoreHandle_t COSemaphore;

bool COisConnected();
void COReadData(ModbusRTUClientClass &modbus);
void COPrintData(void);

#endif