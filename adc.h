/*
 * adc.h
 *
 *  Created on: Nov. 9, 2022
 *      Author: Jimmy Bates, Tom Kuzma
 *
 * Initializes and runs ADC, including temperature
 * Refer to adc.h for more detailed description
 *
 * More info:
 * ADC pins:
 * A0 - J5-6
 * A1 - J5-5
 * A2 - J1-8
 * A3 - J5-4
 *
 */

#ifndef ADC_H_
#define ADC_H_

//ADC calibration
#define Device_cal (void   (*)(void))0x3D7C80

//Definitions for temperature sensor
#define getTempSlope (*(int (*)(void))0x3d7e80)
#define getTempOffset (*(int (*)(void))0x3d7e83)

#define ADC_SETUP_CYCLE 1000 //cycles to wait for adc to power up

#define ADC_MAX 4095
#define ADC_MIN 0

//Triggers for the trigger select registers on ADC
#define TRIGGER_CPU_TIMER_0 0x1
#define TRIGGER_CPU_TIMER_1 0x2
#define TRIGGER_CPU_TIMER_2 0x3
#define TRIGGER_XINT2       0x4
#define TRIGGER_EPWM1A      0x5
#define TRIGGER_EPWM1B      0x6
#define TRIGGER_EPWM2A      0x7
#define TRIGGER_EPWM2B      0x8
#define TRIGGER_EPWM3A      0x9
#define TRIGGER_EPWM3B      0xA
#define TRIGGER_EPWM4A      0xB
#define TRIGGER_EPWM4B      0xC

#include "Headers/F2802x_device.h"

//******* adc_init***********//
//
// Initializes adc channels needed
//
// Arguments:
// int channels - the amount of channels needed to sample, not including temperature
// bool temp_enable - if true, will initialize adc for temperature reading
//
// Return: none
//***************************//
void adc_init(int channels, bool temp_enable);

//******* adc_trigger_select***********//
//
// Changes what triggers the adc channel
//
// Arguments:
// uint16_t channel - channel to change trigger of
// uint16_t trigger - what the adc trigger select triggers on
//
// Return: none
//***************************//
void adc_trigger_select(uint16_t channel, uint16_t trigger);

//******* adc_sample***********//
//
// Starts conversion and then returns value for channel
//
// Arguments:
// int channel - channel to be sampled
// bool soc - if you want new conversion (blocking)
//
// Return value of adc sample
//***************************//
int16_t adc_sample(int channel, bool soc);


//******* temp_sample***********//
//
// Returns value of temperature, converted to q15 Celsius
//
// Arguments:
// bool soc - if you want new conversion (blocking)
//
// Return value of temperature
//***************************//
int32 temp_sample(bool soc);


//******* y_fit16 ***********//
//
// Allows for a conversion of an input within a certain range to a 16-bit output within a certain range
//
// Arguments:
// int16_t *out - pointer of output variable you want changed
// int16_t *in - pointer of input variable to change to out
// int x_min - input minimum
// int x_max - input maximum
// int y_min - output minimum
// int y_max - output maximum
//
// Return: none
//***************************//
void y_fit(int16_t *in, int16_t *out, int x_min, int x_max, int y_min, int y_max);

#endif /* ADC_H_ */

