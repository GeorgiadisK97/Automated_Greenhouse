#pragma once

#include "Adafruit_Sensor.h"

class Sensors
{
public:
    virtual int read() = 0;
    virtual ~Sensors() = default;
};
