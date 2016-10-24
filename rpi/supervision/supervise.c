/*
 * Module Name : Supervise
 * Version : 1.0.0
 *
 *
 * Copyright (c) 2016 zorglub42
 * This software is distributed under the Apache 2 license
 * <http://www.apache.org/licenses/LICENSE-2.0.html>
 *
***********************************************************
 * File Name   : supervise.c
 *
 * Created     : 2016-10
 * Authors     : Simon Bécot <johnny.macmuller(at)gmail.com>
 *
 * Description :
 *  This software is managing used to supervive GPIO pins of
 * a raspberry pi model B, 2 or 3, installed with an OSMC
 * distribution
 * It manages leds and push buttons (with a short and a long push).
 * It also manages a relay to enable software shutdown as described
 * here https://github.com/zorglub42/tools/tree/master/rpi/start-stop-button
 * The current file is managing two push button and two leds:
 * POWER_LED, STATUS_LED, BUTTON_PAUSE, BUTTON_ONOFF
 * Led status is independant of the button action, it is retreived
 * from the OSMC status.
 * A thread is managing pushbutton
 * A trhead is managing leds, a new thread is created for a blinking led
 *
 * The software is using libcurl and WinringPi https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 * To enable trace and tests compile with -D__DEBUG__
 *
 ***********************************************************
*/

#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <curl/curl.h>
#include <signal.h>

#include "supervise.h"

/********************************************************/
/*##### Functions for leds                              */
/********************************************************/

// Argument for the blinking led thread entry point
struct blinkArgs {
    int id;    // LED ID
    int number;// Number of blinks
	int timeOn;  // delay of blinkOn
	int timeOff; // delay of blinkOff
};

/**************************************************/
/* Build and allocate a struct blinkArgs          */
/* args											  */
/* int: id of led                                 */
/* int: number of blinks (-1 for infinite)        */
/* int: time of ON light                          */
/* int: time of OFF light                         */
/* returns  a struct (to be freed)                */
/**************************************************/
struct blinkArgs* getBlinkArgs(int id, int number, int timeOn, int timeOff)
{
	struct blinkArgs* aBlink = malloc(sizeof(struct blinkArgs));
	aBlink->id = id;
	aBlink->number = number;
	aBlink->timeOn = timeOn;
	aBlink->timeOff = timeOff;
	return aBlink;
}

/*************************************************/
/* Blink a led in a separate thread              */
/* arguments is a struct blinkArgs               */
/*************************************************/
void *blinkLedInThread(void *x)
{
  struct blinkArgs* args = (struct blinkArgs *) x;
#ifdef __DEBUG__
  printf("blinkLedInThread led %i for %i times\n", args->id, args->number);
#endif
  int finished = 0;
  int ledArg = args->id; 
  int ledIds = led[ledArg];
  int nbLoop = args->number;
  int theDelayOn = args->timeOn;
  int theDelayOff = args->timeOff;
  free(x);
  int countLoop = 0;
  
  while (!finished)
  {
#ifdef __DEBUG__
     printf("ON\n");
#endif
     digitalWrite (ledIds, HIGH);
     delay(theDelayOn);
#ifdef __DEBUG__
     printf("OFF \n");
#endif
     digitalWrite (ledIds, LOW);
     delay(theDelayOff);
     if (nbLoop > 0) countLoop ++;
     finished = ((countLoop >= nbLoop) && (nbLoop>-1));
  }
#ifdef __DEBUG__
  printf("finished %i\n", countLoop);
#endif
  // clear the thread as it is finished
  ledThread[ledArg] = 0;
  return NULL;
}

/******************************************************/
/* Blink a led for a finite number of times           */
/* arguments:                                         */
/* int ledID: the ID of the led to blink              */
/* int nbBlink: the number of blinks (-1 for infinite */
/******************************************************/

pthread_t doBlinkLed(int ledID, int nbBlink, int theDelayOn, int theDelayOff)
{
#ifdef __DEBUG__
	printf("doBlinkLed  %i : %i\n", ledID, nbBlink);
#endif
	// only do it if not already blinking
	pthread_t threadId = ledThread[ledID];
	if (0 == threadId)
	{
		struct blinkArgs* args = getBlinkArgs(ledID, nbBlink, theDelayOn, theDelayOff);
		pthread_create(&threadId, NULL, blinkLedInThread, (void *)args);
		// Save thread id
		ledThread[ledID] = threadId;
	}
	return threadId;
}

