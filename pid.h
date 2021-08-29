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

/*
//Roll calculations
pid_error_temp = gyro_roll_input - pid_roll_setpoint;
pid_i_mem_roll += pid_i_gain_roll * pid_error_temp;
if(pid_i_mem_roll > pid_max_roll)pid_i_mem_roll = pid_max_roll;
else if(pid_i_mem_roll < pid_max_roll * -1)pid_i_mem_roll = pid_max_roll * -1;

pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll + pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
if(pid_output_roll > pid_max_roll)pid_output_roll = pid_max_roll;
else if(pid_output_roll < pid_max_roll * -1)pid_output_roll = pid_max_roll * -1;

pid_last_roll_d_error = pid_error_temp;
*/