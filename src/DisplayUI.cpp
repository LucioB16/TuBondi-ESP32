#include "DisplayUI.h"
#include "config.h"
#include <time.h>

void drawBackgroundGrid() {
  tft.fillScreen(TFT_BLACK);
  for (int x = 2; x < 320; x += 4) {
    for (int y = 2; y < 240; y += 4) {
      tft.drawPixel(x, y, MATRIX_DIM_GRAY);
    }
  }
}

String getTimestamp() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "00:00:00";
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  return String(timeStr);
}

void drawUIHeader() {
  tft.fillRect(0, 0, 320, 30, TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  String headerText = currentStopCode + " - DUARTE QUIROS";
  tft.drawString(headerText, 10, 8, 2);
  tft.drawRightString(getTimestamp(), 310, 8, 2);
  tft.drawFastHLine(0, 30, 320, MATRIX_BLUE);
}

void drawConfigIcon() {
  int cx = 215; // Center X near refresh
  int cy = 220; // Center Y
  int r_in = 3;
  int r_out = 8;
  int r_teeth = 12;

  tft.fillRect(195, 205, 40, 30, tft.color565(30, 30, 30));
  tft.drawRect(195, 205, 40, 30, MATRIX_DIM_GRAY);
  
  tft.drawCircle(cx, cy, r_out, CONFIG_ICON_COLOR);
  tft.drawCircle(cx, cy, r_in, CONFIG_ICON_COLOR);
  
  for (int i = 0; i < 8; i++) {
    float angle = i * 45 * PI / 180.0;
    float offset = 2.0 * PI / 180.0; // Thicker teeth
    
    for (float a = angle - offset; a <= angle + offset; a += offset) {
      int x1 = cx + cos(a) * r_out;
      int y1 = cy + sin(a) * r_out;
      int x2 = cx + cos(a) * r_teeth;
      int y2 = cy + sin(a) * r_teeth;
      tft.drawLine(x1, y1, x2, y2, CONFIG_ICON_COLOR);
    }
  }
}

void drawRefreshButton() {
  tft.fillRect(240, 205, 75, 30, 0x0194); // tft.color565(0, 50, 100)
  tft.drawRect(240, 205, 75, 30, MATRIX_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("REFRESH", 277, 212, 2);
}

void updateDisplay(JsonArray arrivals, String lastUpdate) {
  drawBackgroundGrid();
  drawUIHeader();
  drawRefreshButton();
  drawConfigIcon(); // Draw it here now
  
  int y = 45;
  Serial.println("\n--------------------------------------------------");
  Serial.println("| Linea | Destino / Proximo        | Distancia   |");
  Serial.println("--------------------------------------------------");

  if (arrivals.size() == 0) {
    tft.setTextColor(MATRIX_ORANGE);
    tft.drawCentreString("SIN ARRIBOS DISPONIBLES", 160, 120, 2);
    Serial.println("| NO HAY ARRIBOS PROXIMOS                        |");
  } else {
    for (JsonObject v : arrivals) {
      const char* linea = v["linea"] | "???";
      const char* proximo = v["proximo"] | "";
      const char* demora = v["demora"] | "";
      int distParada = v["dist_parada"] | -1;
      
      String displayDemora = String(demora);
      if (displayDemora == "" || displayDemora == "null") {
        if (distParada > 0) displayDemora = String(distParada) + "m";
        else displayDemora = "---";
      }

      char buf[80];
      snprintf(buf, sizeof(buf), "| %-5s | %-24s | %-11s |", linea, proximo, displayDemora.c_str());
      Serial.println(buf);

      uint16_t color = MATRIX_ORANGE;
      if (linea && strcmp(linea, "40") == 0) color = TFT_GREEN;
      if (linea && strcmp(linea, "42") == 0) color = TFT_CYAN;

      tft.setTextColor(color);
      tft.drawString(linea, 10, y, 4);
      
      tft.setTextColor(TFT_WHITE);
      String label = String(proximo);
      if (label.length() > 18) label = label.substring(0, 15) + "...";
      tft.drawString(label, 70, y + 5, 2);
      
      tft.setTextColor(MATRIX_BLUE);
      tft.drawRightString(displayDemora, 310, y, 4);
      
      y += 40;
      if (y > 200) break;
    }
  }
  Serial.println("--------------------------------------------------\n");
  
  tft.setTextColor(0x632C); // tft.color565(100, 100, 100)
  tft.drawString("Ult. Refresco: " + lastUpdate, 10, 215, 2);
}
