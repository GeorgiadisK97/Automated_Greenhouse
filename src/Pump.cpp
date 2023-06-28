#include "Pump.h"


Pump::Pump(uint8_t pin) : _pin{pin}
{
    _pump_state = false;
}

void Pump::open()
{
    Serial.println("Pump open");
    digitalWrite(_pin, HIGH);
    _pump_state = true;
}
void Pump::close()
{
    Serial.println("Pump close");
    digitalWrite(_pin, LOW);
    _pump_state = false;
}

void Pump::display() const
{
    lcd.setCursor(8, 0);
    if (_pump_state)
    {
        lcd.print("ON ");
    }
    else
    {
        lcd.print("OFF");
    }
}

String Pump::state(){
    return (_pump_state ? "ON " : "OFF");
}