/*
 * speed.h
 *
 *  Created on: Aug 16, 2017
 *      Author: VuHuyHop
 */

#ifndef SPEED_CTL_SPEED_H_
#define SPEED_CTL_SPEED_H_

#define PWM_MOTOR_LEFT          PWM0_BASE, PWM_OUT_0
#define PWM_MOTOR_RIGHT         PWM0_BASE, PWM_OUT_1
#define PWM_FREQUENCY       20000
#define DRV_ENABLE_LEFT_CHN_PERIPHERAL      SYSCTL_PERIPH_GPIOB
#define DRV_ENABLE_RIGHT_CHN_PERIPHERAL     SYSCTL_PERIPH_GPIOB
#define DRV_ENABLE_LEFT_CHN_PORT            GPIO_PORTB_BASE
#define DRV_ENABLE_RIGHT_CHN_PORT           GPIO_PORTB_BASE
#define DRV_ENABLE_LEFT_CHN_PIN             GPIO_PIN_4
#define DRV_ENABLE_RIGHT_CHN_PIN            GPIO_PIN_5

extern void speed_control_init();
extern void speed_enable_Hbridge(bool Enable);
extern void setPWM(uint32_t ulBaseAddr, uint32_t PinName, int32_t ucDutyCycle);

#endif /* SPEED_CTL_SPEED_H_ */
