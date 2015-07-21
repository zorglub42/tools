##--------------------------------------------------------
 # Module Name : sparkfun LCD utilies
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
all: lcdclear lcdprint lcdbacklight

lcdprint: lcdprint.c
        gcc -Wall -o blink -lwiringPi lcdprint.c -o lcdprint

lcdclear: lcdclear.c
        gcc -Wall -o blink -lwiringPi lcdclear.c -o lcdclear

lcdbacklight: lcdbacklight.c
        gcc -Wall -o blink -lwiringPi lcdbacklight.c -o lcdbacklight


clean:
        [ -f lcdclear ] && rm lcdclear
        [ -f lcdprint ] && rm lcdprint
        [ -f lcdbacklight ] && rm lcdbacklight


uninstall: clean
        [ -f /usr/local/bin/lcdclear ] && rm /usr/local/bin/lcdclear
        [ -f /usr/local/bin/lcdprint ] && rm /usr/local/bin/lcdprint
        [ -f /usr/local/bin/lcdbacklight ] && rm /usr/local/bin/lcdbacklight


install: all
        cp lcdclear /usr/local/bin
        cp lcdprint /usr/local/bin
        cp lcdbacklight /usr/local/bin
