// Filename:            Lab2Idle_DeviceInit.c
//
// Description:	        Initialization code
//
// Version:             1.0
//
// Target:              TMS320F28027
//
// Author:              David Romalo
//
// Date:                11Oct2022
//Edited by Jimmy Bates
//Oct 23 2022
//Just added
//
//
// Edited by JB
// Nov 11 Rememberance day
// Basically added the entire functionality
#include <Headers/F2802x_device.h>

#pragma CODE_SECTION(InitFlash, "ramfuncs");

void DeviceInit(void);
void ISR_ILLEGAL(void);
void InitFlash(void);
void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);

//--------------------------------------------------------------------
//  Configure Device for target Application Here
//--------------------------------------------------------------------


void DeviceInit(void)
{
    //WDogDisable();    // Disable the watchdog initially
 //   DINT;           // Global Disable all Interrupts
 //   IER = 0x0000;   // Disable CPU interrupts
 //   IFR = 0x0000;   // Clear all CPU interrupt flags


// SYSTEM CLOCK speed based on internal oscillator = 10 MHz
// 0xC =  60    MHz     (12)
// 0xB =  55    MHz     (11)
// 0xA =  50    MHz     (10)
// 0x9 =  45    MHz     (9)
// 0x8 =  40    MHz     (8)
// 0x7 =  35    MHz     (7)
// 0x6 =  30    MHz     (6)
// 0x5 =  25    MHz     (5)
// 0x4 =  20    MHz     (4)
// 0x3 =  15    MHz     (3)
// 0x2 =  10    MHz     (2)


   EALLOW; // below registers are "protected", allow access.

// LOW SPEED CLOCKS prescale register settings
   //SysCtrlRegs.LOSPCP.all = 0x0002;       // Sysclk / 4 (15 MHz)
   //SysCtrlRegs.XCLK.bit.XCLKOUTDIV=2;

// PERIPHERAL CLOCK ENABLES
//---------------------------------------------------
// If you are not using a peripheral you may want to switch
// the clock off to save power, i.e. set to =0
//
// Note: not all peripherals are available on all 280x derivates.
// Refer to the datasheet for your particular device.

   SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;    // ADC
   //------------------------------------------------
   SysCtrlRegs.PCLKCR3.bit.COMP1ENCLK = 0;  // COMP1
   SysCtrlRegs.PCLKCR3.bit.COMP2ENCLK = 0;  // COMP2
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 0;   // I2C
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 0;   // SPI-A
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
   //------------------------------------------------
   SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 0;  //eCAP1
//------------------------------------------------


   //EPWM Clock 1 Setup
   /*
   EPwm1Regs.TBPRD = 37800;
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

   */

//--------------------------------------------------------------------------------------
// GPIO (GENERAL PURPOSE I/O) CONFIG
//--------------------------------------------------------------------------------------
//-----------------------
// QUICK NOTES on USAGE:
//-----------------------
// If GpioCtrlRegs.GP?MUX?bit.GPIO?= 1, 2 or 3 (i.e. Non GPIO func), then leave
//  rest of lines commented
// If GpioCtrlRegs.GP?MUX?bit.GPIO?= 0 (i.e. GPIO func), then:
//  1) uncomment GpioCtrlRegs.GP?DIR.bit.GPIO? = ? and choose pin to be IN or OUT
//  2) If IN, can leave next to lines commented
//  3) If OUT, uncomment line with ..GPACLEAR.. to force pin LOW or
//             uncomment line with ..GPASET.. to force pin HIGH or
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
 /*
//  GPIO-00 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;     // 0=GPIO,  1=EPWM1A,  2=Resv,  3=Resv
//  GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO0 = 1;      // uncomment if --> Set High initially
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
//--------------------------------------------------------------------------------------
//  GPIO-01 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;     // 0=GPIO,  1=EPWM1B,  2=EMU0,  3=COMP1OUT
//  GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO1 = 1;      // uncomment if --> Set High initially
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
//--------------------------------------------------------------------------------------
    //Changed this to be output = low at beginning //JB



//  GPIO-02 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;     // 0=GPIO,  1=EPWM2A,  2=Resv,  3=Resv
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;      // 1=OUTput,  0=INput
    GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO2 = 0;      // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-03 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;     // 0=GPIO,  1=EPWM2B,  2=Resv,  3=COMP2OUT
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 0;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO3 = 1;      // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-04 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;     // 0=GPIO,  1=EPWM3A, 2=Resv,   3=Resv
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 0;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO4 = 1;      // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-05 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;     // 0=GPIO,  1=EPWM3B,  2=Resv,  3=ECAP1
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 0;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO5 = 1;      // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-06 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;     // 0=GPIO,  1=EPWM4A,  2=SYNCI,  3=SYNCO
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 0;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO6 = 1;      // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-07 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;     // 0=GPIO,  1=EPWM4B,  2=SCIRX-A,  3=Resv
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 0;      // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;    // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO7 = 1;      // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-08 - GPIO-11 Do Not Exist
//--------------------------------------------------------------------------------------
 *
 *
 */

//  GPIO-12 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;    // 0=GPIO,  1=TZ1,  2=SCITX-A,  3=Resv
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO12 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-13 - GPIO-15 Do Not Exist
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

//  GPIO-16 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;    // 0=GPIO,  1=SPISIMO-A,  2=Resv,  3=TZ2
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO16 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-17 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;    // 0=GPIO,  1=SPISOMI-A,  2=Resv,  3=TZ3
    GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO17 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-18 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;    // 0=GPIO,  1=SPICLK-A,  2=SCITX-A,  3=XCLKOUT
    GpioCtrlRegs.GPADIR.bit.GPIO18 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO18 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-19 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;    // 0=GPIO,  1=SPISTE-A,  2=SCIRX-A,  3=ECAP1
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO19 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-20 - GPIO-27 Do Not Exist
//--------------------------------------------------------------------------------------
//  GPIO-28 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;    // 0=GPIO,  1=SCIRX-A,  2=I2C-SDA,  3=TZ2
//  GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO28 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-29 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;    // 0=GPIO,  1=SCITXD-A,  2=I2C-SCL,  3=TZ3
//  GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPASET.bit.GPIO29 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-30 - GPIO-31 Do Not Exist
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

//  GPIO-32 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;    // 0=GPIO,  1=I2C-SDA,  2=SYNCI,  3=ADCSOCA
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPBSET.bit.GPIO32 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-33 - PIN FUNCTION = --Spare--
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;    // 0=GPIO,  1=I2C-SCL,  2=SYNCO,  3=ADCSOCB
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 0;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;   // uncomment if --> Set Low initially
//  GpioDataRegs.GPBSET.bit.GPIO33 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-34 - PIN FUNCTION = LED for F28027 USB dongle
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;    // 0=GPIO,  1=COMP2OUT,  2=EMU1,  3=Resv
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;     // 1=OUTput,  0=INput
//  GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;   // uncomment if --> Set Low initially
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;     // uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
    EDIS;   // Disable register access
}



