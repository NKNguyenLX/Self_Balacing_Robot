/*
 * pid.h
 *
 *  Created on: Aug 15, 2017
 *      Author: VuHuyHop
 */

#ifndef PID_PID_H_
#define PID_PID_H_
typedef struct
{
    float Kp;
    float Kd;
    float Ki;
    float e;
    float e_;
    float e__;
    float u;
    float u_;
    float Ts;
    float PID_Saturation;
} PID_PARAMETERS;
extern void pid_set_k_parmas(PID_PARAMETERS* pid_parameter,float Kp,float Kd, float Ki);
extern void pid_reset_parmas(PID_PARAMETERS* pid_parameter);
extern float get_error(float angel, float set_point);
extern float pid_process(PID_PARAMETERS* pid_parameter, float error);

#endif /* PID_PID_H_ */
