#include "SO2.h"

SO2t SO2;
SemaphoreHandle_t SO2Semaphore;

bool SO2isConnected(ModbusRTUClientClass &modbus)
{
    return false;
}

void SO2PrintData(void)
{
    if (SO2.data_updated == false)
        return;
    printf("SO2: %.2f \n", SO2.so2_value);
}

void SO2ReadData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(SO2Semaphore, pdMS_TO_TICKS(100)))
    {
        if (modbus.requestFrom(SO2_ADDRESS_ID, HOLDING_REGISTERS, SO2_ADDRESS_DATA, 1))
        {
            uint16_t raw = modbus.read();
            SO2.so2_value = raw / 10.0;
            SO2.data_updated = true;
            SO2.valid = true;
        }
        else
        {
            Serial.print("SO2 Read Error: ");
            Serial.println(modbus.lastError());
            //SO2.valid = false;
            SO2.data_updated = false;
        }
        xSemaphoreGive(SO2Semaphore);
    }
}
