#ifndef _PUMP_H_
#define _PUMP_H_

#include "Motor_control.h"

class Pump : public Motor{
    private:
        bool _pump_state;
        uint8_t _pin;
    public:
        Pump(uint8_t pin); 
        virtual ~Pump() = default;
        virtual void open() override;
        virtual void close() override;
        virtual void display() const override;
        virtual String state() override;
};

#endif // _PUMP_H_