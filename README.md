🛠 Basic-Prototype-of-SMART-HVAC-unit-using-Arduino

📌 Overview
This project implements an automated HVAC control system using Arduino UNO. It supports both AUTO and MANUAL modes with PWM-based fan speed control.

🔧 Components Used
  1.Arduino UNO
  2.DHT11 Temperature Sensor
  3.16x2 I2C LCD
  4.Keypad  4x4 
  5.PWM Motor with TIP122 Transistor and a Diode
  6.Red & Blue LEDs

⚙ Features
  1.Automatic temperature control
  2.Manual heat/cool selection
  3.PWM fan speed control
  4.User temperature setting via keypad
  5.LCD status display

🧠 Technologies Used
Embedded C
I2C Communication
PWM Control
Basic Control Logic

📷 Working Principle
System reads cabin temperature from DHT11 sensor and compares it with set temperature. Based on error value, cooling/heating is activated with proportional fan speed.
