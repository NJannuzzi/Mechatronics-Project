/*
 * File:   Team13Bot.h
 * Author: mdunne
 *
 * Created on January 6, 2012, 10:59 AM
 * Modified by: Max Dunne on 2015.09.18 to implement LED functions and remap 
 * pins for NEW_ROACH functionality. 
 *
 * Implements basic functionality for Roach robot for sensors and actuators
 *
 * If using a new roach, create a NEW_ROACH macro at the project level
 *              in MPLAB-X using the global preprocessor directives.
 */




#ifndef TEAM13BOT_H
#define TEAM13BOT_H

#include <BOARD.h>


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

void Team13Bot_Init(void);

char BeaconDetected(void);

unsigned int ReadFrontTrackWire(void);
unsigned int ReadBackTrackWire(void);

char ReadFrontLeftBumper(void);
char ReadFrontRightBumper(void);
char ReadBackLeftBumper(void);
char ReadBackRightBumper(void);

char ReadFrontLeftTapeSensor(void);
char ReadFrontRightTapeSensor(void);
char ReadBackLeftTapeSensor(void);
char ReadBackRightTapeSensor(void);
char ReadLeftHoleTapeSensor(void);
char ReadRightHoleTapeSensor(void);

void ReleaseBall(void);
void ResetBall(void);
void RotateBeacon(void);
void StopServos(void);

void SetLeftMotorSpeed(unsigned int dutyCycle);
void SetRightMotorSpeed(unsigned int dutyCycle);
void SetLeftMotorDirection(unsigned int dir);
void SetRightMotorDirection(unsigned int dir);

void MoveForward(unsigned int speed);
void MoveBackward(unsigned int speed);
void LeftTankTurn(unsigned int speed);
void RightTankTurn(unsigned int speed);
void LeftPivotTurn(unsigned int speed, unsigned int dir);
void RightPivotTurn(unsigned int speed, unsigned int dir);
void GradualTurn(unsigned int leftSpeed, unsigned int rightSpeed, unsigned int dir);
void SharpTurn(unsigned int leftSpeed, unsigned int rightSpeed);

void StopMotors();

#endif
