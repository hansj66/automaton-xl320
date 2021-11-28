#include "poser.h"
#include <float.h>
// #include "kalman.h"

//This namespace is required to use Control table item names
using namespace ControlTableItem;

// Kalman k;

extern double PID_Setpoint;
extern double PID_Input;
extern double PID_Output;
extern double TargetAngle;


extern double P_global;
extern double I_global;
extern double D_global;
extern double SetPoint_Delta;

Poser::Poser(Adafruit_BNO055 & bno, Dynamixel2Arduino & dxl, PID & pid, Display & display) :
    _bno(bno),
    _dxl(dxl),
    _pid(pid),
    _screen(display),
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
      _dxl.setGoalVelocity(LEFT_ARM_ID, 100, UNIT_PERCENT);
    delay(4);
    _dxl.setGoalVelocity(RIGHT_ARM_ID, 100, UNIT_PERCENT);
    delay(4);

  while (!_dxl.setGoalVelocity(LEFT_ARM_ID, 100, UNIT_PERCENT))  { delay(2); }
  while (!_dxl.setGoalVelocity(RIGHT_ARM_ID, 100, UNIT_PERCENT))  { delay(2); }
  while (!_dxl.setGoalPosition(LEFT_ARM_ID, 512))  { delay(2); }
  while (!_dxl.setGoalPosition(RIGHT_ARM_ID, 512))  { delay(2); }
}


void Poser::Rise() 
{
    return; // For now
    sensors_event_t event; 
    _bno.getEvent(&event);

    int stopPos = 0;
    _dxl.setGoalVelocity(LEFT_ARM_ID, 50);
    delay(4);
    _dxl.setGoalVelocity(RIGHT_ARM_ID, 50);
    delay(4);

    if ((event.orientation.z < 5) && (event.orientation.z > -90)) 
    {
        _dxl.setGoalPosition(LEFT_ARM_ID, 1023 - stopPos);
        delay(4);
        _dxl.setGoalPosition(RIGHT_ARM_ID, 0 + stopPos);    
        delay(4);
    }
    else {
        _dxl.setGoalPosition(LEFT_ARM_ID, 0 + stopPos);
        delay(4);
        _dxl.setGoalPosition(RIGHT_ARM_ID, 1023 - stopPos);
        delay(4);
    }

    delay(5000);
}

