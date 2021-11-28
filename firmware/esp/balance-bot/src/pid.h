#ifndef _PID_H
#define _PID_H

typedef struct {
    double windup_guard;
    double proportional_gain;
    double integral_gain;
    double derivative_gain;
    double prev_error;
    double int_error;
    double control;
    double control_max;
} PID;
 
void pid_zeroize(PID* pid, double c_max);
void pid_update(PID* pid, double curr_error, double dt);
    
#endif