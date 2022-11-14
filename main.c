// Filename:            Lab2Idle_main.c
//
// Description:         This file has a main, timer, and idle function for SYS/BIOS application.
//
// Target:              TMS320F28027
//
// Author:              DR
//
// Date:                Oct. 11, 2022
//
//
// Edited by : Jimmy Bates
// A01035957 (Set T)
// Oct 23 2022 (yea i know, doing this last minute)
// Mainly, adding another ISR for testing, and adding another idle thread
//defines:
#define xdc__strict //suppress typedef warnings
#define COUNT_MAX 99 // Counts up to 99 and then resets to 0

#define SERVO_COUNT 2 // amount of servos
#define ADC_COUNT 3 // amount of adcs


//includes:
#include "adc.h"
#include "servo.h"

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>


#include <xdc/runtime/SysStd.h>  // Going to see if this library works better for printf

#include <Headers/F2802x_device.h>

//function prototypes:

extern void DeviceInit(void);


//SWI func prototypes
extern const Swi_Handle Swi0;



//declare global variables:
volatile Bool isrFlag = FALSE; //flag used by idle function
volatile Bool isrFlag2 = FALSE; // Flag used by second idle function -- JB
volatile UInt tickCount = 0; //counter incremented by timer interrupt

int32 c2000_temp;
float real_temp;

uint16_t adc_result_1, adc_result_2, adc_result_3;

/* ======== main ======== */
Int main()
{ 
    //initialization:
    DeviceInit(); //initialize processor

    //initialize global variables (clear the gah-bage)
    c2000_temp = 0;

    //Servos init
    //Set servo
    float dc_min[8] = { 0.018, 0.018, 0.018, 0.018, 0.018 ,0.018 , 0.018 , 0.018};
    float dc_max[8] = { 0.118, 0.118, 0,0,0,0,0,0 };
    servo_init(2, dc_min, dc_max); // initialize 3 servos

    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on
    adc_trigger_select(0, TRIGGER_CPU_TIMER_2);
    adc_trigger_select(1, TRIGGER_CPU_TIMER_2);
    adc_trigger_select(2, TRIGGER_CPU_TIMER_2);
    adc_trigger_select(3, TRIGGER_CPU_TIMER_2);
    adc_trigger_select(15, TRIGGER_CPU_TIMER_2);

    //jump to RTOS (does not return):
    BIOS_start();
    return(0);
}


/*----- READ ADCS-------*/
void set_servo_1(void) {
    //Set Servo PWM
    servo_set(0, adc_result_1);

    //FIR POST

    //Sample ADC
    adc_result_1 = adc_sample(0, false); //Sample ADCRESULT0, start conversion
}

void read_adc_2(void) {
    adc_result_2 = adc_sample(1, false); //Sample ADCRESULT1, start conversion
}

void read_adc_3(void) {
    adc_result_3 = adc_sample(2, false); //Sample ADCRESULT2, start conversion
}

void temp_hwi(void) {
    c2000_temp = temp_sample(false); // Clear SOC and sample temp

    //convert to Q15 and express it in system_printf (later used for lcd)
    real_temp = (float) c2000_temp/32768 + (float) (c2000_temp%32768)/32768/2;
}



/*------ FIRS ------- */




/*----- SET SERVO PWMS -----*/


/*----- PWM TIMER -------*/





/*----- DEBOUNCE BUTTON AND LISTEN ------*/






/*------ TOOL TIP -------------*/