void Poser::SetSpeed(float speed)
{
  // return; // For now
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
  _dxl.setGoalVelocity(RIGHT_MOTOR_ID, uSpeed_1);
  delay(2);
  _dxl.setGoalVelocity(LEFT_MOTOR_ID, uSpeed_2);

  // _dxl.setGoalVelocity(RIGHT_MOTOR_ID, speed, UNIT_PERCENT);
  // delay(2);
  // _dxl.setGoalVelocity(LEFT_MOTOR_ID, speed, UNIT_PERCENT);
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


char buffer[128];


double Poser::AutoTuneD(const char * heading, double minVal, double maxVal, double increment)
{
  double currentParam = minVal;

  unsigned long start_time, stop_time, loop_time;

  std::map<double, double> ssqMap;
  double ssq;

  while (true) {
      Rise();

      _pid.SetTunings(_pid.GetKp(), _pid.GetKi(), currentParam);

      RelaxArms();
      start_time = millis();
      ssq = Balance();
      stop_time=millis();

      loop_time = stop_time-start_time;
      ssqMap[currentParam] = ssq;

      sprintf(buffer, "%s\nParam:%.3f\nSSQ:%.3f (ms)\nP:%.3f\nI:%.3f\nD:%.3f", heading, 
                                                                                  currentParam, 
                                                                                  ssq,  
                                                                                  _pid.GetKp(), 
                                                                                  _pid.GetKi(), 
                                                                                  _pid.GetKd());
      _screen.DisplayText(buffer);
      currentParam += increment;

      if (currentParam > maxVal) {
        break;
      }
  }

  double bestParam = 0;
  double bestSSQ = DBL_MAX;

  std::map<double, double>::iterator it;
  for (it = ssqMap.begin(); it != ssqMap.end(); it++)
  {
      if (it->second < bestSSQ) {
        bestParam = it->first;
        bestSSQ = it->second;
      }
  }

  return bestParam;
}

double Poser::AutoTuneP(const char * heading, double minVal, double maxVal, double increment)
{
  double currentParam = minVal;

  unsigned long start_time, stop_time, loop_time;

  std::map<unsigned long, double> params;

  while (true) {
      Rise();

      _pid.SetTunings(currentParam, _pid.GetKi(), _pid.GetKd());

      RelaxArms();
      start_time = millis();
      Balance();
      stop_time=millis();

      loop_time = stop_time-start_time;
      params[loop_time] = currentParam;

      sprintf(buffer, "%s\nParam:%.3f\ntime up:%d (ms)\nP:%.3f\nI:%.3f\nD:%.3f", heading, 
                                                                                  currentParam, 
                                                                                  (int)loop_time,  
                                                                                  _pid.GetKp(), 
                                                                                  _pid.GetKi(), 
                                                                                  _pid.GetKd());
      _screen.DisplayText(buffer);
      currentParam += increment;

      if (currentParam > maxVal) {
        break;
      }
  }

  double bestParam = 0;
  unsigned long bestTime = 0;
  std::map<unsigned long, double>::iterator it;
  for (it = params.begin(); it != params.end(); it++)
  {
      if (it->second > bestTime) {
        bestTime = it->first;
        bestParam = it->second;
      }
  }

  return bestParam;
}


double Poser::AutoTuneI(const char * heading, double minVal, double maxVal, double increment)
{
  double currentParam = minVal;

  unsigned long start_time, stop_time, loop_time;

  std::map<unsigned long, double> params;

  while (true) {
      Rise();

      _pid.SetTunings(_pid.GetKp(), currentParam, _pid.GetKd());

      RelaxArms();
      start_time = millis();
      Balance();
      stop_time=millis();

      loop_time = stop_time-start_time;
      params[loop_time] = currentParam;

      sprintf(buffer, "%s\nParam:%.3f\ntime up:%d (ms)\nP:%.3f\nI:%.3f\nD:%.3f", heading, 
                                                                                  currentParam, 
                                                                                  (int)loop_time,  
                                                                                  _pid.GetKp(), 
                                                                                  _pid.GetKi(), 
                                                                                  _pid.GetKd());
      _screen.DisplayText(buffer);
      currentParam += increment;

      if (currentParam > maxVal) {
        break;
      }
  }

  double bestParam = 0;
  unsigned long bestTime = 0;
  std::map<unsigned long, double>::iterator it;
  for (it = params.begin(); it != params.end(); it++)
  {
      if (it->second > bestTime) {
        bestTime = it->first;
        bestParam = it->second;
      }
  }

  return bestParam;
}




void Poser::AutoTune()
{
  double best_kP = 1; // Initial value
  double best_kI = 0; // Initial value
  double best_kD = 0; // Initial value

  _pid.SetTunings(best_kP, best_kI, best_kD);

  // Tune Proportional paramter
  best_kP = AutoTuneP("Proportional", best_kP, 100, 2);

  // Tune Integral paramter
  best_kI = AutoTuneI("Integral", best_kI, 700, 10);

  // // Tune Derivative paramter
  best_kD = AutoTuneD("Derivative", best_kD, 100, 2);

  _pid.SetTunings(best_kP, best_kI, best_kD);

  sprintf(buffer, "Tuning complete\nP:%.3f\nI:%.3f\nD:%.3f", _pid.GetKp(), _pid.GetKi(), _pid.GetKd());
  _screen.DisplayText(buffer, 5000);

}


// Slå på setspeed og rise igjen og retest
// Endre mellom aggressive og konservative tuning-parametre

double Poser::Balance()
{
  double mean = 0;
  double sum = 0;
  int samplecount = 0;
  double sampledeviation = 0;
  double sampleDeviationSquared = 0;
  double ssqsum = 0;
  double ssq;

  sensors_event_t event; 
  _bno.getEvent(&event);
  double error = 0;

  while (true)
  {
    _bno.getEvent(&event);

    PID_Setpoint = TargetAngle + SetPoint_Delta;
    error = PID_Setpoint -event.orientation.z;

    _pid.SetTunings(P_global, I_global, D_global);

    // SSQ - TBD
    samplecount++;
    sum += error;
    mean = sum/samplecount;
    sampledeviation = mean-error;
    sampleDeviationSquared = sampledeviation*sampledeviation;
    ssqsum += sampleDeviationSquared;
    ssq = ssqsum/samplecount;

    if (abs(error) > 70)
    {
        SetSpeed(0);
        return ssq;
    }

    PID_Input = event.orientation.z;

    if (_pid.Compute()) {

//Serial.printf("PID_Input: %.3f, PID Output: %.3f\n", PID_Input, PID_Output);

      if (PID_Output > 0 ) {
        Forwards();
      } else if (PID_Output < 0) {
        Reverse();
      } 

      if (PID_Output != 0) {
          SetSpeed(PID_Output);
      }
    }
  }
}
