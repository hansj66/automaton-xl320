// Nicked this code from https://nicisdigital.wordpress.com/2011/06/27/proportional-integral-derivative-pid-controller/

#include "PID.h"

void pid_zeroize(PID* pid, double c_max) {
    // set prev and integrated error to zero
    pid->prev_error = 0;
    pid->int_error = 0;
    pid->control_max = c_max;
}
 
void pid_update(PID* pid, double curr_error, double dt) 
{
    double diff;
    double p_term;
    double i_term;
    double d_term;
 
    // integration with windup guarding
    pid->int_error += (curr_error * dt);
    if (pid->int_error < -(pid->windup_guard))
        pid->int_error = -(pid->windup_guard);
    else if (pid->int_error > pid->windup_guard)
        pid->int_error = pid->windup_guard;
 
    // differentiation
    diff = ((curr_error - pid->prev_error) / dt);
 
    // scaling
    p_term = (pid->proportional_gain * curr_error);
    i_term = (pid->integral_gain     * pid->int_error);
    d_term = (pid->derivative_gain   * diff);
 
    // summation of terms
    pid->control = p_term + i_term + d_term;
    if (pid->control > pid->control_max)
        pid->control = pid->control_max;
 
    // save current error as previous error for next iteration
    pid->prev_error = curr_error;
}