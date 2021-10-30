#ifndef _PID_H_
#define _PID_H_

class PID {
    public:
    float setpoint;
    float input;
    float error;
    float last_error;
    float output;
    float i_mem;
    float i_gain;
    float p_gain;
    float d_gain;
    float max;

    PID(float p, float i, float d, float max);
    float calculate();
    void reset();
};

#endif