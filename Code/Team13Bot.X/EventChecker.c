/*
 * File:   TemplateEventChecker.c
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with both FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This file includes a test harness that will run the event detectors listed in the
 * ES_Configure file in the project, and will conditionally compile main if the macro
 * EVENTCHECKER_TEST is defined (either in the project or in the file). This will allow
 * you to check you event detectors in their own project, and then leave them untouched
 * for your project unless you need to alter their post functions.
 *
 * Created on September 27, 2013, 8:37 AM
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "EventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"
#include "Team13Bot.h"
#include <stdio.h>
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define BACK_LOWER_BOUND  400
#define BACK_UPPER_BOUND 800
#define FRONT_LOWER_BOUND 300
#define FRONT_UPPER_BOUND 800
#define FRONT_UPPER_CLOSE 900
#define FRONT_LOWER_CLOSE 800
#define BACK_UPPER_CLOSE 900
#define BACK_LOWER_CLOSE 800
#define TRACK_WIRE_CONSENSUS 10
#define HI 1
#define LO 0
#define BUMPER_CONSENSUS_THRESHOLD 200
#define BEACON_CONSENSUS_THRESHOLD 100
#define TAPE_HIGH_CONSENSUS_THRESHOLD 100
#define TAPE_LOW_CONSENSUS_THRESHOLD 50
/*******************************************************************************
 * EVENTCHECKER_TEST SPECIFIC CODE                                                             *
 ******************************************************************************/

//#define EVENTCHECKER_TEST
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
#define SaveEvent(x) do {eventName=__func__; storedEvent=x;} while (0)

static const char *eventName;
static ES_Event storedEvent;
#endif

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this EventChecker. They should be functions
   relevant to the behavior of this particular event checker */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

