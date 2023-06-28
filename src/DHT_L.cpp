#include "DHT_L.h"

DHT_L::DHT_L(uint8_t pin, uint8_t DHT_TYPE)
    : _pin{pin}, _DHT_TYPE{DHT_TYPE}, _dht(pin, DHT_TYPE)
{
}

void DHT_L::begin()
{
    Serial.println("DHT Sensor initialized");
    _dht.begin();
}

int DHT_L::read()
{
    _dht.temperature().getEvent(&_event);
    return _event.temperature;
}

bool DHT_L::check_data(int &DHT_THRESHOLD)
{
    int value;
    value = read();
    return (value > DHT_THRESHOLD ? true : false);
}
