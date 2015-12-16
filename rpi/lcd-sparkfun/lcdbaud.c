//--------------------------------------------------------
 // Module Name : sparkfun LCD Display utilies
 // Version : 1.0.0
 //
 //
 // Copyright (c) 2015 zorglub42
 // This software is distributed under the Apache 2 license
 // <http://www.apache.org/licenses/LICENSE-2.0.html>
 //
 //--------------------------------------------------------
 // File Name   : lcdbaud.c
 //
 // Created     : 2015-07
 // Authors     : Zorglub42 <contact(at)zorglub42.fr>
 //
 // Description :
 //     change LCD display baudrate
 //--------------------------------------------------------
#include <wiringSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int lcd;   // This is required, to start an instance of an LCD

void usage(char *progName){
	printf("Usage:\n\t%s current-baud-rate new-baud-rate\n", progName);
	printf("with:\n\tcurrent-baud-rate in: 300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200\n");
	printf("\tnew-baud-rate in: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n");
}


int main(int argc, char *argv[]){
	if (argc != 2 ) {
		usage(argv[0]);
		return 1;
	}
	lcd=serialOpen("/dev/ttyAMA0", atoi(argv[1]));
	serialPutchar(lcd, 129);
	serialPutchar(lcd, argv[2][0]-'0');
	serialClose(lcd);
	return 0;
}