/* Any private module level variable that you might need for keeping track of
   events would be placed here. Private variables should be STATIC so that they
   are limited in scope to this module. */

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function TemplateCheckBattery(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is a prototype event checker that checks the battery voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the actual battery voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either BATTERY_CONNECTED or BATTERY_DISCONNECTED if the power switch is turned
 *        on or off with the USB cord plugged into the Uno32. Returns TRUE if there was an 
 *        event, FALSE otherwise.
 * @note Use this code as a template for your other event checkers, and modify as necessary.
 * @author Gabriel H Elkaim, 2013.09.27 09:18
 * @modified Gabriel H Elkaim/Max Dunne, 2016.09.12 20:08 */
uint8_t CheckBackTrackWire(void) {
    static ES_EventTyp_t lastEvent = BACK_WIRE_LOW;
    static int consensus = 0;
    ES_EventTyp_t curEvent = lastEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t trackReading = ReadBackTrackWire(); // read the battery voltage
    //printf("Front reading: %u\r\n", trackReading);
    if (trackReading > BACK_UPPER_BOUND) { // is battery connected?
        curEvent = BACK_WIRE_HIGH;
    } else if (trackReading < BACK_LOWER_BOUND) {
        curEvent = BACK_WIRE_LOW;
    }
    if (curEvent != lastEvent) { // check for change from last time
        consensus++;
        if (consensus > TRACK_WIRE_CONSENSUS) {
            consensus = 0;
            thisEvent.EventType = curEvent;
            thisEvent.EventParam = trackReading;
            returnVal = TRUE;
            lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif   
        }
    } else {
        consensus = 0;
    }
    return (returnVal);
}

uint8_t CheckFrontTrackWire(void) {
    static ES_EventTyp_t lastEvent = FRONT_WIRE_LOW;
    static int consensus = 0;
    ES_EventTyp_t curEvent = lastEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t trackReading = ReadFrontTrackWire(); // read the battery voltage
    //printf("Back reading: %u\r\n",trackReading);
    if (trackReading > FRONT_UPPER_BOUND) { // is battery connected?
        curEvent = FRONT_WIRE_HIGH;
    } else if (trackReading < FRONT_LOWER_BOUND) {
        curEvent = FRONT_WIRE_LOW;
    }
    if (curEvent != lastEvent) { // check for change from last time
        consensus++;
        if (consensus > TRACK_WIRE_CONSENSUS) {
            thisEvent.EventType = curEvent;
            thisEvent.EventParam = trackReading;
            returnVal = TRUE;
            lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif   
        }
    } else {
        consensus = 0;
    }
    return (returnVal);
}

uint8_t CheckCloseBackTrackWire(void) {
    static ES_EventTyp_t lastEvent = CLOSE_BACK_WIRE_LOW;
    static int consensus = 0;
    ES_EventTyp_t curEvent = lastEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t trackReading = ReadBackTrackWire(); // read the battery voltage
    //printf("Front reading: %u\r\n", trackReading);
    if (trackReading > BACK_UPPER_CLOSE) { // is battery connected?
        curEvent = CLOSE_BACK_WIRE_HIGH;
    } else if (trackReading < BACK_LOWER_CLOSE) {
        curEvent = CLOSE_BACK_WIRE_LOW;
    }
    if (curEvent != lastEvent) { // check for change from last time
        consensus++;
        if (consensus > TRACK_WIRE_CONSENSUS) {
            consensus = 0;
            thisEvent.EventType = curEvent;
            thisEvent.EventParam = trackReading;
            returnVal = TRUE;
            lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif   
        }
    } else {
        consensus = 0;
    }
    return (returnVal);
}

uint8_t CheckCloseFrontTrackWire(void) {
    static ES_EventTyp_t lastEvent = CLOSE_FRONT_WIRE_LOW;
    static int consensus = 0;
    ES_EventTyp_t curEvent = lastEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t trackReading = ReadFrontTrackWire(); // read the battery voltage
    if (trackReading > FRONT_UPPER_CLOSE) { // is battery connected?
        curEvent = CLOSE_FRONT_WIRE_HIGH;
    } else if (trackReading < FRONT_LOWER_CLOSE) {
        curEvent = CLOSE_FRONT_WIRE_LOW;
    }
    if (curEvent != lastEvent) { // check for change from last time
        consensus++;
        if (consensus > TRACK_WIRE_CONSENSUS) {
            thisEvent.EventType = curEvent;
            thisEvent.EventParam = trackReading;
            returnVal = TRUE;
            lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif   
        }
    } else {
        consensus = 0;
    }
    return (returnVal);
}


uint8_t CheckBumpers(void) {
    static char lastStates[] = {LO, LO, LO, LO}; //Front Left, Front Right, Rear Left, Rear Right
    static int consensus[] = {0, 0, 0, 0};
    char curStates[] = {ReadFrontLeftBumper(), ReadFrontRightBumper(), ReadBackLeftBumper(), ReadBackRightBumper()};
    ES_EventTyp_t upEvents[] = {FL_BUMPER_UP, FR_BUMPER_UP, RL_BUMPER_UP, RR_BUMPER_UP};
    ES_EventTyp_t downEvents[] = {FL_BUMPER_DOWN, FR_BUMPER_DOWN, RL_BUMPER_DOWN, RR_BUMPER_DOWN};

    uint8_t returnVal = FALSE;

    int i;
    for (i = 0; i < 4; i++) {
        if (curStates[i] != lastStates[i]) {
            consensus[i]++;
        }
        if (consensus[i] > BUMPER_CONSENSUS_THRESHOLD) {
            lastStates[i] = curStates[i];
            consensus[i] = 0;
            ES_Event thisEvent;
            if (curStates[i]) {
                thisEvent.EventType = downEvents[i];
            } else {
                thisEvent.EventType = upEvents[i];
            }
            returnVal = TRUE;
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif  
        }
    }
    return (returnVal);
}

uint8_t CheckTapeSensors(void) {
    static char lastStates[] = {LO, LO, LO, LO}; //Front Left, Front Right, Rear Left, Rear Right
    static int consensus[] = {0, 0, 0, 0};
    char curStates[] = {ReadFrontLeftTapeSensor(), ReadFrontRightTapeSensor(), ReadBackLeftTapeSensor(), ReadBackRightTapeSensor()};
    ES_EventTyp_t offEvents[] = {FL_TAPE_OFF, FR_TAPE_OFF, RL_TAPE_OFF, RR_TAPE_OFF};
    ES_EventTyp_t onEvents[] = {FL_TAPE_ON, FR_TAPE_ON, RL_TAPE_ON, RR_TAPE_ON};

    uint8_t returnVal = FALSE;

    int i;
    for (i = 0; i < 4; i++) {
        if (curStates[i] != lastStates[i]) {
            consensus[i]++;
        } else {
            consensus[i] = 0;
        }
        if ((curStates[i] == HI && consensus[i] > TAPE_HIGH_CONSENSUS_THRESHOLD) || (curStates[i] == LO && consensus[i] > TAPE_LOW_CONSENSUS_THRESHOLD)) {
            lastStates[i] = curStates[i];
            consensus[i] = 0;
            ES_Event thisEvent;
            if (curStates[i]) {
                thisEvent.EventType = onEvents[i];
            } else {
                thisEvent.EventType = offEvents[i];
            }
            returnVal = TRUE;
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif  
        }
    }
    return (returnVal);
}

uint8_t CheckSideTapeSensors(void) {
    static char lastStates[] = {LO, LO}; //Hole Left, Hole Right
    static int consensus[] = {0, 0};
    char curStates[] = {ReadLeftHoleTapeSensor(), ReadRightHoleTapeSensor()};
    ES_EventTyp_t offEvents[] = {LH_TAPE_OFF, RH_TAPE_OFF};
    ES_EventTyp_t onEvents[] = {LH_TAPE_ON, RH_TAPE_ON};

    uint8_t returnVal = FALSE;

    int i;
    for (i = 0; i < 2; i++) {
        if (curStates[i] != lastStates[i]) {
            consensus[i]++;
        } else {
            consensus[i] = 0;
        }
        if ((curStates[i] == HI && consensus[i] > 5) || (curStates[i] == LO && consensus[i] > 5)) {
            lastStates[i] = curStates[i];
            consensus[i] = 0;
            ES_Event thisEvent;
            if (curStates[i]) {
                thisEvent.EventType = onEvents[i];
            } else {
                thisEvent.EventType = offEvents[i];
            }
            returnVal = TRUE;
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
            PostTeam13BotHSM(thisEvent);
#else
            SaveEvent(thisEvent);
#endif  
        }
    }
    return (returnVal);
}

uint8_t CheckBeacon(void) {
    static int time = 0;
    static ES_EventTyp_t lastEvent = BEACON_LOST;
    ES_EventTyp_t curEvent = lastEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    char beaconReading = BeaconDetected();
    if (lastEvent == BEACON_LOST) {
        if (beaconReading) {
            time++;
            if (time > BEACON_CONSENSUS_THRESHOLD) {
                curEvent = BEACON_FOUND;
                time = 0;
            }
        } else {
            time = 0;
        }
    } else if (lastEvent == BEACON_FOUND) {
        if (!beaconReading) {
            time++;
            if (time > BEACON_CONSENSUS_THRESHOLD) {
                curEvent = BEACON_LOST;
                time = 0;
            }
        } else {
            time = 0;
        }
    }
    if (curEvent != lastEvent) { // check for change from last time
        thisEvent.EventType = curEvent;
        returnVal = TRUE;
        lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        PostTeam13BotHSM(thisEvent);
#else
        SaveEvent(thisEvent);
#endif   
    }
    return (returnVal);
}

/* 
 * The Test Harness for the event checkers is conditionally compiled using
 * the EVENTCHECKER_TEST macro (defined either in the file or at the project level).
 * No other main() can exist within the project.
 * 
 * It requires a valid ES_Configure.h file in the project with the correct events in 
 * the enum, and the correct list of event checkers in the EVENT_CHECK_LIST.
 * 
 * The test harness will run each of your event detectors identically to the way the
 * ES_Framework will call them, and if an event is detected it will print out the function
 * name, event, and event parameter. Use this to test your event checking code and
 * ensure that it is fully functional.
 * 
 * If you are locking up the output, most likely you are generating too many events.
 * Remember that events are detectable changes, not a state in itself.
 * 
 * Once you have fully tested your event checking code, you can leave it in its own
 * project and point to it from your other projects. If the EVENTCHECKER_TEST marco is
 * defined in the project, no changes are necessary for your event checkers to work
 * with your other projects.
 */
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
static uint8_t(*EventList[])(void) = {EVENT_CHECK_LIST};

void PrintEvent(void);

void main(void) {
    BOARD_Init();
    Team13Bot_Init();
    ResetBall();
    //    ReleaseBall();
    /* user initialization code goes here */

    // Do not alter anything below this line
    int i;

    printf("\r\nEvent checking test harness for %s", __FILE__);

    while (1) {
        if (IsTransmitEmpty()) {
            for (i = 0; i< sizeof (EventList) >> 2; i++) {
                if (EventList[i]() == TRUE) {
                    PrintEvent();
                    break;
                }

            }
        }
    }
}

void PrintEvent(void) {
    printf("\r\nFunc: %s\tEvent: %s\tParam: 0x%X", eventName,
            EventNames[storedEvent.EventType], storedEvent.EventParam);
}
#endif