/*
 * qei.h
 *
 *  Created on: Jul 29, 2017
 *      Author: VuHuyHop
 */

#ifndef ENCODER_QEI_H_
#define ENCODER_QEI_H_
#include <stdbool.h>
#include <stdint.h>
#define MOTOR_SELECT_LEFT    1
#define MOTOR_SELECT_RIGHT   0
void qei_init(uint16_t ms_Timebase);
bool qei_getVelocity(bool Select, int32_t *Velocity);
int32_t qei_getPosLeft();
int32_t qei_getPosRight();
void qei_setPosLeft(int32_t pos);
void qei_setPosRight(int32_t pos);

#endif /* ENCODER_QEI_H_ */
