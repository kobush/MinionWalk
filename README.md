Minion Walk
===========

Code for Minion biped robot from http://www.thingiverse.com/thing:25470
Arduino translates RC remote signals to servo movements. 
Based on code published on thingiverse by Makerbot/Skimbal.

CHANGES
-------

1. Replaced pulseIn with interrupt based RC decoding based on: http://rcarduino.blogspot.co.uk/2012/04/how-to-read-multiple-rc-channels-draft.html
2. Calibration mode to set transmitter throttle/stearing range 

TODO
----

1. Add proportional throttle 
2. Set throttle to zero when no signal from transmitter is received 
3. Wrap RC code in class
4. Add buzzer an play Imperial March when it walks
5. Allow to calibrate servo positions