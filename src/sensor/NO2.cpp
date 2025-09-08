#include "NO2.h"

NO2t NO2;
SemaphoreHandle_t NO2Semaphore;

bool NO2isConnected(ModbusRTUClientClass &modbus)
{
    return false;
}

void NO2PrintData(void)
{
    if (NO2.data_updated == false)
        return;
    printf("NO2: %.2f \n", NO2.no2_value);
}

void NO2ReadData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(NO2Semaphore, pdMS_TO_TICKS(100)))
    {
        if (modbus.requestFrom(NO2_ADDRESS_ID, HOLDING_REGISTERS, NO2_ADDRESS_DATA, 1))
        {
            uint16_t raw = modbus.read();
            NO2.no2_value = raw / 10.0;
            NO2.valid = true;
        }
        else
        {
            Serial.print("NO2 Read Error: ");
            Serial.println(modbus.lastError());
        }
        xSemaphoreGive(NO2Semaphore);
    }
}
