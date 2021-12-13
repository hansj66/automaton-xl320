#ifndef _POSER_H_
#define _POSER_H

#include <Adafruit_BNO055.h>
#include <Dynamixel2Arduino.h>
//#include "pid.h"
#include <PID_v1.h>
#include "display.h"
#include <map>


const uint8_t LEFT_MOTOR_ID = 1;
const uint8_t RIGHT_MOTOR_ID = 2;
const uint8_t LEFT_ARM_ID = 3;
const uint8_t RIGHT_ARM_ID = 4;

const int MINIMUM_VOLTAGE_2S_LIPO = 70; // == 7V

const int OVERLOAD_ERROR      = 1 << 0;
const int OVERHEATING_ERROR   = 1 << 1;
const int UNDERVOLTAGE_ERROR  = 1 << 2;
const int SHUTDOWN_ON_ERROR = OVERLOAD_ERROR | OVERHEATING_ERROR | UNDERVOLTAGE_ERROR;

const int FORWARD_DIR = 1;
const int REVERSE_DIR = -1;

class Poser
{
    public:
     Poser(Adafruit_BNO055 & bno, Dynamixel2Arduino & dxl, PID & pid, Display & display);
     void Rise();
     void RelaxArms();
     void InitServo(uint8_t id, uint8_t mode);
     void Begin();
     double Balance();
     void Forwards();
     void Reverse();
     void SetSpeed(float speed);

    private:
     Adafruit_BNO055 & _bno;
     Dynamixel2Arduino & _dxl;
     PID & _pid;
     Display & _screen;

     int _currentDirection;




};


#endif // _POSER_H_