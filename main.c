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

//includes:
#include "adc.h"
#include "servo.h"

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include <xdc/runtime/SysStd.h>  // Going to see if this library works better for printf

#include <Headers/F2802x_device.h>

//function prototypes:
extern void DeviceInit(void);


#define SERVO_COUNT 2 // amount of servos
#define ADC_COUNT 3 // amount of adcs


//declare global variables:
volatile Bool isrFlag = FALSE; //flag used by idle function
volatile Bool isrFlag2 = FALSE; // Flag used by second idle function -- JB
volatile UInt tickCount = 0; //counter incremented by timer interrupt

uint16_t adc_result_1, adc_result_2, adc_result_3;

/* ======== main ======== */
Int main()
{ 
    //initialization:
    DeviceInit(); //initialize processor

    //Servos init
    //Set servo
    float dc_min[8] = { 0.018, 0.018, 0.018, 0.018, 0.018 ,0.018 , 0.018 , 0.018};
    float dc_max[8] = { 0.118, 0.118, 0,0,0,0,0,0 };
    servo_init(2, dc_min, dc_max); // initialize 3 servos

    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on

    //jump to RTOS (does not return):
    BIOS_start();
    return(0);
}



/* ======== myTickFxn ======== */
//Timer tick function that increments a counter and sets the isrFlag
//Entered 100 times per second if PLL and Timer set up correctly
Void myTickFxn(UArg arg)
{
    static int count = 0; // Keeps track of how many times ISR called -- JB
    if(++count == COUNT_MAX) {
        isrFlag2 = TRUE; // Tells idle thread 2 to count seconds
        count = 0; //Reset count -- JB
    }


    tickCount++; //increment the tick counter
    if(tickCount % 5 == 0) {
        isrFlag = TRUE; //tell idle thread to do something 20 times per second
    }
}

/* ======== myIdleFxn ======== */
//Idle function that is called repeatedly from RTOS
Void myIdleFxn(Void)
{
   if(isrFlag == TRUE) {
       isrFlag = FALSE;
       //toggle blue LED:
       GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
   }
}

/* ======== myIdleFxn2 ======== */
//Idled function that prints time in seconds to the system display
Void myIdleFxn2(Void)
{
   static int seconds = 0; // counts seconds --jb

   if(isrFlag2 == TRUE) {
       isrFlag2 = FALSE; //reset isr -- jb
       seconds++; //increment seconds -- jb
       System_printf("Time (Sec) = %d\n", seconds); // prints seconds to rtos display
   }
}


/* ======== test_pin ======== */
//ISR'd event with timer that toggles every ISR
Void test_pin(void) {
    GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1; // Toggle every ISR, hopefully. Clock will be 2x this.
}

/*----- READ ADCS-------*/
void read_adc_1(void) {
    adc_result_1 = adc_sample(0, true); //Sample ADCRESULT0, start conversion
}

void read_adc_2(void) {
    adc_result_2 = adc_sample(1, true); //Sample ADCRESULT1, start conversion
}

void read_adc_3(void) {
    adc_result_3 = adc_sample(2, true); //Sample ADCRESULT2, start conversion
}

void read_temp(void) {
    temp_sample(1); // Start SOC and sample temp
}



/*------ FIRS ------- */




/*----- SET SERVO PWMS -----*/


/*----- PWM TIMER -------*/





/*----- DEBOUNCE BUTTON AND LISTEN ------*/






/*------ TOOL TIP -------------*/

