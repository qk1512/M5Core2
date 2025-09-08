#include "CO.h"

COt CO;
SemaphoreHandle_t COSemaphore;

bool COisConnected(ModbusRTUClientClass &modbus)
{
    return false;
}

void COPrintData(void)
{
    if (CO.data_updated == false)
        return;
    printf("CO: %.2f \n", CO.co_value);
}

void COReadData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(COSemaphore, pdMS_TO_TICKS(100)))
    {
        if (modbus.requestFrom(CO_ADDRESS_ID, HOLDING_REGISTERS, CO_ADDRESS_DATA, 1))
        {
            uint16_t raw = modbus.read();
            CO.co_value = raw / 10.0;
            CO.valid = true;
        }
        else
        {
            Serial.print("CO Read Error: ");
            Serial.println(modbus.lastError());
        }
        xSemaphoreGive(COSemaphore);
    }
}
