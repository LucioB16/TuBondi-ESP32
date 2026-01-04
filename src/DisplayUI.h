#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <TFT_eSPI.h>
#include <ArduinoJson.h>

extern TFT_eSPI tft;

void drawBackgroundGrid();
void drawUIHeader();
void drawRefreshButton();
void drawConfigIcon();
void updateDisplay(JsonArray arrivals, String lastUpdate);
String getTimestamp();

#endif
