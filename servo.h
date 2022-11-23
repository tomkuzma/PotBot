/*
 * servo.h
 *
 *  Created on: Nov. 7, 2022
 *      Author: Jimmy Bates and Tom Kuzma
 *
 * Takes care of servos which requires:
 * Taking ADC values and loading them into PWM
 * Each ePWM has 2 channels, we can therefore use 1 channel
 *
 *
 * More info:
 * Pins for this are:
 * ePWM 1
 * a - GPIO0 (J6-1)
 * b - GPIO1 (J6-2)
 *
 * ePWM 2
 * a - GPIO2 (J6-3)
 * b - GPIO3 (J6-4)
 *
 * ePWM 3
 * a - GPIO4 (J6-5)
 * b - GPIO5 (J6-6)
 *
 * ePWM 4
 * a - GPIO6 (J2-8)
 * b - GPIO7 (J2-9)
 *
 * Servos seem to go from 1.8%DC -> 12.5%DC
 *
 */

#ifndef SERVO_H_
#define SERVO_H_

#define SERVO_PERIOD 37499 // for 50hz

#define SERVO_MAX 900   // max degrees
#define SERVO_MIN -900     // min degrees

/*****MUST CHANGE******/
#define Z_MAX  45 // Degrees when z==1
#define Z_MIN -45 // Degrees when z==0

//Pointers to write the value to
#define SERVO_1_REG EPwm1Regs.CMPA.half.CMPA
#define SERVO_2_REG EPwm1Regs.CMPB
#define SERVO_3_REG EPwm2Regs.CMPA.half.CMPA
#define SERVO_4_REG EPwm2Regs.CMPB
#define SERVO_5_REG EPwm3Regs.CMPA.half.CMPA
#define SERVO_6_REG EPwm3Regs.CMPB
#define SERVO_7_REG EPwm4Regs.CMPA.half.CMPA
#define SERVO_8_REG EPwm4Regs.CMPB

#include "F2807x_EPwm_defines.h"
#include "Headers/F2802x_device.h"
#include "adc.h"

//****** servo_init ********//
//
// Initializes all parameters needed for managing servos
//
// Arguments: servos (int) - how many servos will be set
// uint16_t dc_min[8] -
// uint16_t dc_max[8]
//
// Return : None
//
//**************************//
void servo_init(int servos, float dc_min[8], float dc_max[8]);

//****** servo_set  ********//
//
// Sets servo pointer PWM value
//
// Arguments:
// channel (int) - channel to the register that will be set
// input (int) - degrees or adc reading
// Return : None
//
//**************************//
void servo_set(uint16_t channel, int16_t input);

//****** enable_epwm_interrupts  ********//
//
// Clears spurious ePWM flags and enables ePWM interrupts
//
// Arguments:
// int servos - how many servos there are (2 servos per epwm interrupt)
//
// Return : None
//
//**************************//
void enable_epwm_interrupts(int servos);

#endif /* SERVO_H_ */
