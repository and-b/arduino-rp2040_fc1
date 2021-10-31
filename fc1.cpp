#include "Arduino.h"
#include "fc1.h"

#include "imu.h"
#include "motor.h"
#include "rc.h"
#include "telemetry.h"
#include "pid.h"

// global variables
float gyro_pitch, gyro_roll, gyro_yaw = 0;
float angle_pitch, angle_roll = 0;
int motorFrontLeftCW, motorFrontRightCCW, motorRearRightCW, motorRearLeftCCW = STOP_PULSE;
uint16_t chPitch, chRoll, chThrottle, chYaw;

// variables for the flight controller 
unsigned long loop_timer;
int throttle;
int pulse;
uint16_t loopTime;
uint8_t fcError;
uint8_t start;
float roll_level_adjust, pitch_level_adjust;
float accel_angle_pitch_calib = -1.35;
float accel_angle_roll_calib = -1.13;

// PID pidRoll(1.3, 0.0, 18.0, 400); primo volo, oscilla molto e velocemente ma resta fermo in aria
// PID pidRoll(1.1, 0.0, 18.0, 400); secondo volo, meglio, ma dopo le manovre trema, proviamo ad alzare D
// PID pidRoll(1.1, 0.0, 21.0, 400); peggio, i tremori sembrano maggiori, provo ad abbassare D
// PID pidRoll(1.1, 0.0, 12.0, 400); i tremori sembrano diminuiti, abbasso ancora D
PID pidRoll(1.1, 0.0, 10.0, 400);
PID pidPitch(1.1, 0.0, 10.0, 400);
PID pidYaw(3.5, 0.01, 0.0, 400);

void setup() {
    Serial.begin(9600);
    Serial1.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    
    setupMotor();   

    setupImu();
    calibrateGyro();

    loop_timer = micros();
}

void loop() {
    // 1. read IMU
    readImu(true);
    calculateAttitude();
    
    // 2. prepare PID inputs
    //65.5 = 1 deg/sec 
    pidRoll.input = (pidRoll.input * 0.7) + ((gyro_roll / 65.5) * 0.3);
    pidPitch.input = (pidPitch.input * 0.7) + ((gyro_pitch / 65.5) * 0.3);
    pidYaw.input = (pidYaw.input * 0.7) + ((gyro_yaw / 65.5) * 0.3);

    pitch_level_adjust = angle_pitch * 15;
    roll_level_adjust = angle_roll * 15;

    // 3. handle sequences to arm / disarm
    if (chThrottle < 1050 && chYaw > 1950 && chPitch > 990 && chRoll > 990) start = 1;

    if (start == 1 && chThrottle < 1050 && chYaw < 1550) {
        start = 2;

        angle_pitch = accel_angle_pitch;
        angle_roll = accel_angle_roll;
        
        //Reset the PID controllers
        pidRoll.reset();
        pidPitch.reset();
        pidYaw.reset();
    
        // led on
        digitalWrite(LED_PIN, HIGH);
    }

    if (start == 2 && chThrottle < 1050 && chYaw < 1050) {
        start = 0;
        // led off
        digitalWrite(LED_PIN, LOW);
    }

    // 4. handle RC input and calculate PID setpoints
    pidRoll.setpoint = 0;
    if (chRoll > 1508) pidRoll.setpoint = chRoll - 1508;
    else if(chRoll < 1492) pidRoll.setpoint = chRoll - 1492;
    pidRoll.setpoint -= roll_level_adjust;
    pidRoll.setpoint /= 3.0;
    
    pidPitch.setpoint = 0;
    if(chPitch > 1508) pidPitch.setpoint = chPitch - 1508;
    else if(chPitch < 1492) pidPitch.setpoint = chPitch - 1492;
    pidPitch.setpoint = -pidPitch.setpoint;
    pidPitch.setpoint -= pitch_level_adjust;
    pidPitch.setpoint /= 3.0;
    
    pidYaw.setpoint = 0;
    if (chThrottle > 1050) { 
        if (chYaw > 1508) pidYaw.setpoint = (chYaw - 1508)/3.0;
        else if (chYaw < 1492) pidYaw.setpoint = (chYaw - 1492)/3.0;
    }

    // 5. calculate PID outputs
    pidRoll.calculate();
    pidPitch.calculate();
    pidYaw.calculate();

    throttle = chThrottle;

    // 6. calculate motor pulses
    if (start == 2) {
        if (throttle > MAX_THROTTLE) throttle = MAX_THROTTLE;
    
        // motor map: 
        // - nose up is positive pitch, nose down is negative pitch
        // - right wing down is positive roll, left wing down is negative roll
        // - rotating right is positive yaw, rotating left is negative yaw
        // when gyro detects a positive roll, the copter is going with the right wing down, so to compensate we need to increase speed for the motors on the right side
        // when gyro detects a positive pitch, the copter is going nose up, so to compensate we need to increase speed for the motor on the rear side
        // when gyro detects a positive yaw, the copter is rotating right, so to compensate we need rotate left. To turn anticlockwise, the clockwise motors motors speed up and the anticlockwise motors slow down
        motorFrontRightCCW = throttle - pidPitch.output + pidRoll.output - pidYaw.output;
        motorRearRightCW = throttle + pidPitch.output + pidRoll.output + pidYaw.output;
        motorRearLeftCCW = throttle + pidPitch.output - pidRoll.output - pidYaw.output;
        motorFrontLeftCW = throttle - pidPitch.output - pidRoll.output + pidYaw.output;

        // set mininum and maximum pulse for the motors
        motorRearLeftCCW = constrain(motorRearLeftCCW, MIN_PULSE, MAX_PULSE);
        motorFrontLeftCW = constrain(motorFrontLeftCW, MIN_PULSE, MAX_PULSE);
        motorFrontRightCCW = constrain(motorFrontRightCCW, MIN_PULSE, MAX_PULSE);
        motorRearRightCW = constrain(motorRearRightCW, MIN_PULSE, MAX_PULSE);
    }
    else {
        // disarmed, stop the motors
        motorFrontRightCCW = STOP_PULSE;
        motorRearRightCW = STOP_PULSE;
        motorRearLeftCCW = STOP_PULSE;
        motorFrontLeftCW = STOP_PULSE;
    }

    sendToESCs();
    sendTelemetry();
    decodeIBus();

    //The refresh rate is 250Hz.
    loopTime = micros() - loop_timer;
    while (micros() - loop_timer < 4000);
    loop_timer = micros();
}
