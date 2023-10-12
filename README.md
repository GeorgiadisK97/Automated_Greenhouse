# Automated Greenhouse

Controlling and monitoring a self-sustainable greenhouse.

## Materials used

1x DHT11 sensor for measuring the inside temperature.  
1x Soil moisture sensor.  
1x 12V DC 1.96W fan.  
1x 12V DC water pump.  
1x 5V servo.  
1x Dual 5V relay (SRD-05VDC-SL-C).  
1x I2C LCD.   
1x 12V 12.5A external power supply (LRS-150-12).  
1x 5V 250mA external power supply.  
1x esp32-c3-devkitC-02.

## Explanation

The current greenhouse consists of a couple of sensors and actuators.
Τhe sensors read the data of the greenhouse environment and send it to the microprocessor for processing.
When processing takes place the actuators are activated or deactivated according to the logic of the program.
Once the actuators execute their respective commands, the microprocessor sends the data to a web server, which runs from the microprocessor itself.
From there, not only we can see the state of the greenhouse, but also we can interact with it by changing the thresholds and sending them back to the ESP.
For example, if the temperature of the greenhouse is above the threshold, then the window opens with the help
of a servo motor. Plus, a DC fan starts to spin until the inside temperature meets the desired limit.
If the temperature is too high or too low, we can connect to the IP shown to us by the LCD screen, where the 
website is hosted and change the limits. After we change them, we hit send and the esp will respond accordingly.


## Future development

More sensors and actuators to be added. 
Solar power.
More interactive website.







