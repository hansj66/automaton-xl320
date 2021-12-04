#include <Arduino.h>
#include <ESPmDNS.h>
#include <Dynamixel2Arduino.h>
#include <Adafruit_BNO055.h>
#include "time.h"
#include "display.h"
#include "poser.h"
#include "imuconfig.h"
#include <PID_v1.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "webserver.h"

#define RX2 17
#define TX2 16

#define DXL_SERIAL   Serial2
#define DEBUG_SERIAL Serial
#define DXL_DIR_PIN 23


double P_global = 294;
double I_global = 1659;
double D_global = 5.35;
double SetPoint_Delta = 1;

AsyncWebServer server(80);

const float DXL_PROTOCOL_VERSION = 2.0;

double TargetAngle = -90.0;
//Define Variables we'll be connecting to
double PID_Setpoint = TargetAngle + SetPoint_Delta;
double PID_Input;
double PID_Output;

//Specify the links and initial tuning parameters
//double Kp=2, Ki=5, Kd=1;

PID pid(&PID_Input, &PID_Output, &PID_Setpoint, P_global, I_global, D_global, DIRECT);


Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
Display screen;
IMUConfig imuConfig(&bno, &screen);

Poser poser(bno, dxl, pid, screen);

const unsigned long LOOPTIME_MS = 12;



void initPID()
{
  // Initialize PID controller
  Serial.printf("Initializing PID");

  pid.SetOutputLimits(-1023,1023); 
  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(LOOPTIME_MS);
}


void initDynamixel()
{
  // Set Port baudrate to 1Mb. This has to match with DYNAMIXEL baudrate.
  dxl.begin(1000000);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  poser.Begin();
  poser.RelaxArms();
  poser.SetSpeed(0);

  sleep(3);
}


void setup() 
{
  DEBUG_SERIAL.begin(115200);
  
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
  // bno.setMode(Adafruit_BNO055::OPERATION_MODE_ACCGYRO);


  screen.Begin();

  // Configure Wi-Fi and mDNS
  screen.DisplayText("Enabling WiFi\nstation mode.");
  WiFi.mode(WIFI_STA);
  sleep(1);
  screen.DisplayText("Accessing network...");
  sleep(1);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
      screen.DisplayText("WiFi Failed!\n");
      return;
  }
  char buf[64];
  sprintf(buf, "web server at:\nhttp://%s\nhttp://stumbler", WiFi.localIP().toString().c_str());
  screen.DisplayText(buf);
  sleep(2);
  // Make discovery on the network somewhat easier than having to rely on the IP address.
  if (!MDNS.begin("stumbler"))
  {
    screen.DisplayText("Error starting mDNS!");
  }
  screen.DisplayText("Enabling mDNS");
  sleep(1);

  // Configure IMU
  while (!bno.begin())
  {
    Serial.print("Bummer! No BNO055 detected\n");
    delay(1000);
  }
  Serial.print("Yay. BNO055 is present\n");
  imuConfig.Begin();
  imuConfig.Calibrate();

  // Servos
  initDynamixel();

  // PID controller
  initPID();

  // Web server "mux"
  server.onNotFound(notFoundHandler);
  server.on("/", HTTP_GET, landingPageHandler);
  server.on("/param", HTTP_GET, paramAdjustHandler);
  server.on("/save", HTTP_GET, paramSaveHandler);
  server.on("/load", HTTP_GET, paramLoadHandler);
  server.on("/greet", HTTP_GET, greetHandler);
  server.on("/boogie", HTTP_GET, boogieHandler);

  server.begin();

  screen.DisplayText("Setup complete.");
  delay(2);

  // Finally, encode the IP address as a QR code and display it on the OLED
  sprintf(buf, "http://%s", WiFi.localIP().toString().c_str());
  screen.DisplayQRCode(buf);
}


void loop() 
{
  poser.Rise();
  poser.RelaxArms();
  poser.Balance();
}