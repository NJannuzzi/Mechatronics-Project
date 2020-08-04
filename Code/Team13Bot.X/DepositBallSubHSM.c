/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "Team13BotHSM.h"
#include "DepositBallSubHSM.h"
#include "Team13Bot.h"
#include "EventChecker.h"
#include "ES_Events.h"
#include "serial.h"

#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    DriveFWD,
    DriveBWD,
    DriveFWDSlow,
    DriveBWDSlow,
    CorrectFWD,
    CorrectBWD,
    MoveServo,
    JiggleBWD,
    JiggleFWD,
    Done,
} DepositBallSubHSMState_t;

static const char *StateNames[] = {
	"DriveFWD",
	"DriveBWD",
	"DriveFWDSlow",
	"DriveBWDSlow",
	"CorrectFWD",
	"CorrectBWD",
	"MoveServo",
	"JiggleBWD",
	"JiggleFWD",
	"Done",
};



/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static DepositBallSubHSMState_t CurrentState = DriveFWD; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitDepositBallSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = DriveFWD;
    returnEvent = RunDepositBallSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunDepositBallSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    DepositBallSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case DriveFWD: // in the first state, replace this with correct names
            MoveForward(700);
            printf("FORWARD/r/n");
            switch (ThisEvent.EventType) {
                case RH_TAPE_ON:
                    nextState = DriveFWDSlow;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                    break;
                case ES_TIMEOUT:
                    nextState = DriveBWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                case FL_TAPE_ON:
                    nextState = DriveBWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                case FR_TAPE_ON:
                    nextState = DriveBWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case DriveBWD: // in the first state, replace this with correct names
             printf("BACKWARD/r/n");
            MoveBackward(700);
            switch (ThisEvent.EventType) {
                case LH_TAPE_ON:
                    nextState = DriveBWDSlow;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                    break;
                case ES_TIMEOUT:
                    nextState = DriveFWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                case RL_TAPE_ON:
                    nextState = DriveFWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                case RR_TAPE_ON:
                    nextState = DriveFWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case DriveFWDSlow: // in the first state, replace this with correct names
             printf("SLOWFWD/r/n");
            MoveForward(300);
            switch (ThisEvent.EventType) {
                case RH_TAPE_OFF:
                    nextState = CorrectBWD;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 200);
                    makeTransition = TRUE;
                    break;
                case ES_TIMEOUT:
                    nextState = DriveBWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case DriveBWDSlow: // in the first state, replace this with correct names
             printf("SLOWBWD/r/n");
            MoveBackward(300);
            switch (ThisEvent.EventType) {
                case LH_TAPE_OFF:
                    nextState = CorrectFWD;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 200);
                    makeTransition = TRUE;
                    break;
                case ES_TIMEOUT:
                    nextState = DriveFWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 5000);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case CorrectBWD:
            MoveBackward(300);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    nextState = MoveServo;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1000);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case CorrectFWD:
            MoveForward(300);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    nextState = MoveServo;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1000);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case MoveServo: // in the first state, replace this with correct names
            StopMotors();
            ReleaseBall();
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    nextState = JiggleBWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 500);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case JiggleBWD: // in the first state, replace this with correct names
            MoveBackward(300);
            ResetBall();
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    nextState = JiggleFWD;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 500);
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case JiggleFWD: // in the first state, replace this with correct names
            MoveForward(300);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    ES_Timer_InitTimer(TOP_TRANSITION_TIMER, 5);
                    nextState = Done;
                    makeTransition = TRUE;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunDepositBallSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunDepositBallSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

