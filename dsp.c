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
void ikine(int16_t *joint1, int16_t *joint2, int16_t x, int16_t y) {

}
