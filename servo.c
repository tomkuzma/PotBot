/*
 * servo.c
 *
 *  Created on: Nov. 7, 2022
 *      Author: Jimmy Bates, Tom Kuzma
 *
 * Initializes servos, other functions related to servo function here
 * Refer to servo.h for more detailed description
 */

#include "servo.h"

//Servo parameters - made global so they don't have to be recalculated
uint16_t __servos_c_pwm_min[8];
uint16_t __servos_c_pwm_max[8];


//****** servo_init ********//
//
// Initializes all parameters needed for managing servos
//
// Arguments: servos (int) - how many servos will be set
// Return : None
//
//**************************//
void servo_init(int servos, float dc_min[8], float dc_max[8]) {
    EALLOW; //Edit protected reg's

    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;   // Enable TBCLK

    //Enable PWMs, depending on how many servos there are

    //This will need to be set up no matter how many servos
    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;  // ePWM1
    EPwm1Regs.TBPRD = SERVO_PERIOD;
    EPwm1Regs.CMPA.half.CMPA = 0; // for reference - this is what sets PWM duty cycle
    EPwm1Regs.CMPB = 0; // for reference - this is what sets PWM duty cycle
    EPwm1Regs.TBCTR = 0; // clear TB counter
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Counter mode, Counts from 0 -> TBPRD
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // Disable synchronization output
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // Divide clock by 4
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV32; // Divide clock by 8
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
    EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR; // Force EPWM Low when counter == CMPB

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;     // 0=GPIO,  1=EPWM1A,  2=Resv,  3=Resv
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)

    if(servos > 1) {
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;     // 0=GPIO,  1=EPWM1B,  2=EMU0,  3=COMP1OUT
        GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
    }

    //Setup for PWM 2, if there's 3-4 servos
    if (servos > 2) {

        GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;     // 0=GPIO,  1=EPWM1A,  2=Resv,  3=Resv
        GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO0 (EPWM1A)

        if(servos > 3) {
            GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;     // 0=GPIO,  1=EPWM1B,  2=EMU0,  3=COMP1OUT
            GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
        }

        SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;  // ePWM2
        EPwm2Regs.TBPRD = SERVO_PERIOD;
        EPwm2Regs.CMPA.half.CMPA = 0; // for reference - this is what sets PWM duty cycle
        EPwm2Regs.CMPB = 0; // for reference - this is what sets PWM duty cycle
        EPwm2Regs.TBCTR = 0; // clear TB counter
        EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Counter mode, Counts from 0 -> TBPRD
        EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // Disable synchronization output
        EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // Divide clock by 4
        EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV32; // Divide clock by 8
        EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
        EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
        EPwm2Regs.AQCTLB.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
        EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
    }

    //Setup for PWM 3, if there's 5-6 servos
    if (servos > 4) {

        GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;     // 0=GPIO,  1=EPWM1A,  2=Resv,  3=Resv
        GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO0 (EPWM1A)

        if(servos > 5) {
            GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;     // 0=GPIO,  1=EPWM1B,  2=EMU0,  3=COMP1OUT
            GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
        }

        SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;  // ePWM3
        EPwm3Regs.TBPRD = SERVO_PERIOD;
        EPwm3Regs.CMPA.half.CMPA = 0; // for reference - this is what sets PWM duty cycle
        EPwm3Regs.CMPB = 0; // for reference - this is what sets PWM duty cycle
        EPwm3Regs.TBCTR = 0; // clear TB counter
        EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Counter mode, Counts from 0 -> TBPRD
        EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // Disable synchronization output
        EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // Divide clock by 4
        EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV32; // Divide clock by 8
        EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
        EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
        EPwm3Regs.AQCTLB.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
        EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
    }

    //Setup for PWM 4, if there's 7-8 servos
    if (servos > 6) {

        GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;     // 0=GPIO,  1=EPWM1A,  2=Resv,  3=Resv
        GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;    // Disable pull-up on GPIO0 (EPWM1A)

        if(servos > 7) {
            GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;     // 0=GPIO,  1=EPWM1B,  2=EMU0,  3=COMP1OUT
            GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
        }

        SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;  // ePWM4
        EPwm4Regs.TBPRD = SERVO_PERIOD;
        EPwm4Regs.CMPA.half.CMPA = 0; // for reference - this is what sets PWM duty cycle
        EPwm4Regs.CMPB = 0; // for reference - this is what sets PWM duty cycle
        EPwm4Regs.TBCTR = 0; // clear TB counter
        EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Counter mode, Counts from 0 -> TBPRD
        EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // Disable synchronization output
        EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // Divide clock by 4
        EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV32; // Divide clock by 8
        EPwm4Regs.AQCTLA.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
        EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
        EPwm4Regs.AQCTLB.bit.ZRO = AQ_SET;   // Force EPWM High when counter resets
        EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR; // Force EPWM Low when counter == CMPB
    }

    EDIS;

    //Set PWM Mins
    int i = 0;
    for(i = 0; i<servos; i++) {
        __servos_c_pwm_min[i] = (float) SERVO_PERIOD * dc_min[i];
        __servos_c_pwm_max[i] = (float) SERVO_PERIOD * dc_max[i];

    }
}

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
void servo_set(uint16_t channel, uint16_t input) {

    //Vars for calculating servo position
    uint32_t input_conv;

    //Convert value for PWM
    input_conv = ((uint32_t) __servos_c_pwm_max[channel] - (uint32_t) __servos_c_pwm_min[channel]) * (uint32_t) input / ADC_MAX + (uint32_t) __servos_c_pwm_min[channel];

    EALLOW;
    switch(channel) {
    case 0:
        SERVO_1_REG = input_conv;
        break;
    case 1:
        SERVO_2_REG = input_conv;
        break;
    case 2:
        SERVO_3_REG = input_conv;
        break;
    case 3:
        SERVO_4_REG = input_conv;
        break;
    case 4:
        SERVO_5_REG = input_conv;
        break;
    case 5:
        SERVO_6_REG = input_conv;
        break;
    case 6:
        SERVO_7_REG = input_conv;
        break;
    case 7:
        SERVO_8_REG = input_conv;
        break;
    default:
        break;
    }
    EDIS;

    //*servo = servo_convert(degrees);

}
