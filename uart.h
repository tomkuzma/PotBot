/*
 * uart.h
 *
 *  Created on: Nov. 16, 2022
 *      Author: Jimmy Bates and Tom Kuzma
 *
 * Takes care of setting uart up, and then
 * contains functions to use the uart -
 * namely, being able to receive from the pi
 * a message and then parse through it
 *
 *
 * More info:
 * Pins for this are:
 * GPIO28 (J1-3) - UART RX
 * GPIO29 (J1-4) - UART TX
 */


#ifndef UART_H_
#define UART_H_

#define RX_READY '1' //Ready for next transmission
#define UART_BUFF_SIZE 14 // Length of uart character array

#ifndef TESTING_DEF
#include "Headers/F2802x_device.h"
#else
#include "Peripheral_Headers/F2802x_device.h"
#endif

#include <string.h>

//****** uart_init ********//
//
// Initializes all regs needed for uart
//
// Return : None
//
//**************************//
void uart_init(void);

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
void uart_tx_char(char input);

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
void uart_tx_str(char *input);

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
void uart_rx(char **input_string, int *ready);

//****** uart_rx ********//
//
// Take the rx buffer and parse through it, converting the string into x y and z values
//
// Arguments:
// char * string - string that holds the completed UART transmitted buffer
// int16_t *x - pointer of x position
// int16_t *y - pointer of y position
// int16_t *z - pointer of z state (true or false)
//
// Return : None
//
//**************************//
int parse_rx(char * string, int16_t *x, int16_t *y, int16_t *z);

#endif /* UART_H_ */
