#include "poser.h"

//This namespace is required to use Control table item names
using namespace ControlTableItem;

Poser::Poser(Adafruit_BNO055 & bno, Dynamixel2Arduino & dxl, PID & pid) :
    _bno(bno),
    _dxl(dxl),
    _pid(pid),
    _currentDirection(0)
{

}


void Poser::Begin()
{
  InitServo(LEFT_ARM_ID, OP_POSITION);
  InitServo(RIGHT_ARM_ID, OP_POSITION);

  InitServo(LEFT_MOTOR_ID, OP_VELOCITY);
  InitServo(RIGHT_MOTOR_ID, OP_VELOCITY);

  SetSpeed(0);
}


void Poser::InitServo(uint8_t id, uint8_t mode)
{
  // Get DYNAMIXEL information
  _dxl.ping(id);

  // Turn off torque when configuring items in EEPROM area
  while (!_dxl.torqueOff(id)) { delay(2); }
  while (!_dxl.writeControlTableItem(MIN_VOLTAGE_LIMIT, id, MINIMUM_VOLTAGE_2S_LIPO)) { delay(2); }
  while (!_dxl.writeControlTableItem(SHUTDOWN, id, SHUTDOWN_ON_ERROR)) { delay(2); }
  while (!_dxl.setOperatingMode(id, mode)) { delay(2); }
  while (!_dxl.torqueOn(id)) { delay(2); }
}



void Poser::RelaxArms()
{
  while (!_dxl.setGoalVelocity(LEFT_ARM_ID, 200))  { delay(2); }
  while (!_dxl.setGoalVelocity(RIGHT_ARM_ID, 200))  { delay(2); }
  while (!_dxl.setGoalPosition(LEFT_ARM_ID, 512))  { delay(2); }
  while (!_dxl.setGoalPosition(RIGHT_ARM_ID, 512))  { delay(2); }
}


void Poser::Rise() 
{
    sensors_event_t event; 
    _bno.getEvent(&event);

    int stopPos = 0;
    _dxl.setGoalVelocity(LEFT_ARM_ID, 50);
    _dxl.setGoalVelocity(RIGHT_ARM_ID, 50);

    if ((event.orientation.z < 5) && (event.orientation.z > -90)) 
    {
        _dxl.setGoalPosition(LEFT_ARM_ID, 1023 - stopPos);
        _dxl.setGoalPosition(RIGHT_ARM_ID, 0 + stopPos);    
    }
    else {
        _dxl.setGoalPosition(LEFT_ARM_ID, 0 + stopPos);
        _dxl.setGoalPosition(RIGHT_ARM_ID, 1023 - stopPos);
    }
}

void Poser::SetSpeed(float speed)
{
  unsigned int uSpeed_2 = abs(speed);
  unsigned int uSpeed_1 = uSpeed_2;
  if (uSpeed_1 > 0x3FF)
    uSpeed_1 = 0x3FF;
  if (uSpeed_2 > 0x3FF)
    uSpeed_2 = 0x3FF;

  // uSpeed_1 &= 0x3FF;
  // uSpeed_2 &= 0x3FF;
  if (_currentDirection == REVERSE_DIR)
  {
    uSpeed_1 |= 0x400;
  } else  {
    uSpeed_2 |= 0x400;
  }

  // while (!_dxl.setGoalVelocity(RIGHT_MOTOR_ID, uSpeed_1)) {}
  // while (!_dxl.setGoalVelocity(LEFT_MOTOR_ID, uSpeed_2)) {}
  _dxl.setGoalVelocity(RIGHT_MOTOR_ID, uSpeed_1);
  _dxl.setGoalVelocity(LEFT_MOTOR_ID, uSpeed_2);
}

void Poser::Forwards()
{
  if (_currentDirection != FORWARD_DIR)
  {
    _currentDirection = FORWARD_DIR;
  }
}

void Poser::Reverse()
{
  if (_currentDirection != REVERSE_DIR)
  {
    _currentDirection = REVERSE_DIR;
  }
}

void Poser::AutoTune()
{
    while (true) {
        RelaxArms();
        delay(5000);
        Rise();
        delay(5000);
        RelaxArms();
        Balance();
    }
}



void Poser::Balance()
{
 unsigned long loopTime = 12;
  unsigned long start_time;
  unsigned long dt = loopTime;

  sensors_event_t event; 
  _bno.getEvent(&event);
  double error_new;

  while (true)
  {
    start_time = millis();
    _bno.getEvent(&event);

    error_new = TARGET_ANGLE+event.orientation.z;
    if (abs(error_new) > 70)
    {
        SetSpeed(0);
        return;
    }

    pid_update(&_pid, error_new, dt);

    if (_pid.control < 0 ) {
      Forwards();
    } else if (_pid.control > 0) {
      Reverse();
    } 

    if (_pid.control != 0) {
        SetSpeed(_pid.control);
    }
  
    do {
      dt = (millis() - start_time);
    } while (dt < loopTime);
  }
}
