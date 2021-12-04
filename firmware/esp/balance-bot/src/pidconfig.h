#ifndef _PID_H
#define _PID_H

typedef struct {
    double P;
    double I;
    double D;
    double SetPoint;
} PID_Data_t;

bool loadPID();
void savePID();
    
#endif