/*****************************************************/
/* blinks led with no interruption                   */
/*****************************************************/
pthread_t infiniteBlinkLed(int ledID, int theDelayOn, int theDelayOff)
{
   return doBlinkLed(ledID, -1, theDelayOn, theDelayOff);
}

/*****************************************************/
/* Interruption of a blinking Thread                 */
/*****************************************************/
void stopBlink(int ledID)
{   
    if (ledThread[ledID] != 0)
    {
        pthread_cancel(ledThread[ledID]);
		ledThread[ledID] = 0;
    }
}

/******************************************/
// Start a led
/******************************************/
void startLed(int ledID) {
    stopBlink(ledID);
    digitalWrite (led[ledID], HIGH);
}

/******************************************/
// Stop a led
/******************************************/
void stopLed(int ledID) {
    stopBlink(ledID);
    digitalWrite (led[ledID], LOW);
}

/**********************************************************/
/* ######## System management                             */
/**********************************************************/

/**********************************************************/
/* Stops the relay                                        */
/**********************************************************/
void stopRelay(){
     digitalWrite (RELAY_GPIO, LOW);
}

/**********************************************************/
/* Define the function to be called when (SIGTERM) signal */  
/* is sent to process (shutdown)                          */
/**********************************************************/
void signal_callback_handler(int signum)
{
#ifdef __DEBUG__
   printf("Caught signal %d\n",signum);
#endif
   if (signum == SIGTERM)
   {
	   stopRelay();
   }
   exit(0);
}
	
/**********************************************************/
/* Shutdown the system                                    */
/**********************************************************/
void systemShutdown()
{
#ifdef __DEBUG__
    printf("systemShutdown");
#endif
    doBlinkLed(POWER_LED, 4, 200, 200);
    while (ledThread[POWER_LED] != 0) ;
    startLed(POWER_LED);
#ifndef __DEBUG__
    system("init 0");
#else
	delay(3000);
    exit(0);
#endif
    return ;// NULL;
}

/********************************************************/
/*  Restart the system                                  */
/********************************************************/
void systemRestart()
{
#ifdef __DEBUG__
    printf("systemRestart");
#endif
    doBlinkLed(POWER_LED, 2, 200, 200);
    while (ledThread[POWER_LED] != 0) ;
    startLed(POWER_LED);
#ifndef __DEBUG__
    system("init 6");
#else
	delay(3000);
#endif
    return ;// NULL;
}

/**************************************************************/
/*###### HTTP Requests management                             */
/**************************************************************/

/* holder for curl fetch */
struct curl_fetch_st {
    char *payload;
    size_t size;
};

/**************************************************************/
/* callback for curl                                          */
/**************************************************************/
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) 
{
#ifdef __DEBUG__
    printf("curl_callback %i, %i\n", size, nmemb);
#endif
    size_t realsize = size * nmemb;                             /* calculate buffer size */
    struct curl_fetch_st *p = (struct curl_fetch_st *) userp;   /* cast pointer to fetch struct */

   /* expand buffer */
    p->payload = (char *) realloc(p->payload, p->size + realsize + 1);

    /* check buffer */
    if (p->payload == NULL) {
      /* this isn't good */
      fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
      /* free buffer */
      free(p->payload);
      /* return */
      return -1;
    }

    /* copy contents to buffer */
    memcpy(&(p->payload[p->size]), contents, realsize);
    /* set new buffer size */
    p->size += realsize;
    /* ensure null termination */
    p->payload[p->size] = 0;
    /* return size */
    return realsize;
}

/*********************************************************************************/
/* Posts a XBMC request to the current media player                              */
/* args                                                                          */
/* char* method: method to Player.method                                         */
/* char* params: can be null                                                     */
/* returns the JSON result of the POST.                                          */
/*********************************************************************************/
char* postXBMCRequest(char* method, char* params)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    static char buffer[128];
    memset(buffer, '\0', sizeof(buffer));

    struct curl_fetch_st curl_fetch;                        /* curl fetch struct */
    struct curl_fetch_st *cf = &curl_fetch; 
    cf->payload = (char *) calloc(1, sizeof(cf->payload));
    cf->size = 0;
   //CURLcode res;
   
	if (curl) {
		sprintf(buffer, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.%s\"", method);
		if (NULL != params)
		{
	                sprintf(buffer, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.%s\", \"params\": %s, \"id\": 99}", method, params);
		}
		else
		{
                     sprintf(buffer, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.%s\", \"id\": 99}", method);
		}
		// logs
#ifdef __DEBUG__
		printf("Send: %s\n", buffer);
#endif		
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, KODI_URL);
		/* set calback function */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);

		/* pass fetch struct pointer */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) cf);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(buffer));
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);

		/*res = */curl_easy_perform(curl);
		
		curl_easy_cleanup(curl);
		if (cf->payload == NULL) {
			/* error */
			fprintf(stderr, "ERROR: Failed to populate payload");
		}
		strcpy(buffer, cf->payload);
		/* free payload */
		free(cf->payload);
