/*
 * File:   roach.c
 * Author: mdunne
 *
 * Created on December 15, 2011, 12:59 PM
 */

#include "BOARD.h"
#include <xc.h>
#include "Team13Bot.h"

#include "pwm.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "Servo.h"
#include <stdio.h>

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/

#define TRUE 1
#define FALSE 0
#define FORWARD 1
#define BACKWARD 0

#define BEACON_TAPE_PORT PORTX
#define PORTXMASK 0x17F8 //0b 0001 0111 1111 1000   not including pwm PORTX_11
#define BEACONMASK 0x0008
#define BEACONOFFSET 3

#define TAPE1MASK 0x0020  //0b 0000 0000 0010 0000  sensor 1 (pin 5)
#define TAPE1OFFSET 5
#define TAPE2MASK 0x0040  //0b 0000 0000 0100 0000  sensor 2 (pin 6)
#define TAPE2OFFSET 6
#define TAPE3MASK 0x0080  //0b 0000 0000 1000 0000  sensor 3 (pin 7)
#define TAPE3OFFSET 7
#define TAPE4MASK 0x0100  //0b 0000 0001 0000 0000  sensor 4 (pin 8)
#define TAPE4OFFSET 8
#define TAPE5MASK 0x0200  //0b 0000 0010 0000 0000  sensor 5 (pin 9)
#define TAPE5OFFSET 9
#define TAPE6MASK 0x0400  //0b 0000 0100 0000 0000  sensor 6 (pin 10)
#define TAPE6OFFSET 10

#define FRONT_TRACK_WIRE AD_PORTV3
#define BACK_TRACK_WIRE AD_PORTV4

#define BUMPER_MOTOR_PORT PORTY
#define PORTYMASK 0x0BE8 //0b 0000 1011 1110 1000   not including pwm PORTY_4,10,12

#define BUMPER1MASK 0x0008  //0b 0000 0000 0000 1000  sensor 1 (pin 3)
#define BUMPER1OFFSET 3
#define BUMPER2MASK 0x0020  //0b 0000 0000 0010 0000  sensor 2 (pin 5)
#define BUMPER2OFFSET 5
#define BUMPER3MASK 0x0080  //0b 0000 0000 1000 0000  sensor 3 (pin 7)
#define BUMPER3OFFSET 7
#define BUMPER4MASK 0x0200  //0b 0000 0010 0000 0000  sensor 4 (pin 9)
#define BUMPER4OFFSET 9

#define BALL_PWM_PIN RC_PORTZ08
#define BALL_DOWN 1900
#define BALL_UP 900
#define BEACON_FORWARD 1800
#define BEACON_LEFT 1000
#define BEACON_PWM_PIN RC_PORTZ09

#define BUMPER_PORT PORTY
#define LEFTMOTOR_ENABLE PWM_PORTY10
#define RIGHTMOTOR_ENABLE PWM_PORTY12
#define LEFTMOTORMASK 0x0040  //0b 0000 0000 0100 0000  sensor 1 (pin 6)
#define LEFTMOTOROFFSET 6
#define RIGHTMOTORMASK 0x0100  //0b 0000 0001 0000 0000  sensor 2 (pin 8)
#define RIGHTMOTOROFFSET 8
#define LEFTMOTOR_DIR 1 << LEFTMOTOROFFSET
#define RIGHTMOTOR_DIR 1 << RIGHTMOTOROFFSET



/*******************************************************************************
 * PRIVATE VARIABLES                                                           *
 ******************************************************************************/



/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/

void Team13Bot_Init(void) {
    AD_Init();
    AD_AddPins(FRONT_TRACK_WIRE | BACK_TRACK_WIRE);
    
    IO_PortsSetPortInputs(BEACON_TAPE_PORT, PORTXMASK);
    
    IO_PortsSetPortInputs(BUMPER_MOTOR_PORT, PORTYMASK);
    
    RC_Init();
    //RC_AddPins(BALL_PWM_PIN | BEACON_PWM_PIN);
    
    PWM_Init();
    PWM_SetFrequency(PWM_1KHZ);
    PWM_AddPins(LEFTMOTOR_ENABLE | RIGHTMOTOR_ENABLE);
    IO_PortsSetPortOutputs(BUMPER_MOTOR_PORT, PORTYMASK & (LEFTMOTORMASK | RIGHTMOTORMASK));
}

char BeaconDetected(void) {
    int beaconInput = (IO_PortsReadPort(BEACON_TAPE_PORT) & BEACONMASK) >> BEACONOFFSET;
    if (beaconInput == 1) {
        //printf("high\r\n");
        return FALSE;
    } else {
        //printf("low\r\n");
        return TRUE;
    }
}

unsigned int ReadFrontTrackWire(void) {
    return AD_ReadADPin(FRONT_TRACK_WIRE);
}

unsigned int ReadBackTrackWire(void) {
    return AD_ReadADPin(BACK_TRACK_WIRE);
}


char ReadFrontLeftBumper(void) {
    return (IO_PortsReadPort(BUMPER_MOTOR_PORT) & BUMPER1MASK) >> BUMPER1OFFSET ^ 1;
}

