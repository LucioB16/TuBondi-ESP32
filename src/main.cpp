#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "config.h"
#include "DisplayUI.h"
#include "TuBondiClient.h"
#include "ConfigManager.h"

// Global objects
TFT_eSPI tft = TFT_eSPI();
// Polling mode: remove IRQ pin to force SPI check every time touched() is called
XPT2046_Touchscreen touch(TOUCH_CS_PIN); 
SPIClass touchSPI(HSPI);
TuBondiClient bondiClient;

String lastUpdateStr = "--:--:--";

void setup() {
  Serial.begin(115200);
  ConfigManager::init();
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  Serial.println("\n[SYSTEM] Iniciando WiFi...");
  tft.drawCentreString("Conectando WiFi...", 160, 100, 2);

  WiFi.mode(WIFI_STA); 
  // Intentar conectar con las credenciales que WiFiManager guarda automáticamente
  WiFi.begin(); 
  
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 15) {
    delay(1000);
    Serial.print(".");
    retry++;
  }
  
  // Si no se conecta con las guardadas, intentar con las fijas del config.h
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[LOG] Reintentando con WiFi por defecto...");
    WiFi.begin(ssid, password);
    retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(1000);
      Serial.print(".");
      retry++;
    }
  }

  // Si después de todo falla, AHI SI abrir el portal
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[WARNING] WiFi Falló. Iniciando Portal.");
    ConfigManager::startPortal();
  } else {
    Serial.println("\n[SYSTEM] Conectado!");
    Serial.print("[LOG] IP local: ");
    Serial.println(WiFi.localIP());
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  tft.fillScreen(TFT_BLACK);
  tft.drawCentreString("Sincronizando Hora...", 160, 100, 2);
  struct tm timeinfo;
  while(!getLocalTime(&timeinfo)) {
    delay(500);
  }

  touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS_PIN);
  touch.begin(touchSPI);
  touch.setRotation(1);

  bondiClient.initSession();
  JsonDocument doc = bondiClient.fetchArrivals(lastUpdateStr);
  updateDisplay(doc["proximos_arribos"], lastUpdateStr);
}

void loop() {
  static unsigned long lastUpdateMillis = 0;
  unsigned long now = millis();
  
  // Update arrivals every 25 seconds
  if (now - lastUpdateMillis > 25000 || lastUpdateMillis == 0) {
    Serial.println("[SYSTEM] Actualizando Arribos...");
    JsonDocument doc = bondiClient.fetchArrivals(lastUpdateStr);
    updateDisplay(doc["proximos_arribos"], lastUpdateStr);
    lastUpdateMillis = millis();
  }
  
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    // Log EVERY raw point initially to debug
    // Filter only ONLY if both are 0 or both are 8191 (extreme cases)
    if ((p.x == 0 && p.y == 0) || (p.x == 8191 && p.y == 8191)) {
        // Skip ghost spam
    } else {
        int touchX = map(p.x, 200, 3720, 0, 320); 
        int touchY = map(p.y, 240, 3850, 0, 240);
        
        Serial.printf("[DEBUG] Raw(%d,%d,Z=%d) -> Screen(%d,%d)\n", p.x, p.y, p.z, touchX, touchY);
        
        // Always draw a dot for feedback
        tft.fillCircle(touchX, touchY, 3, TFT_WHITE);

        if (p.z > 200) { // Require some pressure for button action
            if (touchY > 180) {
                if (touchX > 240) {
                    Serial.println(">>> [LOG] Refresh presionado <<<");
                    tft.fillRect(240, 205, 75, 30, MATRIX_ORANGE);
                    tft.drawCentreString("REFRESH", 277, 212, 2);
                    JsonDocument doc = bondiClient.fetchArrivals(lastUpdateStr);
                    updateDisplay(doc["proximos_arribos"], lastUpdateStr);
                    lastUpdateMillis = millis();
                } 
                else if (touchX > 180 && touchX <= 240) {
                    Serial.println(">>> [LOG] Modo Configuración activado <<<");
                    ConfigManager::startPortal();
                }
            }
            delay(100); 
        }
    }
  }

  static int lastSec = -1;
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (timeinfo.tm_sec != lastSec) {
        lastSec = timeinfo.tm_sec;
        tft.fillRect(240, 0, 80, 28, TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.drawRightString(getTimestamp(), 310, 8, 2);
    }
  }

  delay(20); 
}
