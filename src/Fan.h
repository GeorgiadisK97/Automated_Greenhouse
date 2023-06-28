#ifndef _FAN_H_
#define _FAN_H_
#include "Motor_control.h"

class Fan : public Motor{
    private:
        bool _fan_state;
        uint8_t _pin;
    public:
        Fan(uint8_t pin);
        virtual ~Fan() = default;
        virtual void open() override;
        virtual void close() override;
        virtual void display() const override;
        virtual String state() override;
};

#endif // _FAN_H_