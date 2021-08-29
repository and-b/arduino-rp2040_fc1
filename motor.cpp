#include <Arduino.h>
#include "hardware/pwm.h"
#include "fc1.h"
#include "motor.h"

void calibrateESCs();
void writeESCs(int micros);

void setupMotor() {
    // PWM 0A - pin D4 - GPIO16 - slice 0 - CHAN A
    // PWM 0B - pin D5 - GPIO17 - slice 0 - CHAN B
    // PWM 1A - pin D6 - GPIO18 - slice 1 - CHAN A
    // PWM 1B - pin D7 - GPIO19 - slice 1 - CHAN B
    gpio_set_function(16, GPIO_FUNC_PWM);
    gpio_set_function(17, GPIO_FUNC_PWM);
    gpio_set_function(18, GPIO_FUNC_PWM);
    gpio_set_function(19, GPIO_FUNC_PWM);
    
    // clock is 125Mhz, so 1 tick is 8 nanoseconds. I want a cycle of 1 micro, so the clock divider is 125
    pwm_set_clkdiv(0, 125);
    pwm_set_clkdiv(1, 125);
    
    // this is the maximum counter (16 bit 0-65535). set period of 2040 micros, 490.19 Hz
    pwm_set_wrap(0, 2039);
    pwm_set_wrap(1, 2039);
    
    pwm_set_chan_level(0, PWM_CHAN_A, STOP_PULSE); // GPIO16
    pwm_set_chan_level(0, PWM_CHAN_B, STOP_PULSE); // GPIO17
    pwm_set_chan_level(1, PWM_CHAN_A, STOP_PULSE); // GPIO18
    pwm_set_chan_level(1, PWM_CHAN_B, STOP_PULSE); // GPIO19
    
    pwm_set_enabled(0, true);
    pwm_set_enabled(1, true);

    #ifdef CALIBRATE_ESC
    calibrateESCs();
    #endif
}

void calibrateESCs() {
    Serial.println(F("ESC calibration, check that battery IS DISCONNECTED and press a key"));
    while(!Serial.available());
    Serial.read();
    writeESCs(ESC_CALIB_HIGH);
    Serial.println(F("MAX pulse sent, press a key after the ESC has emitted 3 beeps"));
    while(!Serial.available());
    Serial.read();
    writeESCs(ESC_CALIB_LOW);
    Serial.println(F("ESC calibrated"));
    delay(5000);
}

void writeESCs(int micros) {
    // send same pulse to all 4 ESCs
    motorFrontLeftCW = micros;
    motorFrontRightCCW = micros;
    motorRearRightCW = micros;
    motorRearLeftCCW = micros;

    sendToESCs();
}

void sendToESCs() {
    pwm_set_chan_level(0, PWM_CHAN_A, motorFrontLeftCW); // D4
    pwm_set_chan_level(0, PWM_CHAN_B, motorFrontRightCCW); // D5
    pwm_set_chan_level(1, PWM_CHAN_A, motorRearRightCW); // D6
    pwm_set_chan_level(1, PWM_CHAN_B, motorRearLeftCCW); // D7

    #ifdef DEBUG
    Serial.print(F("Motor3:"));
    Serial.print(motor3FrontLeftCW);
    Serial.print(F(" Motor10:"));
    Serial.print(motor10FrontRightCCW);
    Serial.print(F(" Motor9:"));
    Serial.print(motor9RearRightCW);
    Serial.print(F(" Motor11:"));
    Serial.println(motor11RearLeftCCW);
    #endif
}