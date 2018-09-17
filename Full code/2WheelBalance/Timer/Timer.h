/*
 * Timer.h
 *
 *  Created on: Aug 28, 2017
 *      Author: Dang Thanh Tung
 */

#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_


#define SYSCTL_PERIPH_TIMER       SYSCTL_PERIPH_TIMER4
#define TIMER_BASE                TIMER4_BASE
#define INT_TIMERA                INT_TIMER4A

typedef void (*TIMER_CALLBACK_FUNC)();
typedef uint8_t TIMER_ID;

#define INVALID_TIMER_ID 0xff

void Timer_Init (void);
TIMER_ID Timer_Register_Event (TIMER_CALLBACK_FUNC callback, uint64_t ms);
bool Timer_Unregister_Event(TIMER_ID timer_id);



#endif /* TIMER_TIMER_H_ */
