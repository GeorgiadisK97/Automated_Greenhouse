#include "HMD_Sensor.h"

HMD::HMD(uint8_t pin1, uint8_t pin2)
    : _pin1{pin1}, _pin2{pin2}
{
}

int HMD::read()
{
    digitalWrite(_pin2, HIGH);
    delay(10);
    int value;
    value = analogRead(_pin1);
    value = map(value, 0, 1023, 0, 100);
    digitalWrite(_pin2, LOW);
    return value;
}
