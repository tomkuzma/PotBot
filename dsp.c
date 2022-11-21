/*
 * dsp.c
 *
 *  Created on: Nov. 14, 2022
 *      Author: Jimmy Bates, Tom Kuzma
 */

//****** moving_average ********//
//
// calculates moving average of input array, unsigned ints for adc
//
// Arguments:
// uint16_t out - output buffer
// uint16_t in - input array
// uint8_t N - how many terms to be averaged
// uint8_t start - position pointer should be at for input
//
// Return : None
//
//**************************//

#include "dsp.h"


void moving_average(int16_t *out, int16_t *in, int16_t N, int16_t start) {
   // Pointer which holds input values
   int* p = NULL;

   //New variable for holding output (int32 to accept overflows)
   int32_t temp_out;
   int32_t temp_out_divided;

   //Iterate through input
   for (p = in + start; p < in + (start + N); ++p) {
       temp_out += (int32_t) *p;
   }

   //Store output in buffer
   temp_out_divided = temp_out / N;

   *out = (int16_t) temp_out_divided;
}


//********** ikine **********//
//
// calculates inverse kinematics going from x and y parameters to joint1 and joint2 servos
//
// Arguments:
// int16_t *joint1 - servo 1 value (output)
// int16_t *joint2 - servo 2 value (output)
// int16_t*x - position x (input)
// int16_t y - position y (input)
//
// Return : None
//
//**************************//
void ikine(int16_t *joint1, int16_t *joint2, int32_t x, int32_t y) {

    //For joint 1
    int32_t num1 = 300*y - sqrt_i32(90000*y*y - y*y*y*y + 90000*x*x - 2*x*x*y*y - x*x*x*x);
    int32_t denom1 = (x*x + 300*x + y*y);
    int neg_flag = 0;
    if (num1 < 0) {
        neg_flag = 1;
        num1 = -num1;
    }
    //The fixed-point atan seems to have a problem with going out of range, so if the denom or num are big,
    //Then bit shift both the num or denom so the function can handle it
    if(num1 > 10000 || denom1 > 10000) {
        num1 = num1 >> 6;
        denom1 = denom1 >> 6;
    }
    int32_t joint_temp = 2*atan2_fp(num1/10,denom1/10);
    if (neg_flag==1) *joint1 = -joint_temp;
    else *joint1 = joint_temp;


    //For joint 2
    *joint2 = 2*atan2_fp(sqrt_i32(90000 - y*y - x*x),sqrt_i32(x*x + y*y));
}

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
int32_t sqrt_i32(int32_t v) {
    uint32_t b = (unsigned long int) 1<<30, q = 0, r = v;
    while (b > r)
        b >>= 2;
    while( b > 0 ) {
        uint32_t t = q + b;
        q >>= 1;
        if( r >= t ) {
            r -= t;
            q += b;
        }
        b >>= 2;
    }
    return q;
}


//********** atan2_fp **********//
//
// calculates approximation of atan using fixed point
// Taken from https://www.dsprelated.com/showthread/comp.dsp/28979-3.php
//
// Arguments:
// int y_fp - servo 1 value (output)
// int x_fp - servo 2 value (output)
//
// Return : Int of angle
//
//**************************//
int32_t atan2_fp(int32_t y_fp, int32_t x_fp) {
    int32_t coeff_1 = 45;
    int32_t coeff_1b = -56; // 56.24;
    int32_t coeff_1c = 11;  // 11.25
    int32_t coeff_2 = 135;

    int32_t angle = 0;

    int32_t r;
    int32_t r3;

    int32_t y_abs_fp = y_fp;
    if (y_abs_fp < 0)
        y_abs_fp = -y_abs_fp;

    // On the x axis, i.e. angle is 0 or 180
    if (y_fp == 0) {
        if (x_fp >= 0) angle = 0;
        else angle = 180;
    }
    // In quadrant 1 or 4
    else if (x_fp >= 0)
    {
        r = ((x_fp - y_abs_fp) << MULTIPLY_FP_RESOLUTION_BITS) /
            (x_fp + y_abs_fp);

        r3 = r * r;
        r3 = r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= r;
        r3 = r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= coeff_1c;
        angle = (coeff_1 + ((coeff_1b * r + r3) >>
            MULTIPLY_FP_RESOLUTION_BITS));
    }
    // In quadrant 2 or 3
    else
    {
        r = (((x_fp + y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) /
            ((y_abs_fp - x_fp));
        r3 = r * r;
        r3 = r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= r;
        r3 = r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= coeff_1c;
        angle = coeff_2 + ((int32_t )(((coeff_1b * r + r3) >>
            MULTIPLY_FP_RESOLUTION_BITS)));
    }

    if (y_fp < 0)
        return (-angle);     // negate if in quad III or IV
    else
        return (angle);
}
