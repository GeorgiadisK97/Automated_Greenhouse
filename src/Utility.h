#ifndef _UTILIY_H_
#define _UTILITY_H_

#include "LiquidCrystal_I2C.h"
#include "Sensors.h"
#include "DHT_L.h"
#include "HMD_Sensor.h"
#include "Fan.h"
#include "Pump.h"
#include "Servo_Motor.h"

// LCD I2C init
#define ROWS 2
#define COLS 16
LiquidCrystal_I2C lcd{0x27, COLS, ROWS};

// DHT Sensor pin - temperature threshold init
#define DHTTYPE DHT11
#define DHT_DATA_PIN 6
extern int DHT_THRESHOLD;

// Soil Moisture Sensor init
#define HMD_DATA_PIN 3
#define HMD_POWER_PIN 7
extern int HMD_THRESHOLD;

// Fan pin
#define FAN_RELAY_PIN 4

// Pump pin
#define PUMP_RELAY_PIN 5

// Servo pin
#define _HZ 50
#define _SERVO_PIN 19
#define _MIN_PULSE_WIDTH 500
#define _MAX_PULSE_WIDTH 2400

extern int DHT_THRESHOLD;
extern int HMD_THRESHOLD;
extern int previous_temp_data;
extern int previous_hmd_data;
extern int current_temp_data;
extern int current_hmd_data;

extern bool temp_flag;
extern bool hmd_flag;

extern LiquidCrystal_I2C lcd;

extern Servo_motor window;
extern Pump pump;
extern Fan fan;

extern HMD hmd;
extern DHT_L dht;

byte droplet[] = {
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B11111,
    B11111,
    B01110};

byte thermometer[] = {
    B00100,
    B01010,
    B01010,
    B01010,
    B01010,
    B11111,
    B11111,
    B01110};

void print_UI()
{
    lcd.setCursor(0, 0);
    lcd.print("F:" + fan.state());
    lcd.setCursor(6, 0);
    lcd.print("P:" + pump.state());
    lcd.setCursor(0, 1);
    lcd.print("W:" + window.state());
}

void init_flags()
{
    if (dht.read() > DHT_THRESHOLD)
        temp_flag = true;
    else
        temp_flag = false;
    if (hmd.read() > HMD_THRESHOLD)
        hmd_flag = true;
    else
        hmd_flag = false;
}

void display_moisture(int &moisture)
{
    if (moisture > 10)
    {
        lcd.setCursor(12, 0);
        lcd.print(moisture);
        lcd.print("%");
        lcd.write(0);
    }
    else
    {
        lcd.setCursor(12, 0);
        lcd.print(" ");
        lcd.print(moisture);
        lcd.print("%");
        lcd.write(0);
    }
}

void display_temperature(int &temperature)
{
    lcd.setCursor(11, 1);
    lcd.print(temperature, 1);
    lcd.print(char(223));
    lcd.print("C");
    lcd.write(1);
}

#endif // _UTILITY_H_