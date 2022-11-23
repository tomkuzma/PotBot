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
#define SERVO_1 0 // Channel for servo 1
#define SERVO_2 1 // Channel for servo 2
#define SERVO_3 2 // Channel for servo 3

#define ADC_COUNT 3 // amount of adcs
#define ADC_POT_N 0 // pot for adc channel 0 -> FIR moving average terms/N
#define ADC_POT_X 1 // pot for adc channel 1 -> x_next
#define ADC_POT_Y 2 // pot for adc channel 2 -> y_next

#define X_POS_MIN 30; // make sure it don't collide
#define Y_POS_MIN 30; // make sure it don't collide
#define X_POS_MAX 300; // make sure it don't collide
#define Y_POS_MAX 300; // make sure it don't collide


int asdf_test;
int adsf_test_2;

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
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <xdc/runtime/SysStd.h>  // Going to see if this library works better for printf

#include <Headers/F2802x_device.h>

//function prototypes:

extern void DeviceInit(void);


// HWI Notes:
// HWI sources will be:
//
//  epwm 1 period reset
//  epwm 2 period reset
//  uart receive
//
//
//  do we need spi rx?
//
//
//
//

//SWI func prototypes
extern const Swi_Handle swi_servo; // Pends resources and sets servos
extern const Swi_Handle swi_adc; // Start SOCs and take vals
extern const Swi_Handle swi_uart_tx; // Send UART transmission
extern const Swi_Handle swi_uart_rx; // Receive UART buffer

//task func prototypes
extern const Task_Handle tsk_parse_rx; // Parses UART reception when ready
extern const Task_Handle tsk_fir; // Force between boundaries, Shift vals, moves pointers, performs FIR
extern const Task_Handle tsk_ikine; // Performs ikine when resources available,
extern const Task_Handle tsk_spi;   // Sends SPI values to other C2000
extern const Task_Handle tsk_select_source; // Samples button to see if on and

//semaphores for tasks
extern const Semaphore_Handle sem_uart_received; //Gets posted when UART buffer is full
extern const Semaphore_Handle sem_parse_done; // Gets posted when String has been parsed
extern const Semaphore_Handle sem_adc_done; // Gets posted when ADC vals have been received
extern const Semaphore_Handle sem_fir_ready; //Gets posted when new values are inserted into array
extern const Semaphore_Handle sem_ikine_ready; //Gets posted when FIR is done

//FIR and ikine
int16_t x, y, z; // current values for x,y,z
int16_t x_adc_next, y_adc_next, z_adc_next; // adc next values for x,y,z (gets put into x array and y array
int16_t x_uart_next, y_uart_next, z_uart_next; // uart next values for x,y,z (gets put into x array and y array
int16_t x_array[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // x array for inputs
int16_t y_array[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // y array for inputs
int16_t fir_N;          //Length of array
int16_t x_counter, y_counter;    // Point of array where FIR is calculated from

//UART module
char completed_string[UART_BUFF_SIZE]; // after uart transmission, store here
char buffer_string[UART_BUFF_SIZE]; // puts uart rx into a string form
int buffer_index; // keeps track of parsed string position
int buffer_string_ready; // ready to be parsed

//Joint servo positions
int16_t joint_1;
int16_t joint_2;

//Temperature regs
int32 c2000_temp;
float real_temp;

//Select source for sampling (uart or pots)
int sample_select;

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
    sample_select = UART_SAMPLE;

    //Initialize SERVOs
    float dc_min[8] = { 0.018, 0.018, 0.018, 0,0,0,0,0}; // min duty cycle of servos
    float dc_max[8] = { 0.118, 0.118, 0,0,0,0,0 }; // max duty cycle of servos
    servo_init(SERVO_COUNT, dc_min, dc_max); // initialize 2 servos

    //Initialize ADCs
    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on

    //Turn EPWM Interrupts ON
    enable_epwm_interrupts(SERVO_COUNT);

    //jump to RTOS (does not return):
    BIOS_start();
    return(0);
}


void hwi_epwm_1_isr(void) {
    //Turn EPWM 2 interrupt flag off
    EPwm1Regs.ETCLR.bit.INT = 1;

    //Set servos
    servo_set(SERVO_1, joint_1);
    servo_set(SERVO_2, joint_2);

    //Post request for Pi camera position
    Swi_post(swi_uart_tx);
}

void hwi_epwm_2_isr(void) {
    //Turn EPWM 2 interrupt flag off
    EPwm2Regs.ETCLR.bit.INT = 1;
}

void hwi_uart_rx_isr(void) {
    //Post SWI for receiving buffer (pi cam position)
    Swi_post(swi_uart_rx);

}

/******** swi_uart_tx_isr *******
 *
 * Priority 12
 * Sends transmission to pi requesting xyz coordinates
 *
 */
void swi_uart_tx_isr(void) {
    //BLOCK buffer_string_ready being used???
    uart_tx_char('r');

}

/******** swi_uart_rx_isr *******
 *
 * Priority 13
 * Receives tranmission of FIFO uart rx buffer holding xyz coordinates from pi cam
 *
 */
void swi_uart_rx_isr(void) {    //Turn uart interrupt off
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; // Clear int flag for fifo

    // Take care of transmission
    uart_rx(&buffer_string, &buffer_string_ready);

    //If buffer ready, post task for parsing
    if(buffer_string_ready==1) Semaphore_post(sem_uart_received);

    //Clear buffer string ready flag
    buffer_string_ready=0;
}

/****** tsk_parse_rx_isr ********
 *
 * When the buffer string is full, the semaphore will be posted
 * Then this function will parse through the string
 *
 */
void tsk_parse_rx_isr(void) {
    //Infinite loop
    while(1) {
        //Complete function only when uart buffer is received
        Semaphore_pend(sem_uart_received, BIOS_WAIT_FOREVER);

        //Dump into completed string to be parsed
        strcpy(completed_string, buffer_string);

        //Parse string and dump results into next x,y,z values for uart
        parse_rx(completed_string, &x_uart_next, &y_uart_next, &z_uart_next);

        //Reset SCI
        EALLOW;
        SciaRegs.SCICTL1.bit.SWRESET=1; // Reset SCI
        EDIS;

        //Once completed, post semaphore for storing values in buffer
        Semaphore_post(sem_parse_done);
    }
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






/*------------- IDLE ------------
 * Idle thread - keeps track of temperature *
 */
void idle(void) {
    c2000_temp = temp_sample(true); // Clear SOC and sample temp

    //convert to Q15 and express it in system_printf (later used for lcd)
    real_temp = (float) c2000_temp/32768 + (float) (c2000_temp%32768)/32768/2;
}
