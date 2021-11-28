#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void notFoundHandler(AsyncWebServerRequest *request);
void landingPageHandler(AsyncWebServerRequest *request);
void paramAdjustHandler(AsyncWebServerRequest *request);
