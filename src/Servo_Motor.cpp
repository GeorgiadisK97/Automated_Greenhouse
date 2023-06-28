#include "Servo_Motor.h"

Servo_motor::Servo_motor(int hz, uint8_t pin, int min, int max) : _hz{hz}, _pin{pin}, _MIN_PULSE_WIDTH{min}, _MAX_PULSE_WIDTH{max}
{
    _servo_state = false;
}

void Servo_motor::open()
{
    Serial.println("Temperature above threshold.Opening window");
    for (int i{0}; i < 180; i++)
    {
        servo.write(i);
        delay(30);
    }
    _servo_state = true;
}
void Servo_motor::close()
{
    Serial.println("Temperature below threshold.Closing window");
    for (int i{180}; i > 0; i--)
    {
        servo.write(i);
        delay(30);
    }
    _servo_state = false;
}

void Servo_motor::attach()
{
    servo.setPeriodHertz(_hz);
    servo.attach(_pin, _MIN_PULSE_WIDTH, _MAX_PULSE_WIDTH);
}

void Servo_motor::display() const
{
    (_servo_state ? lcd.print("ON ") : lcd.print("OFF"));
}

String Servo_motor::state()
{
    return (_servo_state ? "ON " : "OFF");
}