interrupt void ISR_ILLEGAL(void)   // Illegal operation TRAP
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm("          ESTOP0");
  for(;;);

}

// This function initializes the Flash Control registers

//                   CAUTION
// This function MUST be executed out of RAM. Executing it
// out of OTP/Flash will yield unpredictable results

void InitFlash(void)
{
   EALLOW;
   //Enable Flash Pipeline mode to improve performance
   //of code executed from Flash.
   FlashRegs.FOPT.bit.ENPIPE = 1;

   //                CAUTION
   //Minimum waitstates required for the flash operating
   //at a given CPU rate must be characterized by TI.
   //Refer to the datasheet for the latest information.

   //Set the Paged Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;

   //Set the Random Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;

   //Set the Waitstate for the OTP
   FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;

   //                CAUTION
   //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
   EDIS;

   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

   asm(" RPT #7 || NOP");
}


// This function will copy the specified memory contents from
// one location to another.
//
//  Uint16 *SourceAddr        Pointer to the first word to be moved
//                          SourceAddr < SourceEndAddr
//  Uint16* SourceEndAddr     Pointer to the last word to be moved
//  Uint16* DestAddr          Pointer to the first destination word
//
// No checks are made for invalid memory locations or that the
// end address is > then the first start address.

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

//===========================================================================
// End of file.
//===========================================================================




