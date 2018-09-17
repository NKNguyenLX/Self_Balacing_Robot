/*
 * speed.c
 *
 *  Created on: Aug 16, 2017
 *      Author: VuHuyHop
 */
#include "../include.h"
#include "speed.h"
#include "Timer/Timer.h"
static uint32_t ui32Load;
static uint32_t ui32PWMClock;
static TIMER_ID speed_control_timID = INVALID_TIMER_ID;
static int32_t SetPoint[2] = {0, 0};
static int32_t RealSpeedSet[2] = {0, 0};
//private function
static void pwm_init(void);
void speed_control_init (void)
{
    pwm_init();
    setPWM(PWM_MOTOR_LEFT, 0);
    setPWM(PWM_MOTOR_RIGHT,0);
    speed_enable_Hbridge(true);
}
static void pwm_init(void)
{
        SysCtlPWMClockSet(SYSCTL_PWMDIV_1);             // PWM Clock = SystemCLK/1
        SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);        // PWM Channel: PB6,7

        // PWM Configuration
        ui32PWMClock = ROM_SysCtlClockGet();    // Calculate PWM clock
        ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

        // 2 PWM channels: PB6, PB7 (module 0, generator 0)
        GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
        GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);

        GPIOPinConfigure(GPIO_PB6_M0PWM0);
        GPIOPinConfigure(GPIO_PB7_M0PWM1);

        PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
        PWMPulseWidthSet(PWM_MOTOR_LEFT, 0);

        PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);

        PWMGenEnable(PWM0_BASE, PWM_GEN_0);

        //======================================//

        ROM_SysCtlPeripheralEnable(DRV_ENABLE_LEFT_CHN_PERIPHERAL);
        ROM_SysCtlPeripheralEnable(DRV_ENABLE_RIGHT_CHN_PERIPHERAL);
        ROM_GPIOPinTypeGPIOOutput(DRV_ENABLE_LEFT_CHN_PORT, DRV_ENABLE_LEFT_CHN_PIN);
        ROM_GPIOPinTypeGPIOOutput(DRV_ENABLE_RIGHT_CHN_PORT, DRV_ENABLE_RIGHT_CHN_PIN);
        ROM_GPIOPinWrite(DRV_ENABLE_LEFT_CHN_PORT, DRV_ENABLE_LEFT_CHN_PIN, 0);
        ROM_GPIOPinWrite(DRV_ENABLE_RIGHT_CHN_PORT, DRV_ENABLE_RIGHT_CHN_PIN, 0);}
void speed_enable_Hbridge(bool Enable)
{
    if (Enable)
        {
            ROM_GPIOPinWrite(DRV_ENABLE_LEFT_CHN_PORT, DRV_ENABLE_LEFT_CHN_PIN, DRV_ENABLE_LEFT_CHN_PIN);
            ROM_GPIOPinWrite(DRV_ENABLE_RIGHT_CHN_PORT, DRV_ENABLE_RIGHT_CHN_PIN, DRV_ENABLE_RIGHT_CHN_PIN);
        }
        else
        {
            ROM_GPIOPinWrite(DRV_ENABLE_LEFT_CHN_PORT, DRV_ENABLE_LEFT_CHN_PIN, 0);
            ROM_GPIOPinWrite(DRV_ENABLE_RIGHT_CHN_PORT, DRV_ENABLE_RIGHT_CHN_PIN, 0);
        }
}

void setPWM(uint32_t ulBaseAddr, uint32_t PinName, int32_t ucDutyCycle)
{
    if (ucDutyCycle > 90)
        ucDutyCycle = 90;
    else if (ucDutyCycle < -90)
        ucDutyCycle = -90;
    PWMPulseWidthSet(ulBaseAddr, PinName, (100 + ucDutyCycle) * ui32Load / 200);
}

static void speed_update_setpoint(void)
{
    int i;
    speed_control_timID = INVALID_TIMER_ID;

    for (i = 0; i < 2; i++)
    {
        if (RealSpeedSet[i] + 10 < SetPoint[i])
            RealSpeedSet[i] += 10;
        else if (RealSpeedSet[i] > SetPoint[i] + 10)
            RealSpeedSet[i] -= 10;
        else
            RealSpeedSet[i] = SetPoint[i];
    }

    speed_control_runtimeout(20);
}
void speed_control_runtimeout(uint32_t ms)
{
    speed_control_stoptimeout();
    speed_control_timID = TIMER_RegisterEvent(&speed_update_setpoint, ms);
}
static void speed_control_stoptimeout(void)
{
    if (speed_control_timID != INVALID_TIMER_ID)
        TIMER_UnregisterEvent(speed_control_timID);
    speed_control_timID = INVALID_TIMER_ID;
}
