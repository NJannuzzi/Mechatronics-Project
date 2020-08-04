/*
 * File: Team13BotHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel Elkaim and Soja-Marie Morgens
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
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
#include "BottomTapeSubHSM.h" //#include all sub state machines called
#include "OrbitCCWSubHSM.h"
#include "OrbitCWSubHSM.h"
#include "ParkFWDSubHSM.h"
#include "ParkBWDSubHSM.h"
#include "DepositBallSubHSM.h"
#include "FindBeaconSubHSM.h"
#include "Team13Bot.h"
#include "EventChecker.h"
#include "LED.h"

#include <stdio.h>
/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/


typedef enum {
    InitPState,
    TankTurnCW,
    TankTurnCCW,
    DriveFWD,
    TapeBackup,
    TapeTurn,
    TapeDrive,
    TowerBackup,
    //RightWheelFWD,
    //BackUp1_2,
    //LeftWheelFWD,
    AlignTowerCCW,
    AlignTowerCW,
    FUCK,
    //BottomTapeDetect,
    OrbitCCW,
    OrbitCW,
    ParkFWD,
    ParkBWD,
    DepositBall,
    FindNextBeacon,
} Team13BotHSMState_t;

static const char *StateNames[] = {
	"InitPState",
	"TankTurnCW",
	"TankTurnCCW",
	"DriveFWD",
	"TapeBackup",
	"TapeTurn",
	"TapeDrive",
	"TowerBackup",
	"AlignTowerCCW",
	"AlignTowerCW",
	"FUCK",
	"OrbitCCW",
	"OrbitCW",
	"ParkFWD",
	"ParkBWD",
	"DepositBall",
	"FindNextBeacon",
};


/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine
   Example: char RunAway(uint_8 seconds);*/
