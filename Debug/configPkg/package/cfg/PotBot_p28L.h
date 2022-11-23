/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

#include <xdc/std.h>

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle UART_ISR;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle EPWM1_ISR;

#include <ti/sysbios/family/c28/Hwi.h>
extern const ti_sysbios_family_c28_Hwi_Handle EPWM2_ISR;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle sem_uart_received;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle sem_fir_ready;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle sem_ikine_ready;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle tsk_parse_rx;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle sem_adc_done;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle sem_parse_done;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle swi_uart_rx;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle swi_uart_tx;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

extern int xdc_rov_runtime_Mon__checksum;

extern int xdc_rov_runtime_Mon__write;

