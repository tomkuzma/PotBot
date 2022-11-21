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

#define SERVO_COUNT 3 // amount of servos
#define ADC_COUNT 3 // amount of adcs

#define X_POS_MIN 30; // make sure it don't collide
#define Y_POS_MIN 30; // make sure it don't collide
#define X_POS_MAX 300; // make sure it don't collide
#define Y_POS_MAX 300; // make sure it don't collide

enum {
    UART_SAMPLE,
    ADC_SAMPLE
}; // Sample sources

//includes:
#include "adc.h"
#include "servo.h"
#include "dsp.h"
#include "uart.h"

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



//Note, need to volatile anything that's being used in an ISR??
//declare global variables:
volatile Bool isrFlag = FALSE; //flag used by idle function
volatile Bool isrFlag2 = FALSE; // Flag used by second idle function -- JB
volatile UInt tickCount = 0; //counter incremented by timer interrupt

//Stuff for FIR and ikine
int16_t x_array[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // x array for inputs
int16_t y_array[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // y array for inputs
int16_t x, y, z; // current values for x,y,z
int16_t x_next, y_next, z_next; // next values for x,y,z (gets put into x array and y array
int16_t fir_N;          //Length of array
int16_t x_counter, y_counter;    // Point of array where FIR is calculated from


//Stuff for UART module
char completed_string[UART_BUFF_SIZE]; // after uart transmission, store here
char buffer_string[UART_BUFF_SIZE]; // puts uart rx into a string form
int buffer_index; // keeps track of parsed string position
int buffer_string_ready; // ready to be parsed


//Joint servo positions
int16_t joint_1_deg;
int16_t joint_2_deg;


//Temperature regs
int32 c2000_temp;
float real_temp;

float test_float;

int sample_select; //select source for sampling (uart or pots)

uint16_t adc_0, adc_1, adc_2;

/* ======== main ======== */
Int main()
{ 


    //initialization:
    DeviceInit(); //initialize processor
    uart_init(); //initialize sci as uart

    //initialize global variables (clear the gah-bage)
    c2000_temp = 0;
    x = 30;
    y = 30;
    x_counter=0;
    y_counter=0;
    buffer_string_ready = 0;
    buffer_index = 0;
    fir_N = 1;
    sample_select = ADC_SAMPLE;

    //Servos init

    float dc_min[8] = { 0.018, 0.018, 0.018, 0,0,0,0,0}; // min duty cycle of servos
    float dc_max[8] = { 0.118, 0.118, 0,0,0,0,0 }; // max duty cycle of servos
    servo_init(SERVO_COUNT, dc_min, dc_max); // initialize 2 servos


    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on

    enable_epwm_interrupts(SERVO_COUNT);

    //jump to RTOS (does not return):
    BIOS_start();
    return(0);
}

float test1, test2;

void epwm1_isr(void) {
    EPwm1Regs.ETCLR.bit.INT = 1;

    //Set servos
    servo_set(1, x_next);
    servo_set(2, y_next);

    x = 212;
    y = 212;

    GpioDataRegs.GPASET.bit.GPIO19 = 1;
    ikine(&joint_1_deg, &joint_2_deg, x, y);

    GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;


    //Add array values to x and y


    //Move FIR position
}

void epwm2_isr(void) {
    EPwm2Regs.ETCLR.bit.INT = 1;
}


/*----- READ ADCS-------*/
void set_servo_1(void) {

    //Set Servo PWM
//    servo_set(1, x_next);
//    servo_set(2, y_next);


    //FIR POST


    //Sample ADC
    //adc_result_1 = adc_sample(0, false); //Sample ADCRESULT0, start conversion

    //Convert adc to degrees
    //y_fit(&adc_result_1, &joint_1_deg, ADC_MIN, ADC_MAX, SERVO_MIN, SERVO_MAX);

    //Remove this, only temporary for testing
    //uart_tx_char('r');
}

void hwi_sample_isr(void) {

    //Always take UART tranmissions anyway
    uart_tx_char('r');

    //Always take ADC samples of rht epots
    adc_1 = adc_sample(1, true); // Start SOC and sample
    adc_2 = adc_sample(2, true); // Start SOC and sample

    adc_0 = adc_sample(0, true);
    y_fit(&adc_0, &fir_N, ADC_MIN, ADC_MAX, N_MIN, N_MAX);
}

void read_adc_2(void) {
}

void read_adc_3(void) {
}

void swi_fir_isr(void) {

}

void swi_sample_isr(void) {

}

void decode_xyz_task(void) {

    //If UART sample is selected, parse buffer if there's new data
    if(sample_select == UART_SAMPLE && buffer_string_ready==1) {

        //Get rid of ready flag
        buffer_string_ready=0;

        //Dump into completed string to be parsed
        strcpy(completed_string, buffer_string);


        //Reset SCI
        EALLOW;
        SciaRegs.SCICTL1.bit.SWRESET=1; // Reset SCI
        EDIS;

        //Post for parsing
        parse_rx(completed_string, &x_next, &y_next, &z);

    }

    //If ADC sample is selected, parse buffer with latest pot values
    else {
        y_fit(&adc_1, &x_next, ADC_MIN, ADC_MAX, SERVO_MIN, SERVO_MAX);
        y_fit(&adc_2, &y_next, ADC_MIN, ADC_MAX, SERVO_MIN, SERVO_MAX);
    }

}

void ikine_task(void) {
    //


    //Dump values into joint degrees

    //joint_1_deg = x_next;
    //joint_2_deg = y_next;

}

void temp_hwi(void) {
    c2000_temp = temp_sample(true); // Clear SOC and sample temp

    //convert to Q15 and express it in system_printf (later used for lcd)
    real_temp = (float) c2000_temp/32768 + (float) (c2000_temp%32768)/32768/2;
}


/*------ FIRS ------- */ /*
void fir_x_isr(void) {
    moving_average(&x_array, &x, fir_N, x_counter);

    if(x_counter==0) x_counter=FIR_INPUT_SIZE-1; //Rollover
    else x_counter--;
}

void fir_y_isr(void) {
    moving_average(&y_array, &y, fir_N, y_counter);

    if(y_counter==0) y_counter=FIR_INPUT_SIZE-1; //Rollover
    else y_counter--;
}
*/


/*----- SET SERVO PWMS -----*/


/*----- PWM TIMER -------*/





/*----- DEBOUNCE BUTTON AND LISTEN ------*/






/*------ TOOL TIP -------------*/


/*------- UART ------------*/


void uart_isr(void) {
    //BLOCK buffer_string_ready being used???

    // Take care of transmission
    uart_rx(&buffer_string, &buffer_string_ready);
}



