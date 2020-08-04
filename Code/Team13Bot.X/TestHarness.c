/*
 * File:   TestHarness.c
 * Author: kalyoung
 *
 * Created on November 14, 2019, 4:56 PM
 */

#include "BOARD.h"
#include <xc.h>
#include <stdio.h>
#include "Team13Bot.h"

#include "pwm.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "Servo.h"

#define TRUE 1
#define FALSE 0
#define PRESSED 1
#define NOTPRESSED 0
#define ON 1
#define OFF 0
#define FORWARD 1
#define BACKWARD 0

int main(void) {
    BOARD_Init();
    Team13Bot_Init();
    printf("starting");
    //    ReleaseBall();
    int delta = 0;
    int increase = 1;
    ResetBall();
    BeaconForward();

    //Motors tests
    //    SetLeftMotorSpeed(500);
    //    SetLeftMotorDirection(FORWARD);
    //    SetRightMotorSpeed(750);
    //    SetRightMotorDirection(BACKWARD);

    //Motor helper function tests
    //    MoveFoward(750);
    //    MoveBackward(750);
    //    LeftTankTurn(750);
    //    RightTankTurn(750);
    //    LeftPivotTurn(750, BACKWARD);
    //    RightPivotTurn(750, FORWARD);
    //    GradualTurn(300, 600, FORWARD);
    //    SharpTurn(600, 300);
    
    
    while (1) {
        ++delta;
        if (delta == 1000000) {
            ReleaseBall();
            printf("Releasing ball\r\n");
        }else if(delta == 2000000){
            printf("stopping servo\r\n");
            BeaconLeft();
        }else if(delta == 3000000){
            ResetBall();
            printf("Reseting ball\r\n");
        }else if(delta == 4000000){
            printf("stopping servo\r\n");
            BeaconForward();
            delta = 0;
        }

        //        if (delta < 1400 && increase == 1) {
        //            ++delta;
        //            RC_SetPulseTime(RC_PORTZ08, MINPULSE + delta);
        //        } else {
        //            increase = 0;
        //        }
        //        
        //        if (delta > 25 && increase == 0) {
        //            --delta;
        //            RC_SetPulseTime(RC_PORTZ08, MINPULSE + delta);
        //        } else {
        //            increase = 1;
        //        }


        //Beacon Detector Test
        //        if (BeaconDetected() == TRUE) {
        //            printf("Beacon Detected!\r\n");
        //        }

        //Track Wire Sensor Tests
        //        printf("front track wire input = %u\r\n", ReadFrontTrackWire());
        //        printf("back track wire input = %u\r\n", ReadBackTrackWire());

        //Bumper Tests
        //        if (ReadFrontLeftBumper() == PRESSED) {
        //            printf("Front Left Bumper pressed!\r\n");
        //        }
        //        if (ReadFrontRightBumper() == PRESSED) {
        //            printf("Front Right Bumper pressed!\r\n");
        //        }
        //        if (ReadBackLeftBumper() == PRESSED) {
        //            printf("Back Left Bumper pressed!\r\n");
        //        }
        //        if (ReadBackRightBumper() == PRESSED) {
        //            printf("Back Right Bumper pressed!\r\n");
        //        }

        //Tape Sensor Tests
        //        if (ReadFrontLeftTapeSensor() == ON) {
        //            printf("Front Left Tape detected!\r\n");
        //        }
        //        if (ReadFrontRightTapeSensor() == ON) {
        //            printf("Front Right Tape detected!\r\n");
        //        }
        //        if (ReadBackLeftTapeSensor() == ON) {
        //            printf("Back Left Tape detected!\r\n");
        //        }
        //        if (ReadBackRightTapeSensor() == ON) {
        //            printf("Back Right Tape detected!\r\n");
        //        }
        //        if (ReadLeftHoleTapeSensor() == ON) {
        //            printf("Back Left Hole Tape detected!\r\n");
        //        }
        //        if (ReadRightHoleTapeSensor() == ON) {
        //            printf("Back Right Hole Tape detected!\r\n");
        //        }


        //        int i = 0;
        //        while (i < 100000) {
        //            ++i;
        //        }
    }

    return 0;
}