char ReadFrontRightBumper(void) {
    return (IO_PortsReadPort(BUMPER_MOTOR_PORT) & BUMPER2MASK) >> BUMPER2OFFSET ^ 1;
}

char ReadBackLeftBumper(void) {
    return (IO_PortsReadPort(BUMPER_MOTOR_PORT) & BUMPER3MASK) >> BUMPER3OFFSET ^ 1;
}

char ReadBackRightBumper(void) {
    return (IO_PortsReadPort(BUMPER_MOTOR_PORT) & BUMPER4MASK) >> BUMPER4OFFSET ^ 1;
}


char ReadFrontLeftTapeSensor(void) {
    return (IO_PortsReadPort(BEACON_TAPE_PORT) & TAPE1MASK) >> TAPE1OFFSET;
}

char ReadFrontRightTapeSensor(void) {
    return (IO_PortsReadPort(BEACON_TAPE_PORT) & TAPE2MASK) >> TAPE2OFFSET;
}

char ReadBackLeftTapeSensor(void) {
    return (IO_PortsReadPort(BEACON_TAPE_PORT) & TAPE3MASK) >> TAPE3OFFSET;
}

char ReadBackRightTapeSensor(void) {
    return (IO_PortsReadPort(BEACON_TAPE_PORT) & TAPE4MASK) >> TAPE4OFFSET;
}

char ReadLeftHoleTapeSensor(void) {
    return (IO_PortsReadPort(BEACON_TAPE_PORT) & TAPE5MASK) >> TAPE5OFFSET;
}

char ReadRightHoleTapeSensor(void) {
    return (IO_PortsReadPort(BEACON_TAPE_PORT) & TAPE6MASK) >> TAPE6OFFSET;
}




void ReleaseBall(void) {
    //RC_Init();
    RC_AddPins(BALL_PWM_PIN);
    RC_RemovePins(BEACON_PWM_PIN);
    RC_SetPulseTime(BALL_PWM_PIN, BALL_DOWN);
}
void ResetBall(void) {
    //RC_Init();
    RC_AddPins(BALL_PWM_PIN);
    RC_RemovePins(BEACON_PWM_PIN);
    RC_SetPulseTime(BALL_PWM_PIN, BALL_UP); 
}

void BeaconForward(void) {
    //RC_Init();
    RC_AddPins(BEACON_PWM_PIN);
    RC_RemovePins(BALL_PWM_PIN);
    RC_SetPulseTime(BEACON_PWM_PIN, BEACON_FORWARD);
}
void BeaconLeft(void) {
   // RC_Init();
    RC_AddPins(BEACON_PWM_PIN);
    RC_RemovePins(BALL_PWM_PIN);
    RC_SetPulseTime(BEACON_PWM_PIN, BEACON_LEFT);
}


void StopServos(void){
   // RC_RemovePins(BEACON_PWM_PIN | BALL_PWM_PIN);
    RC_End();
}

void SetLeftMotorSpeed(unsigned int speed) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, speed);
}

void SetRightMotorSpeed(unsigned int speed) {
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, speed);
}

void SetLeftMotorDirection(unsigned int dir) {
    if (dir == FORWARD) {
        IO_PortsSetPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    } else {
        IO_PortsClearPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    }
}

void SetRightMotorDirection(unsigned int dir) {
    if (dir == FORWARD) {
        IO_PortsSetPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
    } else {
        IO_PortsClearPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
    }
}


void MoveForward(unsigned int speed) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, speed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, speed);
    IO_PortsSetPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    IO_PortsSetPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
}

void MoveBackward(unsigned int speed) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, speed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, speed);
    IO_PortsClearPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    IO_PortsClearPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
}

void LeftTankTurn(unsigned int speed) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, speed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, speed);
    IO_PortsClearPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    IO_PortsSetPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
}

void RightTankTurn(unsigned int speed) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, speed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, speed);
    IO_PortsSetPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    IO_PortsClearPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
}

void LeftPivotTurn(unsigned int speed, unsigned int dir) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, 0);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, speed);
    if (dir == FORWARD) {
        IO_PortsSetPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
    } else {
        IO_PortsClearPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
    }
}

void RightPivotTurn(unsigned int speed, unsigned int dir) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, speed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, 0);
    if (dir == FORWARD) {
        IO_PortsSetPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    } else {
        IO_PortsClearPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    }
}

void GradualTurn(unsigned int leftSpeed, unsigned int rightSpeed, unsigned int dir) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, leftSpeed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, rightSpeed);
    if (dir == FORWARD) {
        IO_PortsSetPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
        IO_PortsSetPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
    } else {
        IO_PortsClearPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
        IO_PortsClearPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
    }
}

void SharpTurn(unsigned int leftSpeed, unsigned int rightSpeed) {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, leftSpeed);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, rightSpeed);
    IO_PortsSetPortBits(BUMPER_MOTOR_PORT, LEFTMOTOR_DIR);
    IO_PortsSetPortBits(BUMPER_MOTOR_PORT, RIGHTMOTOR_DIR);
}

void StopMotors() {
    PWM_SetDutyCycle(LEFTMOTOR_ENABLE, 0);
    PWM_SetDutyCycle(RIGHTMOTOR_ENABLE, 0);
}

