#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "pidconfig.h"

extern String config_page;

extern double P_global;
extern double I_global;
extern double D_global;
extern double SetPoint_Delta;

void notFoundHandler(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "Not found");
}

void landingPageHandler(AsyncWebServerRequest *request)
{
    request->send(200, "text/html", config_page);
}

void paramAdjustHandler(AsyncWebServerRequest *request)
{
    Serial.printf("paramAdjustHandler says Hello!\n");

    String param;
    if (request->hasParam("setpoint")) 
    {
        param = request->getParam("setpoint")->value();
        SetPoint_Delta = atoi(param.c_str())*0.04;
    } 
    if (request->hasParam("P")) 
    {
        param = request->getParam("P")->value();
        P_global = atoi(param.c_str());
    } 
    if (request->hasParam("I")) 
    {
        param = request->getParam("I")->value();
        I_global = atoi(param.c_str());
    } 
    if (request->hasParam("D")) 
    {
        param = request->getParam("D")->value();
        D_global = atoi(param.c_str()) * 0.05;
    } 
    request->send(200, "text/plain", "OK");
}

void paramSaveHandler(AsyncWebServerRequest *request)
{
    paramAdjustHandler(request);
    savePID();
    request->send(200, "text/plain", "OK");
}

void paramLoadHandler(AsyncWebServerRequest *request)
{
    char responseData[128] = {0};
    if (loadPID()) {
        sprintf(responseData, "%.3f,%.3f,%.3f,%.3f,", P_global, I_global, D_global, SetPoint_Delta);
        Serial.printf("Sending responsedata: %s\n", responseData);
    }
    request->send(200, "text/html", responseData);
 }

void greetHandler(AsyncWebServerRequest *request)
{
    Serial.printf("greetHandler called\n");
    request->send(200, "text/html", "OK");
}

void boogieHandler(AsyncWebServerRequest *request)
{
    Serial.printf("boogieHandler called\n");
    request->send(200, "text/html", "OK");
}


