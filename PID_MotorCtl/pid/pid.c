/*
 * pid.c
 *
 *  Created on: Aug 15, 2017
 *      Author: VuHuyHop
 */
#include "PID.h"
#include "../include.h"

// set parmas for PID controller
void pid_set_k_parmas(PID_PARAMETERS* pid_parameter,float Kp,float Kd, float Ki)
{
    pid_parameter->Kp= Kp;
    pid_parameter->Kd= Kd;
    pid_parameter->Ki= Ki;
}
void pid_reset_parmas(PID_PARAMETERS* pid_parameter)
{
    pid_parameter->e    = 0;
    pid_parameter->e_   = 0;
    pid_parameter->e__  = 0;
    pid_parameter->u    = 0;
    pid_parameter->u_   = 0;
}
// get errol
float get_error(float angel, float set_point)
{
    float error;
    error= angel - set_point;
    return error;
}
//caculate PID
float pid_process(PID_PARAMETERS* pid_parameter, float error)
{
    pid_parameter->e= error;
    pid_parameter->e_ = pid_parameter->e;
    pid_parameter->e__ =pid_parameter->e_;
    pid_parameter->u_ =pid_parameter->u;
    pid_parameter->u = pid_parameter->u_ + pid_parameter->Kp * (pid_parameter->e - pid_parameter->e_)
                + pid_parameter->Ki * pid_parameter->Ts * pid_parameter->e
                + (pid_parameter->Kd / pid_parameter->Ts) * (pid_parameter->e - (2 * pid_parameter->e_) + pid_parameter->e__);
    if (pid_parameter->u > pid_parameter->PID_Saturation)
        {
            pid_parameter->u = pid_parameter->PID_Saturation;
        }
        else if (pid_parameter->u < (-pid_parameter->PID_Saturation))
        {
            pid_parameter->u = -pid_parameter->PID_Saturation;
        }

        return pid_parameter->u;
}


