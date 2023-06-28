#ifndef _DHT_L_H_
#define _DHT_L_H_

#include "Sensors.h"
#include <DHT.h>
#include <DHT_U.h>

class DHT_L : public Sensors
{
private:
    uint8_t _pin;
    uint8_t _DHT_TYPE;
    DHT_Unified _dht;
    sensor_t _sensor;
    sensors_event_t _event;

public:
    DHT_L(uint8_t pin, uint8_t DHT_TYPE);
    virtual int read() override;
    virtual bool check_data(int &DHT_THRESHOLD) override;
    void begin();
    virtual ~DHT_L() = default;
};

#endif // _DHT_L_H_