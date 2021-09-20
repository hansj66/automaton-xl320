#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include <Adafruit_BNO055.h>
#include "pid.h"
#include "time.h"

#define RX2 17
#define TX2 16

#define DXL_SERIAL   Serial2
#define DEBUG_SERIAL Serial
#define DXL_DIR_PIN 23
 
const uint8_t LEFT_DXL_ID = 1;
const uint8_t RIGHT_DXL_ID = 2;
const float DXL_PROTOCOL_VERSION = 2.0;

const int MINIMUM_VOLTAGE_2S_LIPO = 70; // == 7V
const int OVERLOAD_ERROR      = 1 << 0;
const int OVERHEATING_ERROR   = 1 << 1;
const int UNDERVOLTAGE_ERROR  = 1 << 2;
const int SHUTDOWN_ON_ERROR = OVERLOAD_ERROR | OVERHEATING_ERROR | UNDERVOLTAGE_ERROR;

const unsigned int MICROSECOND = 1;
const unsigned int MILLISECOND = 1000;
const unsigned int SECOND = 1000000;

const int FORWARD_DIR = 1;
const int REVERSE_DIR = -1;
int CURRENT_DIRECTION = 0;

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);


void initPID()
{
  // Initialize PID controller
  Serial.printf("Initializing PID");
  PID pid;
  pid_zeroize(&pid);
  pid.windup_guard = 0; 
  pid.proportional_gain = 95;
  pid.integral_gain = 0.75;
  pid.derivative_gain = 0.5;
}

//This namespace is required to use Control table item names
using namespace ControlTableItem;

void initServo(uint8_t id)
{
  // Get DYNAMIXEL information
  dxl.ping(id);

  // Turn off torque when configuring items in EEPROM area
  dxl.torqueOff(id);
  dxl.writeControlTableItem(MIN_VOLTAGE_LIMIT, id, MINIMUM_VOLTAGE_2S_LIPO);
  dxl.writeControlTableItem(SHUTDOWN, id, SHUTDOWN_ON_ERROR);
  dxl.setOperatingMode(id, OP_VELOCITY);
  dxl.torqueOn(id);

}

void initDynamixel()
{
  // Set Port baudrate to 1Mb. This has to match with DYNAMIXEL baudrate.
  dxl.begin(1000000);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  initServo(LEFT_DXL_ID);
  initServo(RIGHT_DXL_ID);
}

void setup() 
{
  DEBUG_SERIAL.begin(115200);
  while (!bno.begin())
  {
    Serial.print("Bummer! No BNO055 detected\n");
    delay(1000);
  }
  Serial.print("Yay. BNO055 is present\n");

  initDynamixel();
  initPID();
}

void forwards()
{
  if (CURRENT_DIRECTION != FORWARD_DIR)
  {
    Serial.println("Forwards");
    CURRENT_DIRECTION = FORWARD_DIR;
  }
}

void reverse()
{
  if (CURRENT_DIRECTION != REVERSE_DIR)
  {
    Serial.println("Forwards");
    CURRENT_DIRECTION = REVERSE_DIR;
  }
}

void setSpeed(float speed)
{
  unsigned int uSpeed_2 = abs(speed * 1000);
  unsigned int uSpeed_1 = uSpeed_2;
  if (uSpeed_1 > 0x3FF)
    uSpeed_1 = 0x3FF;
  if (uSpeed_2 > 0x3FF)
    uSpeed_2 = 0x3FF;

  // uSpeed_1 &= 0x3FF;
  // uSpeed_2 &= 0x3FF;
  if (CURRENT_DIRECTION == REVERSE_DIR)
  {
    uSpeed_1 |= 0x400;
  } else  {
    uSpeed_2 |= 0x400;
  }

  dxl.setGoalVelocity(RIGHT_DXL_ID, uSpeed_1);
  dxl.setGoalVelocity(LEFT_DXL_ID, uSpeed_2);
}

void loop() 
{
  // unsigned long loopTime = MICROSECOND * 100;
  unsigned long loopTime = MILLISECOND * 10;
  unsigned long start_time, end_time;
  unsigned long dt = loopTime;

  // Initialize PID controller
  PID pid;
  pid_zeroize(&pid);
  pid.proportional_gain = 0.4;
  pid.integral_gain = 0;  // TBD
  pid.derivative_gain = 0; // TBD
  pid.windup_guard = 0.01;

  sensors_event_t event; 
  bno.getEvent(&event);
  float alpha = 0.5;    // Filter

  float zOld = event.orientation.z;
  float zNew = zOld;
  float TARGET_ANGLE = 90;

  while (true)
  {
    start_time = micros();
    bno.getEvent(&event);

    float error = TARGET_ANGLE+event.orientation.z;
    // Smoothing out the IMU response a bit
    zNew = (alpha * error + (1-alpha)*zOld) / 2;
    pid_update(&pid, zNew, dt);
    zOld = zNew;

    if (error < 0 ) {
      forwards();
    } else if (error > 0) {
      reverse();
    } 

    if (error != 0) {
      setSpeed(pid.control);
    }
  
    end_time = micros();
    dt = end_time - start_time;
    while (true) {
      end_time = micros();
      if (end_time > start_time+loopTime)
        break;
    }
  }
}