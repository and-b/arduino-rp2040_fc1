#ifndef _FC1_H_
#define _FC1_H_

// gyro rotatinal speed
extern float gyro_pitch, gyro_roll, gyro_yaw;
// attitude angles
extern float angle_pitch, angle_roll;
// accelerometer sensors
extern int16_t accel_x, accel_y, accel_z;
// accelerometer calibration
extern float accel_angle_pitch, accel_angle_roll;

// rc channel inputs
extern uint16_t chPitch, chRoll, chThrottle, chYaw;

// status
extern uint8_t start;
extern uint16_t loopTime; // loop time in us
extern uint8_t fcError;

// variable for controlling the ESCs
#define ESC_CALIB_HIGH 2000 // first pulse to send to start ESCs calibration
#define ESC_CALIB_LOW 1000 // pulse to send for ending calibration
#define MIN_PULSE 1100 // minimum pulse sent when copter is in armed state (flying)
#define MAX_PULSE 2000 // maximum pulse sent when copter is in armed state (flying)
#define STOP_PULSE 1000 // pulse to send when we want to stop the motor (disarmnig)
#define MAX_THROTTLE 1700 // max throttle before adding PID corrections (we need to leave headroom so the PID can compensate)

#define LED_PIN 13
// motors
extern int motorFrontLeftCW, motorFrontRightCCW, motorRearRightCW, motorRearLeftCCW;

#endif