#ifdef __DEBUG__
        printf("Received %s\n", buffer);
#endif
	}
	return buffer;
}

/*************************************************************/
/* Utility: Get an int value in a string                     */
/* args:                                                     */
/* char* input json string to parse                          */
/* char* pattern string to find                              */
/* return integer value, or -1 if not found                  */
/*************************************************************/
int getIntValue(const char* input, const char* pattern)
{
#ifdef __DEBUG__
	printf("getIntValue %s %s\n", input, pattern);
#endif
	// max 2 digit integers
	char buffer[MAX_DIGIT + 1];
	char found = 0;
	// points to the string
	char* value = strstr(input, pattern);
	if (value != NULL)
	{
	// point after the pattern 
	value = strstr(value, ":");
#ifdef __DEBUG__
	printf("value %s\n", value);
#endif
	// skip the :
	while (!isdigit(value[0]))	
	{
		value++;
	}
	int nbchar = 0;
	while (!found)
	{
		buffer[nbchar] = value[nbchar];
		nbchar++;
		found = (!isdigit(value[nbchar]) || MAX_DIGIT == nbchar);
	}
	buffer[nbchar] = 0;
	return atoi(buffer);
	}
	return -1;
}

/******************************************/
// Play/Pause the Kodi current media
/******************************************/
void play_pause() {
  char* result = postXBMCRequest("GetActivePlayers", NULL);
  int pId = getIntValue(result, "playerid");
  char params[MAX_BUFFER];
  if (pId != -1)
  {
    sprintf(params, "{ \"playerid\": %i }", pId);
    postXBMCRequest("PlayPause", params);
  }
  return ;
}

/******************************************/
// Stops playing current media
/******************************************/
void stop_player() {
  char* result = postXBMCRequest("GetActivePlayers", NULL);
  int pId = getIntValue(result, "playerid");
  char params[MAX_BUFFER];
  sprintf(params, "{ \"playerid\": %i }", pId);
  postXBMCRequest("Stop", params);
  return ;
}

/******************************************/
/* TODO: Up, Down, Left, Right */
/******************************************/

/*************************************************************/
/* returns the playing status of current media player        */
/* arg: identifier of current player is updated              */
/* returns NOT_PLAYING, PLAYING or PAUSED                    */
/*************************************************************/
char getPlayingStatus(int* id)
{
#ifdef __DEBUG__
   printf("getPlayingStatus\n");
#endif
   char* result = postXBMCRequest("GetActivePlayers", NULL);
#ifdef __DEBUG__
   printf("Result %s\n", result);
#endif
   int pId = getIntValue(result, "playerid");
   *id = pId;
   if ((0 == pId) || (1 == pId))
   {
	  char params[MAX_BUFFER];
	  sprintf(params, "{ \"playerid\": %i, \"properties\" : [\"speed\"] }", pId);
	  postXBMCRequest("GetProperties", params);
	  int speed = getIntValue(result, "speed");
	  switch (speed)
	  {
		  case 0: speed = PAUSED; break;
		  case 1: speed = PLAYING; break;
		  default: speed = NOT_PLAYING; 
	  }
	  return speed;
   }
   return NOT_PLAYING;
}

// initialize the GPIO
void initGPIO()
{
   // Relay
   pinMode(RELAY_GPIO,OUTPUT);
   digitalWrite (RELAY_GPIO, HIGH);
   // Leds
   int i = 0;
   for (i=0; i < NBLED; i++)
   {
		pinMode(led[i],OUTPUT);
   }
   startLed(POWER_LED);
   // buttons
   for (i=0; i < NBBUTTON; i++)
   {
		pinMode(button[i],INPUT);
   }
}

