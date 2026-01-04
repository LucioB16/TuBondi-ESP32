#ifndef TU_BONDI_CLIENT_H
#define TU_BONDI_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class TuBondiClient {
public:
    TuBondiClient();
    bool initSession();
    JsonDocument fetchArrivals(String& lastUpdateTime);

private:
    String phpSessionId;
};

#endif
