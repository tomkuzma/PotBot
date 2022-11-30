/********** PotBot main RTOS file******
*
* Authored by Jimmy Bates
* November 30th 2022
*
* main.c
* Takes care of all the RTOS stuff, mainly putting ISRs for the
* SWIs, TSKs, and HWIs, and the IDLE
*
* Initializes all modules
* SPI, UART, ADC, Servo (PWM), DSP
*
***************************************/

#define xdc__strict //suppress typedef warnings
#define COUNT_MAX 99 // Counts up to 99 and then resets to 0

#define SWI_PERIOD 3 // How many instances of the PWM before the swi gets called

#define SERVO_COUNT 4 // amount of servos
#define SERVO_1 2 // Channel for servo 1
#define SERVO_2 3 // Channel for servo 2
#define SERVO_Z 1 // Channel for servo 3

#define ADC_COUNT 3 // amount of adcs
#define ADC_POT_N 0 // pot for adc channel 0 -> FIR moving average terms/N
#define ADC_POT_X 2 // pot for adc channel 1 -> x_next
#define ADC_POT_Y 1 // pot for adc channel 2 -> y_next

#define X_POS_MIN 1 // make sure it don't collide
#define Y_POS_MIN 1 // make sure it don't collide
#define X_POS_MAX 300 // make sure it don't collide
#define Y_POS_MAX 300 // make sure it don't collide

enum {
    ADC_SAMPLE,
    UART_SAMPLE
}; // Sample sources

//includes:
#include "adc.h"
#include "servo.h"
#include "dsp.h"
#include "uart.h"
#include "spi.h"

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

//SWI func prototypes
extern const Swi_Handle swi_epwm_1; // Pends resources and sets servos, start SOCs
extern const Swi_Handle swi_epwm_2; // take vals, start SOCs
extern const Swi_Handle swi_uart_rx; // Receive UART buffer
extern const Swi_Handle swi_uart_reset; // Reset UART buffer if stuck

//task func prototypes
extern const Task_Handle tsk_uart_tx; // Sends tx for cam pi coordinates
extern const Task_Handle tsk_parse_rx; // Parses UART reception when ready
extern const Task_Handle tsk_spi;   // Sends SPI values to other C2000

//semaphores for tasks
extern const Semaphore_Handle sem_parse; //Gets posted when UART buffer is full
extern const Semaphore_Handle sem_spi; // Gets posted when String has been parsed and adc is done
extern const Semaphore_Handle sem_uart_tx; //Gets posted when servos have been set

