Minion Walk
===========

Code for Minion biped robot from http://www.thingiverse.com/thing:25470
Arduino translates RC remote signals to servo movements. 
Based on code published on thingiverse by Makerbot/Skimbal.

Requires Arduino Button library from http://arduino.cc/playground/Code/Button

CHANGES
-------

1. Replaced pulseIn with interrupt based RC decoding based on: http://rcarduino.blogspot.co.uk/2012/04/how-to-read-multiple-rc-channels-draft.html
2. Calibration mode to set transmitter throttle/stearing range 
3. Proportional throttle control (impacts delay in main loop)
4. Automatically go to rest if no RC signal received in 100ms. 

TODO
----

1. Reimplement proportional throttle to impact move increments
2. Wrap RC code in class
3. Add buzzer an play Imperial March when it walks
4. Allow to calibrate servo positions