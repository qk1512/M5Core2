#include "O3.h"

O3t O3;
SemaphoreHandle_t O3Semaphore;

bool O3isConnected(ModbusRTUClientClass &modbus)
{
    return false;
}

void O3PrintData(void)
{
    if (O3.data_updated == false) return;
    printf("O3: %.2f \n", O3.o3_value);
}

void O3ReadData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(O3Semaphore, pdMS_TO_TICKS(100)))
    {
        if (modbus.requestFrom(O3_ADDRESS_ID, HOLDING_REGISTERS, O3_ADDRESS_DATA, 1))
        {
            uint16_t raw = modbus.read();
            O3.o3_value = raw / 10.0;
            //O3.data_updated = true;
            O3.valid = true;
        }
        else
        {
            Serial.print("O3 Read Error: ");
            Serial.println(modbus.lastError());
            //O3.data_updated = false;
        }
        xSemaphoreGive(O3Semaphore);
    }
}
