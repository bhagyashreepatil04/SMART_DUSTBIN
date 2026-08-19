Smart Dustbin — Touchless Lid & Fill-Level Alert
A dual-sensor Arduino Uno system that opens the dustbin lid without contact and warns when it's nearly full — built to make waste disposal more hygienic and prevent overflow.
Problem Statement
People avoid touching dustbin lids due to hygiene concerns, while overflowing bins create unpleasant, unhygienic surroundings. A touchless, self-monitoring dustbin is needed to solve both issues.
Solution
A dual-sensor Arduino Uno system:
One HC-SR04 ultrasonic sensor detects a hand nearby and triggers an SG90 servo motor to open the lid without contact.
A second ultrasonic sensor inside the bin continuously tracks the fill level.
When the bin reaches 80% capacity, an LED and buzzer alert nearby people to empty it — enabling hygienic, contact-free waste disposal with real-time overflow prevention.
Tech Stack
Arduino Uno
Embedded C++
Arduino IDE
HC-SR04 Ultrasonic Sensor (×2)
SG90 Servo Motor
Buzzer & LED
How It Works
Lid control — the hand-detection ultrasonic sensor continuously measures distance. When an object (hand) is detected within a set threshold, the servo rotates to open the lid; after a short delay with no object detected, it closes again.
Fill-level monitoring — the second ultrasonic sensor, mounted facing down inside the bin, measures the distance to the trash surface. This is converted into a fill percentage based on the bin's known depth.
Overflow alert — once fill level crosses 80%, the LED lights up and the buzzer sounds to notify people nearby that the bin needs emptying.

Project Status
Prototype built and tested on breadboard with Arduino Uno.
