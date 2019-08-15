# Closed-loop-speed-control
An IR sensor module is placed in front of the the shaft of a DC motor. Based on the signal from the IR sensor, the current RPM of the shaft is calculated.
The reference rpm, defined as the desired speed at which the shaft needs to rotate, is obtained via the serial monitor, entered by the user. Based on the reference RPM and the current RPM of the motor a closed loop speed control system is designed.

The output variable of the control system is the 8-bit PWM value, provided by the Arduino. This signal is used to control the Base-emitter voltage of an NPN transistor, which operates as a switch. 

The Motor is connected to the collector of the transistor and the emitter is grounded.



