#ifndef _SERVO_MOTOR_H_
#define _SERVO_MOTOR_H_

#include "Motor_control.h"

class Servo_motor : public Motor{
    private:
        bool _servo_state;
        uint8_t _pin;
        int _hz;
        int _MIN_PULSE_WIDTH;
        int _MAX_PULSE_WIDTH;
        Servo servo;
    public:
        Servo_motor(uint8_t pin);
        Servo_motor(int hz, uint8_t pin, int min, int max);
        virtual ~Servo_motor() = default;
        virtual void open() override;
        virtual void close() override;
        virtual void display() const override;
        virtual String state() override;
        void attach();
};
#endif // _SERVO_MOTOR_H_