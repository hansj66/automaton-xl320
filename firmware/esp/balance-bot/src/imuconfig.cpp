#include "imuconfig.h"
#include "display.h"
#include "ArduinoNvs.h"
#include <vector>

ArduinoNvs nvs;

const char * CALIBRATION_BLOB_NAME = "cal";
const int MESSAGE_GRACE_PERIOD = 1000;

IMUConfig::IMUConfig(Adafruit_BNO055 * bno, Display * screen) :
    _calibrationDataLoaded(false),
    _bno(bno),
    _screen(screen)
{
}

void IMUConfig::Begin()
{
    NVS.begin();
}

void IMUConfig::Calibrate()
{

    _screen->DisplayText("Checking for NVS\ncalibration data", MESSAGE_GRACE_PERIOD);

    if (NVS.getBlob(CALIBRATION_BLOB_NAME, _calibrationData)) {
        _screen->DisplayText("NVS Calibration\nfound", MESSAGE_GRACE_PERIOD);
        if (_calibrationData.size() == CALIBRATION_DATA_SIZE) {
            _calibrationDataLoaded = true; 
        }
        else {
            char buf[128];
            sprintf(buf, "Found %d bytes\nof calibration data\nexpected %d bytes\nRecalibrating...", _calibrationData.size(), CALIBRATION_DATA_SIZE);
            _screen->DisplayText(buf, 1000);
        }
        // Calibration data is found in NVS, but should we
        // force recalibration anyway ? (In case the device is powered on while held upside down)

        sensors_event_t event; 
        _bno->getEvent(&event);
        if (event.orientation.z > 0)
        {
            _calibrationDataLoaded = false;
            _screen->DisplayText("Forcing recalibration", MESSAGE_GRACE_PERIOD);
        }
    }
    else {
       _screen->DisplayText("NVS Calibration\nnot found",MESSAGE_GRACE_PERIOD);
    }

    // Write the sensor offsets to the BNO055 and bail out
    if (_calibrationDataLoaded) {
        _bno->setSensorOffsets(_calibrationData.data());
        _screen->DisplayText("Updating BNO055\ncalibration data\nfrom NVS", MESSAGE_GRACE_PERIOD);
        return;
    }

    uint8_t sys;
    uint8_t gyro;
    uint8_t accel;
    uint8_t mag;
    char textbuf[256];

    while (!_bno->isFullyCalibrated())
    {
        _bno->getCalibration(&sys,&gyro, &accel, &mag);
        sprintf(textbuf, "Calibration status:\nSystem: %d\nGyro: %d\nAccel: %d\nMag: %d", sys, gyro, accel, mag);
        _screen->DisplayText(textbuf);
    }    
    _screen->DisplayText("Calibration OK");
    delay(1500);

    uint8_t calibrationData[22];
    if (_bno->getSensorOffsets(calibrationData))
    {
        if (NVS.setBlob(CALIBRATION_BLOB_NAME, calibrationData, sizeof(calibrationData)))
        {
            _screen->DisplayText("Calibration data\nwritten to NVS\nOK", MESSAGE_GRACE_PERIOD);
        }
        else
        {
            _screen->DisplayText("FAILED\nWriting calibration data\nto NVS\nOK", MESSAGE_GRACE_PERIOD);
        }
    }
}


// // The default settings for the BNO055 at powerup are
// // as follows:
// // 
// // Sensor          Parameter           Value
// // ==============================================
// // Accelerometer   Power mode          Normal
// //                 Range               +/-4G
// //                 Bandwidth           62.5Hz
// //                 resolution          14 bits
// // ----------------------------------------------
// // Gyroscope       Power mode          Normal
// //                 Range               2000 deg/s
// //                 Bandwidth           32Hz
// //                 Resolution          16 bits
// // ----------------------------------------------
// // Magnetometer    Power mode          Forced
// //                 ODR                 20Hz
// //                 XY repetition       15
// //                 Z repetition        16
// //                 Resolution x/y/z    13/13/15

// #define BNO055_ACC_CONFIG_ADDR 0x08
// #define BNO055_GYRO_CONFIG_ADDR 0x0A

// void configureIMU(Adafruit_BNO055 & bno, Adafruit_BNO055::adafruit_bno055_opmode_t currentMode)
// {
//   Adafruit_BNO055::adafruit_bno055_opmode_t modeback = _mode;

// /* Switch to config mode (just in case since this is the default) */
//   bno.setMode(Adafruit_BNO055::OPERATION_MODE_CONFIG);
//   delay(25);

//   /* save selected page ID and switch to page 1 */
//   uint8_t savePageID = read8(Adafruit_BNO055::BNO055_PAGE_ID_ADDR);
//   write8(Adafruit_BNO055::BNO055_PAGE_ID_ADDR, 0x01);

//   // set accelerometer configuration to 
//   // - 2G range 
//   // - 1000Hz Bandwidth
//   // Normal operation mode
//   write8(BNO055_ACC_CONFIG_ADDR, 0b00011100);
//   delay(10);

//   // set gyro configuration to
//   // - 2000 dps
//   // - 523 Hz Bandwidth
//   // Normal operation mode
//   write8(BNO055_GYRO_CONFIG_ADDR, 0b00000000);
//   delay(10);


//   /* restore page ID */
//   write8(Adafruit_BNO055::BNO055_PAGE_ID_ADDR, savePageID);

//   Adafruit_BNO055::setMode(modeback);
//   delay(20);
// }