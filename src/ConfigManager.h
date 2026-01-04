#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

class ConfigManager {
public:
    static void init();
    static void load();
    static void save(String stopCode, String conf);
    static void startPortal();
    static bool isConfigMode;
};

#endif
