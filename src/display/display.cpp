#include "display.h"

void drawSensorBoxes()
{
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);

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

/* void updateBoxValue(int index, float value)
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
 */

void updateBoxValue(int index, const char *label, float value, float offset, float rate)
{
    int boxWidth = 150;
    int boxHeight = 70;
    int padding = 10;

    int row = index / 2;
    int col = index % 2;
    int x = col * (boxWidth + padding) + padding;
    int y = row * (boxHeight + padding) + padding;

    // Draw box background
    M5.Lcd.fillRect(x, y, boxWidth, boxHeight, DARKGREY);
    M5.Lcd.drawRect(x, y, boxWidth, boxHeight, WHITE);

    // Sensor label (white)
    M5.Lcd.setCursor(x + 5, y + 5);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, DARKGREY);
    M5.Lcd.printf("%s: ", label);

    // Sensor value (green, aligned right after label)
    int labelWidth = strlen(label) * 12; // estimate text width (12px per char)
    M5.Lcd.setCursor(x + 5 + labelWidth + 10, y + 5);
    M5.Lcd.setTextColor(GREEN, DARKGREY);
    M5.Lcd.printf("%.2f", value);

    /* // Offset value on a new line
    M5.Lcd.setCursor(x + 5, y + 40);
    M5.Lcd.setTextColor(YELLOW, DARKGREY);
    M5.Lcd.setTextSize(1.5);
    M5.Lcd.printf("Offset: %.2f", offset); */
    // Offset (next line, yellow)
    M5.Lcd.setCursor(x + 5, y + 30);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(YELLOW, DARKGREY);
    M5.Lcd.printf("Offset: %.2f", offset);

    // Rate (next line, cyan)
    M5.Lcd.setCursor(x + 5, y + 50);
    M5.Lcd.setTextColor(CYAN, DARKGREY);
    M5.Lcd.printf("Rate:   %.2f", rate);
}