/* global variables initialization */
void init()
{
#ifdef __DEBUG__
   printf("init\n");
#endif
   signal(SIGTERM, signal_callback_handler);
   // initialize arrays to 0
   memset(ledThread, 0, sizeof(ledThread));
   memset(actionPushButton, 0, sizeof(actionPushButton));
   memset(actionLongPushButton, 0, sizeof(actionPushButton));
   initGPIO();
   // initialize actions for buttons
   actionPushButton[BUTTON_PAUSE] = play_pause;
   actionLongPushButton[BUTTON_PAUSE] = stop_player;
   actionPushButton[BUTTON_ONOFF] = systemShutdown;
   actionLongPushButton[BUTTON_ONOFF] = systemRestart;
}

/*************************************************/
/* Loop for status checking                      */
/*************************************************/
void *checkStatus(void *x)
{
 #ifdef __DEBUG__
    printf("checkStatus");
#endif
  char playerStatus = NOT_PLAYING;
   for (;;)
   {
     int playerId = 0;
     char newStatus  = getPlayingStatus(&playerId);
     if (newStatus != playerStatus) 
     {
#ifdef __DEBUG__
       printf("Status %i", newStatus);
#endif
	   // reset led if we change status
	stopLed(STATUS_LED);
       switch (newStatus)
       {
          case PLAYING : infiniteBlinkLed(STATUS_LED, 1200, 1200); break;
	       case PAUSED  : infiniteBlinkLed(STATUS_LED, 500, 500); break;
          case NOT_PLAYING : 
          default: stopLed(STATUS_LED); break;
        }
        playerStatus = newStatus;
      }
	  // wait 1 second
	  delay(1500);
    }
 }


/*****************************************************/
/* main loop                                         */
/* initializes a separate thread for checking status */
/* and check for push buttons                        */
/*****************************************************/
void infiniteLoop()
{
  pthread_t threadId = 0;
  // manage status leds in separate process (every second)
  pthread_create(&threadId, NULL, checkStatus, (void *)NULL);
  // Manage push buttons in main thread

  for (;;)
  {
      char i;
      for (i = 0; i < NBBUTTON; i++)
      {
        int but = digitalRead(button[i]);
        int totalTime = 0;
        if (but == HIGH)
        {
            int alreadyLaunched = 0;
            while(digitalRead(button[i]) == HIGH)
            {
                delay (100) ;
                totalTime += 100;
                if (totalTime>=LONG_PUSH_TIME && !alreadyLaunched) // process long push even if still pushed
                {
                    alreadyLaunched = 1;
                    if (actionLongPushButton[i] != NULL)
                    {
                        (*actionLongPushButton[i])();
                    }
                }
            }
            // execute associated action.
            void (*method)() = (totalTime<LONG_PUSH_TIME)?(*actionPushButton[i]):NULL;
            if (method != NULL)
            {
                (*method)();
            }
        }
      }
      delay (200) ;
  }
}


#ifdef __DEBUG__
////////////////////////////////////////////////////
// Tests

// tests if leds are working properly
void test1()
{
  printf("test1\n");
  int i = 0;
  for (i=0; i < NBLED; i++)
  {
	  stopLed(i);
	  delay(2000);
	  startLed(i);
	  delay(2000);
	  doBlinkLed(i, 10, 100, 100);
	  while (ledThread[i] != 0) ;
	  doBlinkLed(i, 5, 200, 200);
	  while (ledThread[i] != 0) ;

	  doBlinkLed(i, 2, 200, 600);
          while (ledThread[i] != 0) ;
	  delay(2000);
	  startLed(i);
  }
}

// checks if libcurl is working properly and is properly configured
void test2()
{
  printf("test2\n");
  int playerId = 0;
  char newStatus  = getPlayingStatus(&playerId);
  printf("Status %c Id %i\n", newStatus, playerId);
}

// launch player status thread wait 30s
void test3()
{
  printf("test3\n");
  pthread_t threadId = 0;
  pthread_create(&threadId, NULL, checkStatus, (void *)NULL);
  delay(20000);
}

// Test functions times. The player should be on
void test4()
{
  printf("test4\n");
  int loop;
  for (loop = 0; loop < 5; loop++)
  {
      play_pause();
      delay(3000);
  }
  printf("test4.1 OK\n");

  actionPushButton[0]();
  printf("test4.2 OK\n");
  delay(1000);
  stop_player();
  delay(1000);
}

#endif


/*********************************************************************/
/* Main process of the application                                   */
/*********************************************************************/
int main (void)
{
  // initialize Wiring pi
  wiringPiSetup();
  // initialize application
  init();
  // launch supervition.
  infiniteLoop();
//  test1();
//  test2();
//  test3();
//  test4();
  return 0 ;
}