//FIR and ikine
int16_t x, y, z; // current values for x,y,z
int16_t x_adc_next, y_adc_next, z_switch_next; // adc next values for x,y,z (gets put into x array and y array
int16_t x_uart_next, y_uart_next, z_uart_next; // uart next values for x,y,z (gets put into x array and y array
int16_t x_array[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // x array for inputs
int16_t y_array[] = {[0 ... FIR_INPUT_SIZE*2-1] = 0}; // y array for inputs
int16_t z_array[] = {[0 ... Z_FIR_INPUT_SIZE*2-1] = 0}; // z array for inputs
int16_t fir_N;          //Length of array
int16_t x_counter, y_counter, z_counter;    // Point of array where FIR is calculated from

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

int swi_counter=0;

/* ======== main ======== */
Int main()
{
    //initialization:
    DeviceInit(); //initialize processor
    uart_init(); //initialize sci as uart
    spi_init(); //Intialize all the spi stuff too

    //initialize global variables (clear the gah-bage)
    c2000_temp = 0;
    x = 300; // Starting position of xy
    y = 0; // Starting position of xy
    x_counter=FIR_INPUT_SIZE-1;
    y_counter=FIR_INPUT_SIZE-1;
    z_counter=Z_FIR_INPUT_SIZE-1;
    buffer_string_ready = 0;
    buffer_index = 0;
    fir_N = N_MIN;
    sample_select = UART_SAMPLE;

    //Clear UART buffer
    int j;
    for (j = 0; j<UART_BUFF_SIZE; j++) buffer_string[j] = NULL;

    //Initialize SERVOs
    float dc_min[8] = { 0, 0.252*2, 0.229*2, 0.048*2, 0,0,0,0 }; // min duty cycle of servos
    float dc_max[8] = { 0, 0.059*2, 0.071*2, 0.213*2, 0,0,0,0 }; // max duty cycle of servos

    servo_init(SERVO_COUNT, dc_min, dc_max); // initialize 2 servos
    led_pwm_init(); // Initializes PWM for temperature

    //Initialize ADCs
    adc_init(3,true); //Initialize 3 ADC channels, and turn temperature sensor on

    //Turn EPWM Interrupts ON
    enable_epwm_interrupts(SERVO_COUNT);

    //jump to RTOS (does not return):
    BIOS_start();
    return(0);
}


void hwi_epwm_1_isr(void)
{
    //Clear EPWM 1 flag
    EPwm1Regs.ETCLR.bit.INT=1;

    //Only call SWIs on every SWI_PERIOD intervals (probably 3 or 4)
    swi_counter++;
    if(swi_counter>SWI_PERIOD)
        swi_counter=0;

    //Post routine for epwm 1
    if(swi_counter==SWI_PERIOD-1)
        Swi_post(swi_epwm_1);
}

void hwi_epwm_2_isr(void)
{
    //Clear EPWM 2 flag
    EPwm2Regs.ETCLR.bit.INT=1;

    //Post routine for epwm 2
    if(swi_counter==SWI_PERIOD-1) {
        Swi_post(swi_epwm_2);
        Swi_post(swi_uart_reset);
    }
}

void hwi_uart_rx_isr(void)
{
    GpioDataRegs.GPBSET.bit.GPIO34=1;

    //Post SWI for receiving buffer (pi cam position)
    Swi_post(swi_uart_rx);


    GpioDataRegs.GPBCLEAR.bit.GPIO34=1;

}

/******** swi_uart_reset *******
 *
 * Priority 15
 * Resets buffer if need be
 *
 */
void swi_uart_reset_isr(void) {
    //If UART buffer is full, and stays full, reset the buffer
    if(SciaRegs.SCIFFRX.bit.RXFFOVF == 1) {
        EALLOW;
        SciaRegs.SCICTL1.bit.SWRESET=0; // Reset SCI
        SciaRegs.SCICTL1.bit.SWRESET=1; // Reset SCI

        SciaRegs.SCIFFTX.bit.SCIRST = 0; // Reset SCI FFIO
        SciaRegs.SCIFFTX.bit.SCIRST = 1; // Reset SCI FFIO

        SciaRegs.SCIFFRX.bit.RXFIFORESET = 0; // Reset SCI FFIO
        SciaRegs.SCIFFRX.bit.RXFIFORESET = 1; // Reset SCI FFIO
        EDIS;
    }
}

/******** swi_epwm_1_isr *******
 *
 * Priority 14
 * Takes care of z-axis servo positioning
 *
 */
void swi_epwm_1_isr(void)
{
    //Z  next
    z=z_switch_next;

    //Assign a value to the servo
    int16_t joint_z;
    if(z == 0) servo_set(SERVO_Z, SERVO_MAX);
    else servo_set(SERVO_Z, SERVO_MIN);

    //Take ADC values (sequentially)
    int16_t adc_N, adc_X, adc_Y; //Adc result variables
    adc_Y = adc_sample(ADC_POT_Y, true);
    adc_X = adc_sample(ADC_POT_X, true);
    adc_N = adc_sample(ADC_POT_N, true);

    //Y-fit the ADC 0 sample into N
    y_fit(&adc_N, &fir_N, ADC_MIN, ADC_MAX, N_MIN, N_MAX);

    //Y-fit the ADC 1 and 2 samples into x_adc_next, and y_adc_next
    y_fit(&adc_X, &x_adc_next, ADC_MIN, ADC_MAX, X_POS_MIN, X_POS_MAX); // x next
    y_fit(&adc_Y, &y_adc_next, ADC_MIN, ADC_MAX, Y_POS_MIN, Y_POS_MAX); // y next

    //Post SPI sem
    Semaphore_post(sem_spi);

    //Post UART tx sem
    Semaphore_post(sem_uart_tx);
}

/******** swi_epwm_2_isr *******
 *
 * Priority 15
 * Takes care of xy
 * FIRs, Ikines, then sets Servos
 *
 */
void swi_epwm_2_isr(void)
{

    //Set servo 1 and 2 duty cycles
    servo_set(SERVO_1, joint_1);
    servo_set(SERVO_2, joint_2+SERVO_MIN); //-90 degrees to account for 90 deg offset being in joint 2 position brings you

    //Insert next x and y value into input array, if UART
    if(sample_select==ADC_SAMPLE) {
        //Store x if uart
        x_array[x_counter] = x_uart_next;
        x_array[x_counter+FIR_INPUT_SIZE] = x_uart_next;

        //Store y if uart
        y_array[y_counter] = y_uart_next;
        y_array[y_counter+FIR_INPUT_SIZE] = y_uart_next;
    }
    //Insert next x and y into input array, if ADC
    else {
        //Store x if adc
        x_array[x_counter] = x_adc_next;
        x_array[x_counter+FIR_INPUT_SIZE] = x_adc_next;

        //Store y if adc
        y_array[y_counter] = y_adc_next;
        y_array[y_counter+FIR_INPUT_SIZE] = y_adc_next;
    }

    //FIR x
    moving_average(&x_array, &x, fir_N, x_counter);

    //FIR y
    moving_average(&y_array, &y, fir_N, y_counter);

    //Ikine kinematics
    ikine_float(&joint_1, &joint_2, x, y);

    //Increment pointer for x FIR
    if(x_counter==0) x_counter=FIR_INPUT_SIZE-1; //Rollover
    else x_counter--;

    //Increment pointer for y FIR
    if(y_counter==0) y_counter=FIR_INPUT_SIZE-1; //Rollover
    else y_counter--;
}


/******** swi_uart_rx_isr *******
 *
 * Priority 13
 * Receives tranmission of FIFO uart rx buffer holding xyz coordinates from pi cam
 *
 */
void swi_uart_rx_isr(void)
{

    //Turn uart interrupt off
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; // Clear int flag for fifo

    // Take care of transmission
    uart_rx(&buffer_string, &buffer_string_ready);

    //If buffer ready, post task for parsing
    if(buffer_string_ready==1)
    {
        //Dump into completed string to be parsed
        strcpy(completed_string, buffer_string);

        //Clear buffer string ready flag
        buffer_string_ready=0;

        //Post for Parsing
        Semaphore_post(sem_parse);
    }
}

/****** tsk_parse_rx_isr ********
 *
 * When the buffer string is full, the semaphore will be posted
 * Then this function will parse through the string
 * Roughly 10us
 */
void tsk_parse_rx_isr(void)
{
    //Infinite loop
    while(1)
    {
        //Complete function only when uart buffer is received
        Semaphore_pend(sem_parse, BIOS_WAIT_FOREVER);

        //Parse string and dump results into next x,y,z values for uart
        parse_rx(completed_string, &x_uart_next, &y_uart_next, &z_uart_next);

        //Once completed, post semaphore for storing values in buffer
        Semaphore_post(sem_spi);
    }
}

/****** tsk_uart_tx_isr ********
 *
 * Send transmission for request of PI cam coordinates
 * Roughly 30us
 */
void tsk_uart_tx_isr(void)
{
    while(1)
    {
        GpioDataRegs.GPBSET.bit.GPIO34=1;
        GpioDataRegs.GPBCLEAR.bit.GPIO34=1;

        //Pend semaphore when epwm routine is done
        Semaphore_pend(sem_uart_tx, BIOS_WAIT_FOREVER);

        GpioDataRegs.GPBSET.bit.GPIO34=1;

        //Send TX
        uart_tx_char('r');

        GpioDataRegs.GPBCLEAR.bit.GPIO34=1;
    }
}

/****** tsk_spi_isr ********
 *
 * Send transmission to other c2000 coordinates and other info
 *
 */
void tsk_spi_isr(void)
{
    while(1)
    {

        //Pend SPI sem twice (epwm routine and uart tx sent)
        Semaphore_pend(sem_spi, BIOS_WAIT_FOREVER);
        Semaphore_pend(sem_spi, BIOS_WAIT_FOREVER);

        //Send SPI Buffer with array of vals
        spi_send_int(x, X_PARAM);
        spi_send_int(y, Y_PARAM);
        spi_send_int(joint_1/10, Q1_PARAM);
        spi_send_int(joint_2/10, Q2_PARAM);

    }
}


/*------------- IDLE ------------
 * Idle thread -git  keeps track of temperature *
 */
void idle(void)
{
    //Sample button for Z next
    sample_select = !GpioDataRegs.GPADAT.bit.GPIO6;

    //Sample button for source select
    z_switch_next = !GpioDataRegs.GPADAT.bit.GPIO7;

    //Get temperature sample
    c2000_temp = temp_sample(true); // Clear SOC and sample temp

    //make this bad boy celsius
    real_temp = (float) c2000_temp/32768 + (float) (c2000_temp%32768)/32768/2;

    //PWM set the LED
    led_pwm_set(real_temp);
}

