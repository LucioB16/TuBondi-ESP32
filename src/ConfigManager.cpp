#include "ConfigManager.h"
#include "config.h"
#include "DisplayUI.h"
#include <Preferences.h>
#include <WiFiManager.h>
#include <XPT2046_Touchscreen.h>

extern XPT2046_Touchscreen touch;

Preferences preferences;
bool ConfigManager::isConfigMode = false;

void ConfigManager::init() {
    preferences.begin("bondi", false);
    load();
}

void ConfigManager::load() {
    currentStopCode = preferences.getString("stop", "A911");
    currentConf = preferences.getString("conf", "cbaciudad");
    Serial.println("Config loaded: " + currentStopCode + " - " + currentConf);
}

void ConfigManager::save(String stopCode, String conf) {
    preferences.putString("stop", stopCode);
    preferences.putString("conf", conf);
    currentStopCode = stopCode;
    currentConf = conf;
    Serial.println("Config saved.");
}

void ConfigManager::startPortal() {
    isConfigMode = true;
    WiFiManager wm;
    
    // Draw Config Screen
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(MATRIX_BLUE);
    tft.drawCentreString("MODO CONFIGURACION", 160, 20, 2);
    
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("1. Cambia tu WiFi a:", 160, 60, 2);
    tft.setTextColor(MATRIX_ORANGE);
    tft.drawCentreString("BondiTracker-Setup", 160, 85, 4);
    
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("2. Entra en tu navegador a:", 160, 125, 2);
    tft.setTextColor(TFT_CYAN);
    tft.drawCentreString("http://192.168.4.1", 160, 150, 2);
    
    // BACK button area
    tft.fillRect(100, 180, 120, 40, tft.color565(100, 0, 0));
    tft.drawRect(100, 180, 120, 40, TFT_WHITE);
    tft.drawCentreString("VOLVER", 160, 190, 2);

    WiFiManagerParameter custom_stop("stop", "Codigo Parada", currentStopCode.c_str(), 10);
    WiFiManagerParameter custom_conf("conf", "Conf API", currentConf.c_str(), 20);
    wm.addParameter(&custom_stop);
    wm.addParameter(&custom_conf);

    wm.setConfigPortalBlocking(false);
    wm.setConfigPortalTimeout(180);

    if (wm.startConfigPortal("BondiTracker-Setup")) {
        Serial.println("AP Started: BondiTracker-Setup");
    }

    String ipAddr = WiFi.softAPIP().toString();
    if (ipAddr == "0.0.0.0" || ipAddr == "127.0.0.1") ipAddr = WiFi.localIP().toString();
    
    // Update screen with actual URL if it changed
    tft.fillRect(0, 145, 320, 30, TFT_BLACK);
    tft.setTextColor(TFT_CYAN);
    tft.drawCentreString("http://" + ipAddr, 160, 150, 2);
    
    Serial.print("Access Portal at: http://");
    Serial.println(ipAddr);

    // Interactive Loop for Config Mode
    unsigned long portalStart = millis();
    while (isConfigMode) {
        wm.process(); // Handle web portal logic
        
        if (touch.touched()) {
            TS_Point p = touch.getPoint();
            
            // FILTRADO CRITICO
            bool isValid = (p.x > 50 && p.x < 4000 && p.y > 50 && p.y < 4000 && p.z > 200 && p.z < 4000);

            if (isValid) {
                int touchX = map(p.x, 200, 3700, 0, 320); 
                int touchY = map(p.y, 240, 3800, 0, 240);
                
                Serial.printf("[TOUCH-CFG] X=%d, Y=%d (Z=%d)\n", touchX, touchY, p.z);
                tft.fillCircle(touchX, touchY, 2, TFT_WHITE);

                // VOLVER button area: (100, 180, 120, 40)
                if (touchX > 80 && touchX < 240 && touchY > 150 && touchY < 240) {
                    Serial.println(">>> [ACTION] Botón Volver <<<");
                    tft.fillRect(100, 180, 120, 40, MATRIX_ORANGE);
                    tft.setTextColor(TFT_BLACK);
                    tft.drawCentreString("Saliendo...", 160, 190, 2);
                    delay(500);
                    isConfigMode = false;
                    ESP.restart();
                }
                delay(200); 
            }
        }
        
        if (millis() - portalStart > 180000) { 
            Serial.println("[LOG] Timeout modo configuracion");
            isConfigMode = false;
        }
        delay(20);
    }
    
    save(custom_stop.getValue(), custom_conf.getValue());
    isConfigMode = false;
    ESP.restart();
}
