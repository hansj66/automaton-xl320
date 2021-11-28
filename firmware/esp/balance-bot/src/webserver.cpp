#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern String config_page;

double P_global = 0;
double I_global = 0;
double D_global = 0;
double SetPoint_Delta = 0;

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


