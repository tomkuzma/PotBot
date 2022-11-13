/*
 * adc.c
 *
 *  Created on: Nov. 9, 2022
 *      Author: Jimmy Bates, Tom Kuzma
 *
 * Initializes and runs ADC, including temperature
 * Refer to adc.h for more detailed description
 */

#include "adc.h"

//Declaring temp parameters as globals so they don't have to be calculated each time
int16 __adc_c_temp_slope;
int16 __adc_c_temp_offset;

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
void adc_init(int channels, bool temp_enable) {
    EALLOW;

    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;    // ADC enable clock
    AdcRegs.ADCCTL1.all = 0x00e0; //simultaneously power up ADC's analog circuitry, bandgap, and reference buffer
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1; //pulse at end of conversion
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1; //Enable ADC

    int count_adc=0;
    while(count_adc<=ADC_SETUP_CYCLE) count_adc++; //Give time for adc to power up

    //Setup for first channel (A0)
    AdcRegs.ADCSOC0CTL.bit.CHSEL = 0; // Sample channel 0
    AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x6; // Minimum window

    AdcRegs.INTSEL1N2.bit.INT1SEL = 0; // Connect EOC0 -> ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1E = 1; // Enable ADC EOC Interrupt

    if(channels >= 2) {
        //Setup for second channel (A1)
        AdcRegs.ADCSOC1CTL.bit.CHSEL = 1; // Sample channel 1
        AdcRegs.ADCSOC1CTL.bit.ACQPS = 0x6; // Minimum window

        AdcRegs.INTSEL1N2.bit.INT2SEL = 1; // Connect EOC1 -> ADCINT2
        AdcRegs.INTSEL1N2.bit.INT2E = 1; // Enable ADC EOC Interrupt
    }

    if(channels >= 3) {
        //Setup for third channel (A2)
        AdcRegs.ADCSOC2CTL.bit.CHSEL = 2; // Sample channel 2
        AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x6; // Minimum window

        AdcRegs.INTSEL3N4.bit.INT3SEL = 2; // Connect EOC2 -> ADCINT3
        AdcRegs.INTSEL3N4.bit.INT3E = 1; // Enable ADC EOC Interrupt
    }

    if(channels >= 4) {
        //Setup for fourth channel (A3)
        AdcRegs.ADCSOC3CTL.bit.CHSEL = 3; // Sample channel 3
        AdcRegs.ADCSOC3CTL.bit.ACQPS = 0x6; // Minimum window

        AdcRegs.INTSEL3N4.bit.INT4SEL = 3; // Connect EOC3 -> ADCINT4
        AdcRegs.INTSEL3N4.bit.INT4E = 1; // Enable ADC EOC Interrupt
    }

    //If temp enable, set up the ADC for temperature
    if(temp_enable) {
        AdcRegs.ADCCTL1.bit.TEMPCONV = 1; //Connect A5 - temp sensor
        AdcRegs.ADCSOC15CTL.bit.CHSEL = 5; // Assign last ADC channel to temperature sensor
        AdcRegs.ADCSOC15CTL.bit.ACQPS = 0x6; // Minimum window

        AdcRegs.INTSEL9N10.bit.INT9SEL = 15; // Connect EOC15 -> ADCINT5
        AdcRegs.INTSEL9N10.bit.INT9E = 1; // Enable ADC EOC Interrupt

        __adc_c_temp_slope = getTempSlope(); // Temp slope for equation
        __adc_c_temp_offset = getTempOffset(); // Temp offset for equation
    }

    EDIS;
}

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
void adc_trigger_select(uint16_t channel, uint16_t trigger) {
    EALLOW;

    switch(channel) {
    case 0:
        AdcRegs.ADCSOC0CTL.bit.TRIGSEL = trigger;
        break;
    case 1:
        AdcRegs.ADCSOC1CTL.bit.TRIGSEL = trigger;
        break;
    case 2:
        AdcRegs.ADCSOC2CTL.bit.TRIGSEL = trigger;
        break;
    case 3:
        AdcRegs.ADCSOC3CTL.bit.TRIGSEL = trigger;
        break;
    case 15:
        AdcRegs.ADCSOC15CTL.bit.TRIGSEL = trigger;
        break;
    default:
        break;
    }

    EDIS;
}


//******* adc_sample***********//
//
// clears flag (optional) and returns value for channel
//
// Arguments:
// int channel - channel to be sampled
//
// Return value of adc sample
//***************************//
int16_t adc_sample(int channel, bool soc) {
    switch(channel) {
    case 0:
        //If Soc is true, start conversion
        if(soc) {
        AdcRegs.ADCSOCFRC1.bit.SOC0=1; //Start conversion
        while(AdcRegs.ADCINTFLG.bit.ADCINT1==0) {} //Wait for eoc
        AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //Clear flag
        }
        return AdcResult.ADCRESULT0; // Return adc result channel 0
        break;
    case 1:
        //If Soc is true, start conversion
        if(soc) {
        AdcRegs.ADCSOCFRC1.bit.SOC1=1; //Start conversion
        while(AdcRegs.ADCINTFLG.bit.ADCINT2==0) {} //Wait for eoc
        AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1; //Clear flag
        }
        return AdcResult.ADCRESULT1; // Return adc result channel 1
        break;
    case 2:
        //If Soc is true, start conversion
        if(soc) {
        AdcRegs.ADCSOCFRC1.bit.SOC2=1; //Start conversion
        while(AdcRegs.ADCINTFLG.bit.ADCINT3==0) {} //Wait for eoc
        AdcRegs.ADCINTFLGCLR.bit.ADCINT3 = 1; //Clear flag
        }
        return AdcResult.ADCRESULT2; // Return adc result channel 2
        break;
    case 3:
        //If Soc is true, start conversion
        if(soc) {
        AdcRegs.ADCSOCFRC1.bit.SOC3=1; //Start conversion
        while(AdcRegs.ADCINTFLG.bit.ADCINT4==0) {} //Wait for eoc
        AdcRegs.ADCINTFLGCLR.bit.ADCINT4 = 1; //Clear flag
        }
        return AdcResult.ADCRESULT3; // Return adc result channel 0
        break;
    default:
        break;
    }

    return -1;
}


//******* temp_sample***********//
//
// Returns value of temperature, converted to q15 Celsius
//
// Arguments:
// bool soc - if you want new conversion (blocking)
//
// Return value of temperature
//***************************//
int32 temp_sample(bool soc) {

    //If soc is true, start conversion
    if(soc) {
        AdcRegs.ADCSOCFRC1.bit.SOC15 = 1; // soc
        while(AdcRegs.ADCINTFLG.bit.ADCINT9==0); // wait for eoc
        AdcRegs.ADCINTFLGCLR.bit.ADCINT9 = 1; // clear interrupt flag for adc
    }
    return (int32)(AdcResult.ADCRESULT15 - __adc_c_temp_offset) * (int32)__adc_c_temp_slope; // return converted temperature

}
