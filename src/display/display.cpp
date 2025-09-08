#include "display.h"
void updateLCD(float so2Offset, float o3Offset, float no2Offset, float coOffset, float pm2Offset, float pm10Offset, float so2, float o3, float no2, float co, float pm2, float pm10)
{
    M5.Lcd.fillRect(0, 0, 320, 240, BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("SO2: %.2f (Offset: %.2f)\n", so2 + so2Offset, so2Offset);
    M5.Lcd.printf("O3: %.2f (Offset: %.2f)\n", o3 + o3Offset, o3Offset);
    M5.Lcd.printf("NO2: %.2f (Offset: %.2f)\n", no2 + no2Offset, no2Offset);
    M5.Lcd.printf("CO: %.2f (Offset: %.2f)\n", co + coOffset, coOffset);
    M5.Lcd.printf("PM2.5: %.2f (Offset: %.2f)\n", pm2 + pm2Offset, pm2Offset);
    M5.Lcd.printf("PM10: %.2f (Offset: %.2f)\n", pm10 + pm10Offset, pm10Offset);
}