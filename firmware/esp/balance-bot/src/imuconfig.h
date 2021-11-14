#ifndef _IMUCONFIG_H_
#define _IMUCONFIG_H_

#include <Adafruit_BNO055.h>
#include <vector>
#include "display.h"

const int CALIBRATION_DATA_SIZE = 22;

class IMUConfig 
{
    public:
        IMUConfig(Adafruit_BNO055 * bno, Display * screen);
        void Begin();
        void Calibrate();

    private:
        std::vector<uint8_t> _calibrationData;
        bool _calibrationDataLoaded;
        Adafruit_BNO055 * _bno;
        Display * _screen;
};




#endif // _IMUCONFIG_H_