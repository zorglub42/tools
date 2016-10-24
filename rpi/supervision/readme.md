# Supervision of raspberry pi mediacenter

This is an alternative and enhancement to https://github.com/sbecot/tools/tree/master/rpi/start-stop-button 
Not only supervision manages the start/stop button, but it also manages a play/pause button for KODI, and two status leds (power and play).
The supervision is written in C which improves the performance, as WiringPi and Curl are used in library and not independant processes. The script version is about 3% CPU on a Rpi B, and the C version about 0.3% .

The current file is managing two push button and two leds:
 * POWER_LED, STATUS_LED, 
 * BUTTON_PAUSE, BUTTON_ONOFF

Led status is independant of the button action, it is retreived from the OSMC status by polling (curl).
A thread is managing pushbutton
A trhead is managing leds, a new thread is created for a blinking led


The software is using libcurl and WinringPi https://projects.drogon.net/raspberry-pi/wiringpi/


To compile, install WiringPi and libcurl use the command

    gcc -Wall -o supervise supervise.c -lwiringPi -pthread -lcurl

To enable trace and tests compile with -D__DEBUG__



