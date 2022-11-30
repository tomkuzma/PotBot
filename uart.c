/*
 * uart.c
 *
 *  Created on: Nov. 16, 2022
 *      Author: Jimmy Bates
 *
 * Takes care of setting uart up, and then
 * contains functions to use the uart -
 * namely, being able to receive from the pi
 * a message and then parse through it
 *
 */

#include "uart.h"

uint16_t __uart_c_buffer_index; // index of uart array
char __uart_c_buffer_string[UART_BUFF_SIZE]; // character array to stuff vals in


//****** uart_init ********//
//
// Initializes all regs needed for uart
//
// Return : None
//
//**************************//
void uart_init(void)
{
    //initialize uart globals
    __uart_c_buffer_index=0;

    //UART SETUP START
    EALLOW;

    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;  // Enable SCI Clock
    SysCtrlRegs.LOSPCP.bit.LSPCLK = 0x02; // Sysclk (MIGHT NEED TO BE REMOVED IN RTOS VERSION)

    //UART RX
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;     // Enable Pull-up for GPO28
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;    // Enable UART RX
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;   // Set as asynchronous input

    //UART TX
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 1;     // Disable Pull-up for GPIO29
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;    // Enable UART TX

    //Enable FIFO
    SciaRegs.SCIFFTX.all=0xE040;
    SciaRegs.SCIFFRX.all=0x2061; // Interrupt enable, flags after 3 bits are in fifo 0010 0000 0110 0100
    SciaRegs.SCIFFCT.all=0x0;

    SciaRegs.SCICCR.all =0x0007;    //No parity bit, 1 stop bit

    //Set baud rate of 115200bps (with low clock speed of 15MHz)
    SciaRegs.SCIHBAUD = 0; // Not needed
    SciaRegs.SCILBAUD = 0x0007; // BRR = LSPCLK/(Baud*8) - 1

    //SciaRegs.SCICTL2.bit.RXBKINTENA = 1; // Enable SCI interrupt


    SciaRegs.SCICTL1.bit.TXENA = 1; // Enable TX
    SciaRegs.SCICTL1.bit.RXENA = 1; // Enable RX
    SciaRegs.SCICTL1.bit.SWRESET = 1; // Reset SCI

    EDIS;
    //END UART SETUP
}

//****** uart_tx_char ********//
//
// Sends a single character through uart
//
// Arguments:
// char input - single character being sent
//
// Return : None
//
//**************************//
void uart_tx_char(char input)
{
    //Wait until other characters have been sent
    while(SciaRegs.SCICTL2.bit.TXRDY == 0) { }
    SciaRegs.SCITXBUF=(uint16_t)input;
}

//****** uart_tx_str ********//
//
// Sends a single character through uart
//
// Arguments:
// char input - array of characters (string) being sent
//
// Return : None
//
//**************************//
void uart_tx_str(char *input)
{
    //Iterate through loop until end of character
    int i=0; //Create index
    while(input[i] != '\0') uart_tx_char(input[i++]);
}

//****** uart_rx ********//
//
// Receive uart rx buffer and dump result into variable
//
// Arguments:
// char **input_string - string to put buffer into
// int **buff_i - index of the buffer string
// bool_t *ready - associate this with the flag, string is ready to be parsed
// Return : None
//
//**************************//
void uart_rx(char **input_string, int *ready)
{
    //UART INTERRUPT ROUTINE
    strcpy(__uart_c_buffer_string, input_string);

    while(SciaRegs.SCIFFRX.bit.RXFFST != 0 && SciaRegs.SCIFFRX.bit.RXFFOVF == 0) {

        //Dump result into buffer, parse into string
        uint16_t buffer_int = SciaRegs.SCIRXBUF.all; //for dumping whole word into
        char buffer = (char) buffer_int; // just the char part

        //Ensure buffer string ready starts at 0
        *ready = 0;

        //Make decision of indexing based on character
        if(buffer=='x')
        {
            //Restart buffer index
            __uart_c_buffer_index=0;

            //Clear string
            int j;
            for(j=0; j<UART_BUFF_SIZE; j++) __uart_c_buffer_string[j]=NULL;

            __uart_c_buffer_string[__uart_c_buffer_index] = (char) buffer;
            __uart_c_buffer_index++; //increment buffer

        }
        else if(buffer=='T' || buffer=='F')
        {
            //Dump T or F in last variable
            __uart_c_buffer_string[__uart_c_buffer_index]= (char) buffer;
            __uart_c_buffer_index++;
            //End of uart buffer, put eol
            __uart_c_buffer_string[__uart_c_buffer_index]= '\0';

            //Signal flag that string can be dumped and processed
            *ready=1;
        }
        else
        {
            //dump buffer in appropriate spot in buffer string
            __uart_c_buffer_string[__uart_c_buffer_index] = (char) buffer;
            __uart_c_buffer_index++; //Increment buffer
        }
    }
    strcpy(input_string, __uart_c_buffer_string);

}



//****** parse_rx ********//
//
// Take the rx buffer and parse through it, converting the string into x y and z values
//
// Arguments:
// char * string - string that holds the completed UART transmitted buffer
// int16_t *x - pointer of x position
// int16_t *y - pointer of y position
// int16_t *z - pointer of z state (true or false)
//
// Return : -1 if error, 1 if successful
//
//**************************//
int parse_rx(char * string, int16_t *x, int16_t *y, int16_t *z) {
    int digs; // number of digits to process
    char* compare_str = "xyz"; // To compare the direction chars

    int temp_xyz[] = { 0,0,0 }; // To store digits temporarily before offloading to xyz ints

    int str_ind = 0; // overall string index
    int outer_ind; // For comparing chars to x, y, z
    int inner_ind; // For storing the digits in
    int scalar_ind; // For scaling from 1-100

    for (outer_ind = 0; outer_ind < 3; outer_ind++)
    {
       //If character bytes aren't x, y, z, return error
       if (string[str_ind++] != compare_str[outer_ind])
           return -1;

       //If X or Y
       if (outer_ind < 2)
       {
          //Store number of digits to look ahead
          digs = string[str_ind++] - 48;

          //Perform parsing of numbers
          for (inner_ind = 0; inner_ind < digs; inner_ind++)
          {
             //Get scalar
             int scalar = 1;
             for (scalar_ind = 0; scalar_ind < (digs - inner_ind - 1); scalar_ind++)
                 scalar = scalar * 10;

             //Times scalar by char digit and store to temp buffer
             temp_xyz[outer_ind] = temp_xyz[outer_ind] + (string[str_ind++] - 48) * scalar;
          }
       }
       //If Z
       else
       {
          //Store Z position in buffer - should be 0 if F or 1 if T
          if(string[str_ind] == 'T')
              temp_xyz[outer_ind] = 1;
          else if (string[str_ind] == 'F')
              temp_xyz[outer_ind] = 0;
          else
              return -1; //there's been an error
       }
    }

    //Store results in pointer args
    *x = temp_xyz[0];
    *y = temp_xyz[1];
    *z = temp_xyz[2];

    return 1; //Success!
}
