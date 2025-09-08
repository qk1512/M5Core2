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

void drawSensorBoxes()
{
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(BLACK);

    int boxWidth = 150;
    int boxHeight = 70;
    int padding = 10;

    String labels[6] = {"SO2", "O3", "NO2", "CO", "PM2.5", "PM10"};

    for (int i = 0; i < 6; i++)
    {
        int row = i / 2;
        int col = i % 2;
        int x = col * (boxWidth + padding) + padding;
        int y = row * (boxHeight + padding) + padding;

        M5.Lcd.drawRect(x, y, boxWidth, boxHeight, WHITE);
        M5.Lcd.setCursor(x + 5, y + 5);
        M5.Lcd.printf("%s:", labels[i].c_str());
    }
}

void updateBoxValue(int index, float value)
{
    int boxWidth = 150;
    int boxHeight = 70;
    int padding = 10;

    int row = index / 2;
    int col = index % 2;
    int x = col * (boxWidth + padding) + padding;
    int y = row * (boxHeight + padding) + padding;

    // Clear old value area (just inside the box)
    M5.Lcd.fillRect(x + 5, y + 25, boxWidth - 10, 20, BLACK);

    // Print new value
    M5.Lcd.setCursor(x + 5, y + 25);
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.printf("%.2f", value);
}
