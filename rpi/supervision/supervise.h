/*
 * Module Name : Supervice
 * Version : 1.0.0
 *
 *
 * Copyright (c) 2016 zorglub42
 * This software is distributed under the Apache 2 license
 * <http://www.apache.org/licenses/LICENSE-2.0.html>
 *
***********************************************************
 * File Name   : supervise.h
 *
 * Created     : 2016-10
 * Authors     : Simon Bécot <johnny.macmuller(at)gmail.com>
 *
 * Description :
 *  This software is managing used to supervive GPIO pins of
 * a raspberry pi model B, 2 or 3, installed with an OSMC
 * distribution
 * supervise.h contains the constants
 *
 ***********************************************************
*/

#ifndef __SUPERVISE_H___
#define __SUPERVISE_H__
//////////////////////////////////////////////////////////////////////////////////////////////
// Constant definitions
///////////////////////////
// Specific configuration

///////////////////////////////////////
// LEDs

// Number of managed leds
#define NBLED 2
// Constant for POWER LED
#define POWER_LED 0
// Constant for PLAY LED
#define STATUS_LED 1

// LEDS GPIOs
// PIN FOR + OF POWER LED
#define POWER_LED_GPIO 0
// PIN FOR + OF PLAY LED
#define STATUS_LED_GPIO 6

// Array containing GPIO Ids of leds
int led[NBLED]={POWER_LED_GPIO,STATUS_LED_GPIO};
// Array containing threads ID (each blinking led in a separate thread
// WARNING: this array has to be initialized in init procedure.
pthread_t ledThread[NBLED];


/////////////////////////////////////////
// Push BUTTONS

// Number of managed buttons
#define NBBUTTON 2
// Constant for PLAY/PAUSE button
#define BUTTON_PAUSE 0
// Constant for ON/SHUTDOWN button
#define BUTTON_ONOFF 1

// Long push 2 seconds
#define LONG_PUSH_TIME 2000
// Repeat time (when there is no long push defined)
#define REPEAT_PUSH_TIME 500

// PIN FOR PAUSE GPIO
#define BUTTON_PAUSE_GPIO 3
// PIN FOR ON/SHUTDOWN
#define BUTTON_ON_GPIO 4

// Array containing GPIO Ids of push buttons
int button[NBBUTTON]={BUTTON_PAUSE_GPIO,BUTTON_ON_GPIO};
// Array containing actions that are triggered when a button is pushed
// WARNING: this array has to be initialized in init procedure.
void (*actionPushButton[NBBUTTON])();
// Array containing actions that are triggered when a button is pushed for more than 2 seconds
// WARNING: this array has to be initialized in init procedure.
void (*actionLongPushButton[NBBUTTON])();

// GPIO for the relay
// Relay for ON/OFF
#define RELAY_GPIO 5

// Local URL of the KODI server
#define KODI_URL "http://127.0.0.1:80/jsonrpc"

// Status constants
//PLAY 0=NOT PLAYING, 1=PLAYING, 2=PAUSED
#define NOT_PLAYING 0
#define PLAYING 1
#define PAUSED 2

// KODI Answer buffer size
#define MAX_BUFFER 60
// Max number of digit for Kodi returned values (used for player Id and Speed)
#define MAX_DIGIT 2
