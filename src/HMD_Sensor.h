#ifndef _HMD_SENSOR_H_
#define _HMD_SENSOR_H_

#include "Sensors.h"

class HMD : public Sensors
{
private:
    uint8_t _pin1; // Data from sensor
    uint8_t _pin2; // 5V Supply to sensor
public:
    HMD(uint8_t pin1, uint8_t pin2);
    virtual int read() override;
    virtual ~HMD() = default;
};

#endif // _HMD_SENSOR_H_