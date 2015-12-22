##--------------------------------------------------------
 # Module Name : sparkfun LCD Display utilies
 # Version : 1.0.0
 #
 #
 # Copyright (c) 2015 zorglub42
 # This software is distributed under the Apache 2 license
 # <http://www.apache.org/licenses/LICENSE-2.0.html>
 #
 #--------------------------------------------------------
 # File Name   : makefile
 #
 # Created     : 2015-07
 # Authors     : Zorglub42 <contact(at)zorglub42.fr>
 #
 # Description :
 #     makefile to compile and install LCD utilies
 #--------------------------------------------------------
##
all: lcdclear lcdprint lcdbacklight lcdbaud

lcdprint: lcdprint.c
	gcc -Wall  -lwiringPi lcdprint.c -o lcdprint

lcdclear: lcdclear.c
	gcc -Wall -lwiringPi lcdclear.c -o lcdclear

lcdbacklight: lcdbacklight.c
	gcc -Wall  -lwiringPi lcdbacklight.c -o lcdbacklight

lcdbaud: lcdbacklight.c
	gcc -Wall  -lwiringPi lcdbaud.c -o lcdbaud


clean:
	([ -f lcdclear ] && rm lcdclear) || echo "lcdclear does not exist"
	([ -f lcdprint ] && rm lcdprint) || echo "lcdprint does not exist"
	([ -f lcdbacklight ] && rm lcdbacklight) || echo "lcdbacklight does not exist"
	([ -f lcdbaud ] && rm lcdbaud) || echo "lcdbaud does not exist"


uninstall: clean
	([ -f /usr/local/bin/lcdclear ] && rm /usr/local/bin/lcdclear) || echo "/usr/local/bin/lcdclear does not exist"
	([ -f /usr/local/bin/lcdprint ] && rm /usr/local/bin/lcdprint) || echo "/usr/local/bin/lcdprint does not exist"
	([ -f /usr/local/bin/lcdbacklight ] && rm /usr/local/bin/lcdbacklight) || echo "/usr/local/bin/lcdbacklight does not exist"
	([ -f /usr/local/bin/lcdbaud ] && rm /usr/local/bin/lcdbaud) || echo "/usr/local/bin/lcdbaud does not exist"


install: all
	cp lcdclear /usr/local/bin
	cp lcdprint /usr/local/bin
	cp lcdbacklight /usr/local/bin
	cp lcdbaud /usr/local/bin
