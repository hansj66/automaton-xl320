// Nicked this code from https://nicisdigital.wordpress.com/2011/06/27/proportional-integral-derivative-pid-controller/

#include "pidconfig.h"
#include "display.h"
#include <vector>
#include <string.h>
#include <stdio.h>
#include "ArduinoNvs.h"

const char * PID_BLOB_NAME = "pid";

const int MESSAGE_GRACE_PERIOD = 1000;

//extern Display screen;
extern double P_global;
extern double I_global;
extern double D_global;
extern double PID_Setpoint;

bool loadPID()
{
    Serial.printf("Checking for NVS\nPID data\n");
    // screen.DisplayText("Checking for NVS\nPID data", MESSAGE_GRACE_PERIOD);

    std::vector<uint8_t> PID_Data;

    if (NVS.getBlob(PID_BLOB_NAME, PID_Data)) {
        
        Serial.printf("NVS PID\nfound\n");
        // screen.DisplayText("NVS PID\nfound", MESSAGE_GRACE_PERIOD);
        if (PID_Data.size() == sizeof(PID_Data_t)) {
                PID_Data_t pid;
                memcpy(&pid, PID_Data.data(), PID_Data.size());

                P_global = pid.P;
                I_global = pid.I;
                D_global = pid.D;
                PID_Setpoint = pid.SetPoint;
                Serial.printf("PID data loaded\nfrom NVS\n");

                Serial.printf("P:%.3f\n", pid.P);
                Serial.printf("I:%.3f\n", pid.I);
                Serial.printf("D:%.3f\n", pid.D);
                Serial.printf("Setpoint:%.3f\n", pid.SetPoint);

                // screen.DisplayText("PID data loaded\nfrom NVS", MESSAGE_GRACE_PERIOD);
                return true;
        }
        else {
            char buf[128];
            sprintf(buf, "Found %d bytes\nof PID data\nexpected %d bytes. Unable to initialize PID...\n", PID_Data.size(), sizeof(PID_Data_t));
            Serial.printf(buf);
            // screen.DisplayText(buf, 1000);
        }
    }
    else {
        Serial.printf("NVS PID data\nnot found\n");
        // screen.DisplayText("NVS PID data\nnot found",MESSAGE_GRACE_PERIOD);
    }
    return false;
}

void savePID()
{
    PID_Data_t pid;

    pid.P = P_global;
    pid.I = I_global;
    pid.D = D_global;
    pid.SetPoint = PID_Setpoint;


    if (NVS.setBlob(PID_BLOB_NAME, (uint8_t *)&pid, sizeof(PID_Data_t)))
    {
        Serial.printf("Writing PID data into NVS\n");
        Serial.printf("P:%.3f\n", pid.P);
        Serial.printf("I:%.3f\n", pid.I);
        Serial.printf("D:%.3f\n", pid.D);
        Serial.printf("Setpoint:%.3f\n", pid.SetPoint);
        // screen.DisplayText("PID data\nwritten to NVS\nOK", MESSAGE_GRACE_PERIOD);
    }
    else
    {
        Serial.printf("Failed writing PID data into NVS\n");
        // screen.DisplayText("FAILED\nWriting PID data\nto NVS\nOK", MESSAGE_GRACE_PERIOD);
    }

}
