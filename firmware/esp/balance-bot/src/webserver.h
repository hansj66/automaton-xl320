#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void notFoundHandler(AsyncWebServerRequest *request);
void landingPageHandler(AsyncWebServerRequest *request);
void paramAdjustHandler(AsyncWebServerRequest *request);
void paramSaveHandler(AsyncWebServerRequest *request);
void paramLoadHandler(AsyncWebServerRequest *request);
void greetHandler(AsyncWebServerRequest *request);
void boogieHandler(AsyncWebServerRequest *request);