/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static Team13BotHSMState_t CurrentState = InitPState; // <- change enum name to match ENUM
static uint8_t MyPriority;
static int depositFlag = 0;
static int parkFlag = 0;
static int foundFlag = 0;
/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTeam13BotHSM(uint8_t Priority) {
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 2000);
    LED_Init();
    LED_AddBanks(LED_BANK1 | LED_BANK2 | LED_BANK3);
    LED_SetBank(LED_BANK1, 0xf);
    LED_SetBank(LED_BANK2, 0xf);
    LED_SetBank(LED_BANK3, 0xf);
    Team13Bot_Init();
    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostTeam13BotHSM(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateHSM(ES_Event ThisEvent)
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
ES_Event RunTeam13BotHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    Team13BotHSMState_t nextState; // <- change type to correct enum
    static int beaconFlag = 0;
    static int dirFlag = 0;
    static int foundFlag = 0;
    ES_Tattle(); // trace call stack
    static int sawOtherWire = 0;
    switch (CurrentState) {
        case InitPState: // If current state is initial Pseudo State
            if (ThisEvent.EventType == ES_TIMEOUT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state
                // Initialize all sub-state machines
                depositFlag = 0;
                parkFlag = 0;
                BeaconForward();
                ResetBall();
                LED_SetBank(LED_BANK1, 0x0);
                LED_SetBank(LED_BANK2, 0x0);
                LED_SetBank(LED_BANK3, 0x0);
                // now put the machine into the actual initial state
                nextState = TankTurnCW;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ;
            }
            break;

        case TankTurnCW: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does

            RightTankTurn(750);
            //StopMotors();
            //            ThisEvent = RunBottomTapeSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_NO_EVENT:
                    break;
                case BEACON_FOUND:
                    printf("next beacon!\r\n");
                    nextState = DriveFWD;
                    makeTransition = TRUE;
                    break;
                default:
                    break;
            }
            break;
        case TankTurnCCW: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does

            LeftTankTurn(750);
            //StopMotors();
            //            ThisEvent = RunBottomTapeSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_NO_EVENT:
                    break;
                case BEACON_FOUND:
                    printf("next beacon!\r\n");
                    nextState = DriveFWD;
                    makeTransition = TRUE;
                    break;

                default:
                    break;
            }
            break;
        case DriveFWD:
            MoveForward(950);
            switch (ThisEvent.EventType) {
                case ES_NO_EVENT:
                    break;
                case BEACON_LOST:
                    printf("next beacon!\r\n");
                    nextState = TankTurnCW;
                    makeTransition = TRUE;
                    break;
                case FL_TAPE_ON:
                    nextState = TapeBackup;
                    makeTransition = TRUE;
                    dirFlag = 0;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 250);
                    break;
                case FR_TAPE_ON:
                    nextState = TapeBackup;
                    makeTransition = TRUE;
                    dirFlag = 1;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 250);
                    break;
                case FR_BUMPER_DOWN:
                    nextState = TowerBackup;
                    makeTransition = TRUE;
                    dirFlag = 1;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 250);
                    break;
                case FL_BUMPER_DOWN:
                    nextState = TowerBackup;
                    makeTransition = TRUE;
                    dirFlag = 0;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 250);
                    break;
                default:
                    break;
            }
            break;
        case TapeBackup:
            MoveBackward(600);
            switch (ThisEvent.EventType) {
                default:
                    break;
                case ES_TIMEOUT:
                    nextState = TapeTurn;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1000);
                    break;
            }
            break;
        case TowerBackup:
            MoveBackward(600);
            //BeaconLeft();
            switch (ThisEvent.EventType) {
                default:
                    break;
                case ES_TIMEOUT:
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                    nextState = AlignTowerCCW;
                    makeTransition = TRUE;
                    /*if (dirFlag == 0) {
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1000);
                    } else {
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1700);
                    }*/

                    break;
            }
            break;
        case AlignTowerCCW:
            RightTankTurn(700);
            switch (ThisEvent.EventType) {
                default:
                    break;
                case ES_TIMEOUT:
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 3000);
                    nextState = OrbitCCW;
                    makeTransition = TRUE;
                    break;
            }
            break;
        case AlignTowerCW:
            LeftTankTurn(300);
            switch (ThisEvent.EventType) {
                default:
                    break;
                case ES_TIMEOUT:
                    nextState = OrbitCCW;
                    makeTransition = TRUE;
                    break;
            }
            break;
        case TapeTurn:
            if (dirFlag) {
                LeftTankTurn(750);
            } else {
                RightTankTurn(750);
            }
            switch (ThisEvent.EventType) {
                default:
                    break;
                case ES_TIMEOUT:
                    nextState = TapeDrive;
                    makeTransition = TRUE;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 3000);
                    break;
            }
            break;
        case TapeDrive:
            MoveForward(750);
            switch (ThisEvent.EventType) {
                default:
                    break;
                case ES_TIMEOUT:
                    if (dirFlag) {
                        nextState = TankTurnCW;
                    } else {
                        nextState = TankTurnCCW;
                    }
                    makeTransition = TRUE;
                    break;
                case FL_TAPE_ON:
                    nextState = TankTurnCW;
                    makeTransition = TRUE;
                    break;
                case FR_TAPE_ON:
                    nextState = TankTurnCCW;
                    makeTransition = TRUE;
                    break;
                case FR_BUMPER_DOWN:
                    nextState = TowerBackup;
                    makeTransition = TRUE;
                    dirFlag = 1;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 250);
                    break;
                case FL_BUMPER_DOWN:
                    nextState = TowerBackup;
                    makeTransition = TRUE;
                    dirFlag = 0;
                    ES_Timer_InitTimer(TOP_LEVEL_TIMER, 250);
                    break;
            }
            break;

        case OrbitCCW: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            ThisEvent = RunOrbitCCWSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case FL_TAPE_ON:
                    sawOtherWire = 0;
                    nextState = OrbitCW;
                    makeTransition = TRUE;
                    break;
                case FRONT_WIRE_LOW:
                    sawOtherWire = 0;
                    break;
                case FRONT_WIRE_HIGH:
                    sawOtherWire = 1;
                    break;
                case BACK_WIRE_HIGH:
                    if (sawOtherWire) {
                        sawOtherWire = 0;
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 3000);
                        nextState = ParkFWD;
                        makeTransition = TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;
        case OrbitCW: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            ThisEvent = RunOrbitCWSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case RL_TAPE_ON:
                    sawOtherWire = 0;
                    nextState = OrbitCCW;
                    makeTransition = TRUE;
                    break;
                case BACK_WIRE_LOW:
                    sawOtherWire = 0;
                    break;
                case BACK_WIRE_HIGH:
                    sawOtherWire = 1;
                    break;
                case FRONT_WIRE_HIGH:
                    if (sawOtherWire) {
                        sawOtherWire = 0;
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 3000);
                        nextState = ParkBWD;
                        makeTransition = TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;
        case FUCK:
            StopMotors();
            break;

        case ParkFWD: // in the first state, replace this with correct names
            printf("PARKING/r/n");
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            ThisEvent = RunParkFWDSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TOP_TRANSITION_TIMER) {
                        printf("LEAVING PARKING/r/n");
                        nextState = DepositBall;
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                        makeTransition = TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;
        case ParkBWD: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            ThisEvent = RunParkBWDSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TOP_TRANSITION_TIMER) {
                        nextState = DepositBall;
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                        makeTransition = TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;

        case DepositBall: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            ThisEvent = RunDepositBallSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TOP_TRANSITION_TIMER) {
                        printf("LEAVING DEPOSIT/r/n");
                        // printf("STUFF WORKED\r\n");
                        ES_Timer_InitTimer(TOP_LEVEL_TIMER, 1500);
                        nextState = FindNextBeacon;
                        makeTransition = TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;

        case FindNextBeacon: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            ThisEvent = RunFindBeaconSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TOP_TRANSITION_TIMER) {
                        nextState = DriveFWD;
                        makeTransition = TRUE;
                        parkFlag = 0;
                        dirFlag = 0;
                        InitParkFWDSubHSM();
                        InitParkBWDSubHSM();
                        InitOrbitCWSubHSM();
                        InitOrbitCCWSubHSM();
                        InitDepositBallSubHSM();
                        InitFindBeaconSubHSM();
                    }
                    break;
                default:
                    break;
            }
            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunTeam13BotHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunTeam13BotHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}

void setDepositFlag(void) {
    depositFlag = 1;
}

void parkBounceFlag(void) {
    parkFlag = 1;
}

void setFoundFlag(void) {
    foundFlag = 1;
}
/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
