/*
 * dsp.h
 *
 *  Created on: Nov. 14, 2022
 *      Author: Jimmy Bates and Tom Kuzma
 *
 * FIR module based on variable moving average
 *
 */

#ifndef DSP_H_
#define DSP_H_

#define N_MIN 10  //Min number of h[n] terms
#define N_MAX 100 //Max number of h[n] terms
#define MULTIPLY_FP_RESOLUTION_BITS 15 // for fixed point atan

#include "Headers/F2802x_device.h"
#include <math.h>

//Stuff for input arrays
#define FIR_INPUT_SIZE N_MAX // Size of input array

//****** moving_average ********//
//
// calculates moving average of input array, unsigned ints for adc
//
// Arguments:
// int16_t out - output buffer
// int16_t in - input array
// int16_t N - how many terms to be averaged
// int16_t start - position pointer should be at for input
//
// Return : None
//
//**************************//
void moving_average(int16_t *out, int16_t *in, int16_t N, int16_t start);

//********** ikine **********//
//
// calculates inverse kinematics going from x and y parameters to joint1 and joint2 servos
//
// Arguments:
// int16_t *joint1 - servo 1 value (output)
// int16_t *joint2 - servo 2 value (output)
// int16_t x - position x (input)
// int16_t y - position y (input)
//
// Return : None
//
//**************************//
void ikine(int32_t *joint1, int32_t *joint2, int32_t x, int32_t y);

//********** sqrt_i32 **********//
//
// calculates fixed point square roots using bit shift
// Taken from https://github.com/chmike/fpsqrt/blob/master/fpsqrt.c
//
// Arguments:
// int v - squared integer to be square rooted
//
// Return : Int of square root
//
//**************************//
int32_t sqrt_i32(int32_t v);

//********** atan2_fp **********//
//
// calculates approximation of atan using fixed point
// Taken from https://www.dsprelated.com/showthread/comp.dsp/28979-3.php
//
// Arguments:
// int y_fp - numerator of atan function
// int x_fp - denominator of atan function
//
// Return : Int of angle
//
//**************************//
int32_t atan2_fp(int32_t y_fp, int32_t x_fp);

#endif /* DSP_H_ */
