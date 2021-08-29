#include "pid.h"

PID::PID(float p, float i, float d, float max) {
    this->p_gain = p;
    this->i_gain = i;
    this->d_gain = d;
    this->max = max;
}

float PID::calculate() {
    error = input - setpoint;
    i_mem += i_gain * error;
    if (i_mem > max) i_mem = max;
    else if (i_mem < max * -1) i_mem = max * -1;
    
    output = p_gain * error + i_mem + d_gain * (error - last_error);
    if (output > max) output = max;
    else if (output < max * -1) output = max * -1;

    last_error = error;

    return output;
}

void PID::reset() {
    i_mem = 0;
    last_error = 0;
}