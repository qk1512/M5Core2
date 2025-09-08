#include "PM.h"

PMt PM;
SemaphoreHandle_t PMSemaphore;

static const struct
{
    uint16_t regAddr;
    uint8_t regCount;
} modbusRequest[] =
    {
        {0x0004, 1},
        {0x0009, 1}};

static uint8_t requestIndex = 0;

bool PMisConnected(ModbusRTUClientClass &modbus)
{
    return false;
}

void PMPrintData(void)
{
    if (PM.data_updated == false)
        return;
    printf("2.5: %.2f \n", PM.pm2_value);
    printf("10: %.2f \n", PM.pm10_value);
}

void SMReadData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(PMSemaphore, pdMS_TO_TICKS(100)))
    {
        if (modbus.requestFrom(PM_ADDRESS_ID, HOLDING_REGISTERS, modbusRequest[requestIndex].regAddr, modbusRequest[requestIndex].regCount))
        {
            switch (requestIndex)
            {
            case 0:
                if (modbus.available())
                {
                    //PM.data_updated = false;
                    PM.pm2_value = modbus.read();
                }
                break;
            case 1:
                if (modbus.available())
                {
                    PM.pm10_value = modbus.read();
                }
                break;
            }
        }
        requestIndex = (requestIndex + 1) % (sizeof(modbusRequest) / sizeof(modbusRequest[0]));
        xSemaphoreGive(PMSemaphore);
    }
}
