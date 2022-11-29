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
// uint16_t in - input array
// uint16_t out - output buffer
// uint8_t N - how many terms to be averaged
// uint8_t start - position pointer should be at for input
//
// Return : None
//
//**************************//

#include "dsp.h"


void moving_average(int16_t *in, int16_t *out, int16_t N, int16_t start)
{
   // Pointer which holds input values
   int* p = NULL;

   //New variable for holding output (int32 to accept overflows)
   int32_t temp_out=0;
   int32_t temp_out_divided=0;

   //Iterate through input
   for (p = in + start; p < in + (start + N); ++p)
   {
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
// int16_t x - position x (input)
// int16_t y - position y (input)
//
// Return : None
//
//**************************//
void ikine(int16_t *joint1, int16_t *joint2, int32_t x, int32_t y)
{

    if(sqrt_i32(x*x + y*y) < 300)
    {

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
        if(num1 > 10000 || denom1 > 10000)
        {
            num1 = num1 >> 6;
            denom1 = denom1 >> 6;
        }
        int32_t joint_temp = 20*atan2_fp(num1/10,denom1/10);
        if (neg_flag==1) *joint1 = -joint_temp;
        else *joint1 = joint_temp;


        //For joint 2
        *joint2 = 20*atan2_fp(sqrt_i32(90000 - y*y - x*x),sqrt_i32(x*x + y*y));

    }
}

/************ikine_fixed*************
*
* Floating point implementation of inverse kinematics
*
* Arguments:
* int* joint1 - Angle in degrees output joint 1
* int* joint2 - Angle in degrees output joint 2
* int x_in - Coordinate input of x
* int y_in - Coordinate input of y
*
* Return:
* 1 if works, 0 if not worky
* ***********************************/
float yf;
float xf;
int ikine_float(int *joint1, int *joint2, float x_in, float y_in)
{
    //Normalize to be 0-1
    xf = (float) x_in / 300;
    yf = (float) y_in / 300;

    //Fix if singularity
    if (x_in <= 0 && y_in <= 0)
    {
       return 0; // singularity, probably bad data. just return.
    }

    if (x_in > 300 || y_in > 300)
    {
       return 0; // invalid
    }

    //Fix if over 1
    if ((x_in * x_in + y_in * y_in) > 90000)
    {
       //Can just use unit vectors
       float radius = sqrt(xf * xf + yf * yf);
       xf = xf / radius;
       yf = yf / radius;
    }

    //Fix if over 1
    if ((x_in*x_in + y_in*y_in)< 81) {
       //Need to
       float radius = sqrt(xf * xf + yf * yf);
       xf = 0.03 * xf / radius;
       yf = 0.03 * yf / radius;
    }

    //Factor values into numerators and denominators
    float x2 = xf * xf;
    float y2 = yf * yf;
    float num1 = yf - sqrt(-x2 * x2 - 2 * x2 * y2 + x2 - y2 * y2 + y2);
    float num2 = -x2 - y2 + 1;
    float denom1 = x2 + xf + y2;
    float denom2 = x2 + y2;

    //Offload variables from atan
    int joint1_temp = JOINT_FACTOR * atan(num1 / denom1);
    int joint2_temp = JOINT_FACTOR * atan(sqrt(num2 / denom2));

    if(joint1_temp>900)
        joint1_temp -= JOINT_OFFSET;

    //Check to make sure these values make sense
    if(joint1_temp <= 900 && joint1_temp >= -900)
    {
        *joint1 = joint1_temp;
    }
    if(joint2_temp <= 1800 && joint2_temp >= 0)
    {
        *joint2 = joint2_temp;
    }

    return 1; // Works
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
