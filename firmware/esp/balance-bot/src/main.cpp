#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include <Adafruit_BNO055.h>
#include "pid.h"
#include "time.h"
#include "display.h"
#include "poser.h"
#include "kalman.h"
#include "imuconfig.h"

#define RX2 17
#define TX2 16

#define DXL_SERIAL   Serial2
#define DEBUG_SERIAL Serial
#define DXL_DIR_PIN 23
 
const float DXL_PROTOCOL_VERSION = 2.0;


const int PID_CONTROL_MAX = 0x3FF;

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
PID pid;
Display screen;
IMUConfig config(&bno, &screen);

Poser poser(bno, dxl, pid);




void initPID()
{
  // Initialize PID controller
  Serial.printf("Initializing PID");

  // Uten ballast, men med USB koblet til
  pid_zeroize(&pid, PID_CONTROL_MAX);
  pid.windup_guard = 0; 
  pid.proportional_gain = 370;
  pid.integral_gain = 0;
  pid.derivative_gain = 0;
}


void initDynamixel()
{
  // Set Port baudrate to 1Mb. This has to match with DYNAMIXEL baudrate.
  dxl.begin(1000000);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  poser.Begin();

  poser.RelaxArms();
  sleep(3);
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

  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
//  bno.setMode(Adafruit_BNO055::OPERATION_MODE_ACCGYRO);


  screen.Begin();
  config.Begin();
  config.Calibrate();

  initDynamixel();
  initPID();


}


void loop() 
{
//    poser.AutoTune();

  Kalman k;

  sensors_event_t event; 

  // sensors_vec_t a;
  // sensors_vec_t g;

  char buf[265];
  unsigned long loopTime = 10;
  unsigned long dt = loopTime;
  unsigned long start_time;

  // float kA;

  pid.proportional_gain = 200;
  pid.integral_gain = 0;
  pid.derivative_gain = 0;

  int count = 0;

  while (true) 
  {
    count++;


    start_time = millis();

    // bno.getEvent(&event, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    // a = event.acceleration;
    // bno.getEvent(&event, Adafruit_BNO055::VECTOR_GYROSCOPE);
    // g = event.gyro;
    // kA = k.getAngle(a.z, g.x, dt);
    // double error_new = kA+0.3;

    bno.getEvent(&event);
    double error_new = TARGET_ANGLE+event.orientation.z;

    if (count > 50) {
      sprintf(buf, "angle: %3f", event.orientation.z);
      screen.DisplayText(buf);
      count = 0;
    }


    pid_update(&pid, error_new, dt);
    if (pid.control > 0 ) {
      poser.Reverse();
    } else if (pid.control < 0) {
      poser.Forwards();
    } 
    if (pid.control != 0) {
     poser.SetSpeed(pid.control);
    }

    dt = (millis() - start_time);

    // do {
    //   dt = (millis() - start_time);
    // } while (dt < loopTime);
  }


//   unsigned long loopTime = 12;
//   unsigned long start_time;
//   unsigned long dt = loopTime;

//   sensors_event_t event; 
//   bno.getEvent(&event);
//   double pid_input;
//   double error_new = TARGET_ANGLE+event.orientation.z;
// //  double error_old = error_new;
//   //double alpha = 0.98;


//   char buf[265];
//   while (true)
//   {
//     start_time = millis();
//     bno.getEvent(&event);

//   //  error_old = error_new;
//     error_new = TARGET_ANGLE+event.orientation.z;
//     //pid_input = (alpha * error_new + (1-alpha)*error_old) / 2;


// //    pid_update(&pid, pid_input, dt);
//     pid_update(&pid, error_new, dt);

//     if (pid.control < 0 ) {
//       poser.Forwards();
//     } else if (pid.control > 0) {
//       poser.Reverse();
//     } 

//     // sprintf(buf, "e: %.3f, c: %.3f, a: %.3f", pid_input, pid.control, event.orientation.z);
//     // Serial.println(buf);
//     //sprintf(buf, "c: %d, e: %.3f",bno.isFullyCalibrated(), error_new);
//     //Serial.println(buf);

//     if (pid.control != 0) {
//   //    poser.SetSpeed(pid.control);
//     }
  
//     do {
//       dt = (millis() - start_time);
//     } while (dt < loopTime);


//   }

}