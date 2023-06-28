#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

extern int lcdColumns;
extern int lcdRows;
extern LiquidCrystal_I2C lcd;


class Motor{
    friend void LCD_display(const Motor &obj);
    public:
        virtual void open() = 0;
        virtual void close() = 0;
        virtual void display() const = 0;
        virtual String state() = 0;
        virtual ~Motor() = default;
};
