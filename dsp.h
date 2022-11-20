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

#include "Headers/F2802x_device.h"

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
void ikine(int16_t *joint1, int16_t *joint2, int16_t x, int16_t y);

#endif /* DSP_H_ */
