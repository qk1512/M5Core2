#include "sensor.h"

SensorDatat SensorData;
SemaphoreHandle_t SensorSemaphore;

static const struct
{   
    uint8_t deviceId;
    uint16_t regAddr;
    uint8_t regCount;
} modbusRequest[] =
{
    {SO2_ADDRESS_ID, 0x0006, 1},
    {O3_ADDRESS_ID, 0x0006, 1},
    {NO2_ADDRESS_ID, 0x0006, 1},
    {CO_ADDRESS_ID, 0x0006, 1},
    {PM_ADDRESS_ID, 0x0004, 1},
    {PM_ADDRESS_ID, 0x0009, 1}    
};

static uint8_t requestIndex = 0;

bool sensorsAreConnected(ModbusRTUClientClass &modbus)
{
    return false;
}

void printSensorData(void)
{
    if (SensorData.data_updated == false) return;
    printf("SO2: %.2f \n", SensorData.so2);
    printf("O3: %.2f \n", SensorData.o3);
    printf("NO2: %.2f \n", SensorData.no2);
    printf("CO: %.2f \n", SensorData.co);
    printf("PM2.5: %.2f \n", SensorData.pm25);
    printf("PM10: %.2f \n", SensorData.pm10);
}


/* void readSensorData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(SensorSemaphore, pdMS_TO_TICKS(100)))
    {
        if (modbus.requestFrom(modbusRequest[requestIndex].deviceId, HOLDING_REGISTERS, modbusRequest[requestIndex].regAddr, modbusRequest[requestIndex].regCount))
        {
            switch (requestIndex)
            {
            case 0:
                if (modbus.available())
                {   
                    //SensorData.data_updated = false;
                    SensorData.so2 = modbus.read();
                }
                break;
            case 1:
                if (modbus.available())
                {
                    SensorData.o3 = modbus.read();
                }
                break;
            case 2:
                if (modbus.available())
                {
                    SensorData.no2 = modbus.read();
                }
                break;
            case 3:
                if (modbus.available())
                {
                    SensorData.co = modbus.read();
                }
                break;
            case 4:
                if (modbus.available())
                {
                    SensorData.pm25 = modbus.read();
                }
                break;
            case 5:
                if (modbus.available())
                {
                    SensorData.pm10 = modbus.read();
                    SensorData.data_updated = true;
                }
                break;                
            }
        }
        requestIndex = (requestIndex + 1) % (sizeof(modbusRequest) / sizeof(modbusRequest[0]));
        xSemaphoreGive(SensorSemaphore);
    }
}
 */
void readSensorData(ModbusRTUClientClass &modbus)
{
    if (xSemaphoreTake(SensorSemaphore, pdMS_TO_TICKS(100)))
    {
        switch (requestIndex)
        {
        case 0:
            SensorData.data_updated = false;
            SensorData.so2 = random(10,20);
            break;
        case 1:
            SensorData.o3 = random(10,20);
            break;
        case 2:
            SensorData.no2 = random(10,20);
            break;
        case 3:
            SensorData.co = random(10,20);
            break;
        case 4:
            SensorData.pm25 = random(10,20);
            break;
        case 5:
            SensorData.pm10 = random(10,20);
            SensorData.data_updated = true;
            break;
        }
        requestIndex = (requestIndex + 1) % (sizeof(modbusRequest) / sizeof(modbusRequest[0]));
        xSemaphoreGive(SensorSemaphore);
    }
}