/*
 * Timer.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Dang Thanh Tung
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "Timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.c"
#include "driverlib/timer.h"

#define TIMER_PERIOD_MS 1
#define MAX_TIMEOUT_EVENT 10

typedef struct
{
    TIMER_CALLBACK_FUNC callback;
    uint64_t period_counter;
} TIMEOUT_EVENT;
static TIMEOUT_EVENT timer_event_list[MAX_TIMEOUT_EVENT];

void TIMER_ISR(void);   //prototype

void Timer_Init (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER);
    TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC); //set full-width periodic timer
    TimerLoadSet(TIMER_BASE, TIMER_A, SysCtlClockGet() * TIMER_PERIOD_MS/ 1000);   //Interval: TIMER_PERIOD_MS (ms), 1/ms
    TimerIntRegister(TIMER_BASE, TIMER_A, &TIMER_ISR);  //Register Interrupt handler
    IntEnable(INT_TIMERA);  //Enable Interrupt
    TimerIntEnable(TIMER_BASE, TIMER_TIMA_TIMEOUT); //Enable Interrupt when timeout
    TimerIntClear(TIMER_BASE, TIMER_TIMA_TIMEOUT);  //Clear Interrupt flag
    TimerControlStall(TIMER_BASE, TIMER_A, false);  //Timer keeps running while in debug mode (if true stop)
    TimerEnable(TIMER_BASE, TIMER_A);   //Start timer
}

TIMER_ID Timer_Register_Event (TIMER_CALLBACK_FUNC callback, uint64_t ms)
{
    int i;
    for(i=0; i< MAX_TIMEOUT_EVENT; i++)
    {
      if((timer_event_list[i].period_counter  == 0) && (timer_event_list[i].callback == NULL)) break;
    }
    if(i == MAX_TIMEOUT_EVENT)
        return INVALID_TIMER_ID;

    timer_event_list[i].period_counter = (uint64_t)(ms/TIMER_PERIOD_MS);
    timer_event_list[i].callback = callback;

    return (TIMER_ID)i;
}

bool Timer_Unregister_Event(TIMER_ID timer_id)
{
    bool status = false;
    if(timer_id < MAX_TIMEOUT_EVENT)
    {
        timer_event_list[timer_id].period_counter = 0;
        timer_event_list[timer_id].callback = NULL;
        status = true;
    }
    return status;
}

void TIMER_ISR(void)
{
    int i;
    TimerIntClear(TIMER_BASE, TIMER_TIMA_TIMEOUT);  //clear interrupt flag
    for(i=0; i<MAX_TIMEOUT_EVENT; i++)
    {
        if(timer_event_list[i].period_counter > 0)
        {
            timer_event_list[i].period_counter--;
            if(timer_event_list[i].period_counter == 0 && timer_event_list[i].callback != NULL)
            {
                (timer_event_list[i].callback)();
                /*
                 * Only clear timeout callback when period equal to 0
                 * Another callback could be register in current timeout callback
                 */
                if (timer_event_list[i].period_counter == 0)
                    timer_event_list[i].callback = NULL;
            }
        }
    }

}

