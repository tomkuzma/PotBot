/*
 * spi.h
 *
 *  Created on: Nov. 7, 2022
 *      Author: Jimmy Bates
 *
 * Takes care of SPI transfer to other C2000
 * Has functionality for init and sending a string
 */

#include "spi.h"

//*********** spi_init ***********//
//
// Initializes spi on c2000 to talk to other c2000
//
// Arguments:
// None
//
// Return: none
//*******************************//
void spi_init()
{
    //Start of SPI bus
    EALLOW;

    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;   // SPI-A


    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16(SPISIMOA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;    // Configure GPIO16 as SPISIMOA

    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17(SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;    // Configure GPIO17 as SPISOMIA

    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18(SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;    // Configure GPIO18 as SPICLKA

    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19(SPISTEA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1;    // Configure GPIO19 as SPISTEA

    SpiaRegs.SPICCR.bit.SPISWRESET = 0; //Need this to be 0 to set SPI regs - Initializes SPI

    SpiaRegs.SPICCR.bit.SPICHAR = 0xF; //Set word length to 16 bits

    // Output change on rising, input latch on falling
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;

    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1; // Set up as master
    SpiaRegs.SPICTL.bit.SPIINTENA = 0; // Interrupt disabled (don't need it!)
    SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0; //Overrun int disabled


    SpiaRegs.SPIFFRX.all = 0x0000; //BYE BYE
    SpiaRegs.SPIFFTX.all = 0xE000; //Enable FIFO TX, reset it, nothing else
    SpiaRegs.SPIFFCT.all = 0x0000; //Don't need this gah-bage!

    SpiaRegs.SPICTL.bit.TALK=1; //Enable transmit

    SpiaRegs.SPIBRR = 0x4; // Make SPI speed 3.75MHz (LPCLK/4)
    SpiaRegs.SPIPRI.bit.FREE = 1; // So breakpoints don't halt SPI

    //SpiaRegs.SPICCR.bit.SPILBK=1;

    SpiaRegs.SPICCR.bit.SPISWRESET = 1; //Set this to 1 to enable SPI now that reg's are set
    EDIS;
}

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
int spi_send_int(int16_t tx_int, int16_t parameter)
{
    int tx_int_with_mask;

    //Use the most significant digits to communicate which parameter it is
    switch(parameter)
    {
    case X_PARAM:
        tx_int_with_mask = tx_int | X_MASK;
        break;
    case Y_PARAM:
        tx_int_with_mask = tx_int | Y_MASK;
        break;
    case Q1_PARAM:
        tx_int_with_mask = tx_int | Q1_MASK;
        break;
    case Q2_PARAM:
        tx_int_with_mask = tx_int | Q2_MASK;
        break;
    default:
        break;
    }

    //Put masked integer into buffer
    SpiaRegs.SPITXBUF = tx_int_with_mask;
}


