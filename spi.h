/*
 * spi.h
 *
 *  Created on: Nov. 26, 2022
 *      Author: jbate
 */

#ifndef SPI_H_
#define SPI_H_

#ifndef TESTING_DEF
#include "Headers/F2802x_device.h"
#else
#include "Peripheral_Headers/F2802x_device.h"
#endif

//Masks for ORing when sending or ANDing when receiving
#define X_MASK      0x4000
#define Y_MASK      0x2000
#define Q1_MASK     0x1000
#define Q2_MASK     0x0800
#define TEMP_MASK   0x0400
#define N_MASK      0x0200

enum {
    X_PARAM,
    Y_PARAM,
    Q1_PARAM,
    Q2_PARAM,
    TEMP_PARAM,
    N_PARAM
};

//*********** spi_init ***********//
//
// Initializes spi on c2000 to talk to other c2000
//
// Arguments:
// None
//
// Return: none
//*******************************//
void spi_init();

//******* spi_send_int ***********//
//
// Sends int to spi
//
// Arguments:
// int16_t tx_int - int of xy coordinate being sent
// int16_t parameter - int of parameter being used (X,Y,Q1,Q2,temp,N,etc)
//
// Return: 1 for successful transmission, 0 for non successful transmission
//*******************************//
int spi_send_int(int16_t tx_int, int16_t parameter);

#endif /* SPI_H_ */
