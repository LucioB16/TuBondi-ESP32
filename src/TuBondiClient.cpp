#include "TuBondiClient.h"
#include "config.h"
#include "DisplayUI.h"
#include <WiFi.h>

TuBondiClient::TuBondiClient() : phpSessionId("") {}

bool TuBondiClient::initSession() {
  HTTPClient http;
  String url = currentBaseUrl + "/web/urbano/?conf=" + currentConf;
  
  const char* headers[] = {"Set-Cookie"};
  http.collectHeaders(headers, 1);
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_FOUND) {
    String cookie = http.header("Set-Cookie");
    if (cookie.length() > 0) {
      int start = cookie.indexOf("PHPSESSID=");
      if (start != -1) {
        int end = cookie.indexOf(";", start);
        phpSessionId = cookie.substring(start, end);
        http.end();
        return true;
      }
    }
  }
  http.end();
  return false;
}

JsonDocument TuBondiClient::fetchArrivals(String& lastUpdateTime) {
  JsonDocument doc;
  if (WiFi.status() != WL_CONNECTED) return doc;

  HTTPClient http;
  String url = currentBaseUrl + "/usuario/urbano2_cmd.php?cmd=proximos_arribos";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
  if (phpSessionId != "") {
    http.addHeader("Cookie", phpSessionId);
  }
  
  String payload = "conf=" + currentConf + "&codigo=" + currentStopCode + "&show80min=true&onlygps_array=%7B%7D";
  
  int httpCode = http.POST(payload);
  
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      lastUpdateTime = getTimestamp();
    } else {
      Serial.print("JSON Parse error: "); Serial.println(error.c_str());
    }
  } else {
    if (httpCode == 403 || httpCode == 401) initSession();
  }
  http.end();
  return doc;
}
