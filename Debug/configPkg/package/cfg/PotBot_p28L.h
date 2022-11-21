/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#include <xdc/std.h>

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle cpu_timer_0;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle Temp;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle UART_ISR;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle EPWM1_ISR;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle EPWM2_ISR;

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle cpu_timer_2;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle decode_xyz;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

extern int xdc_rov_runtime_Mon__checksum;

extern int xdc_rov_runtime_Mon__write;

