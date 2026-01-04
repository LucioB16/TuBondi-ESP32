#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Credentials
extern const char* ssid;
extern const char* password;

// API Config (Dynamic)
extern String currentBaseUrl;
extern String currentStopCode;
extern String currentConf;

// Pins for Touch (HSPI)
#define TOUCH_CS_PIN  15
#define TOUCH_IRQ_PIN 16
#define TOUCH_CLK     14
#define TOUCH_MISO    12
#define TOUCH_MOSI    13

// LED Matrix Colors
#define MATRIX_DIM_GRAY 0x18C3 // tft.color565(30, 30, 30)
#define MATRIX_ORANGE   0xFD80 // tft.color565(255, 120, 0)
#define MATRIX_BLUE     0x04BF // tft.color565(0, 150, 255)
#define CONFIG_ICON_COLOR 0xAD75 // tft.color565(173, 170, 160)

// NTP Config
extern const char* ntpServer;
extern const long  gmtOffset_sec;
extern const int   daylightOffset_sec;

#endif
