#include "Fan.h"

Fan::Fan(uint8_t pin) : _pin{pin}
{
    _fan_state = false;
}

void Fan::open()
{
    Serial.println("Fan open");
    digitalWrite(_pin, HIGH);
    _fan_state = true;
}
void Fan::close()
{
    Serial.println("Fan close");
    digitalWrite(_pin, LOW);
    _fan_state = false;
}

void Fan::display() const
{
    lcd.setCursor(2, 0);
    if (_fan_state)
    {
        lcd.print("ON");
    }
    else
    {
        lcd.print("OFF");
    }
}

String Fan::state(){
    return (_fan_state ? "ON " : "OFF");
}