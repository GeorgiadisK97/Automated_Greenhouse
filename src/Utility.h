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

void display_moisture(int &moisture)
{
    lcd.setCursor(0, 1);
    if (HMD_THRESHOLD <= 9)
    {
        lcd.print(" " + String(HMD_THRESHOLD) + ":");
    }
    else
    {
        lcd.print(String(HMD_THRESHOLD) + ":");
    }
    if (moisture >= 10)
    {
        lcd.setCursor(3, 1);
        lcd.print(moisture);
        lcd.print("%");
        lcd.write(0);
    }
    else
    {
        lcd.setCursor(3, 1);
        lcd.print(moisture);
        lcd.print("%");
        lcd.write(0);
        lcd.print("  ");
    }
}

void display_temperature(int &temperature)
{
    lcd.setCursor(8, 1);
    lcd.print(String(DHT_THRESHOLD) + ":" + String(temperature));
    lcd.print(char(223));
    lcd.print("C");
    lcd.write(1);
}

#endif // _UTILITY_H_