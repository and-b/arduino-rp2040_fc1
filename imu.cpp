#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM6DSOX.h>
#include "fc1.h"
#include "imu.h"

#define IMU_CALIBRATION_COUNT 2000

#define LSM6DS3_ADDRESS            0x6A

#define LSM6DS3_WHO_AM_I_REG       0X0F
#define LSM6DS3_CTRL1_XL           0X10
#define LSM6DS3_CTRL2_G            0X11

#define LSM6DS3_STATUS_REG         0X1E

#define LSM6DS3_CTRL6_C            0X15
#define LSM6DS3_CTRL7_G            0X16
#define LSM6DS3_CTRL8_XL           0X17

#define LSM6DS3_OUTX_L_G           0X22
#define LSM6DS3_OUTX_H_G           0X23
#define LSM6DS3_OUTY_L_G           0X24
#define LSM6DS3_OUTY_H_G           0X25
#define LSM6DS3_OUTZ_L_G           0X26
#define LSM6DS3_OUTZ_H_G           0X27

#define LSM6DS3_OUTX_L_XL          0X28
#define LSM6DS3_OUTX_H_XL          0X29
#define LSM6DS3_OUTY_L_XL          0X2A
#define LSM6DS3_OUTY_H_XL          0X2B
#define LSM6DS3_OUTZ_L_XL          0X2C
#define LSM6DS3_OUTZ_H_XL          0X2D

float gyro_calib_pitch, gyro_calib_roll, gyro_calib_yaw = 0;
int16_t accel_calib_x, accel_calib_y, accel_calib_z = 0;
int16_t accel_x, accel_y, accel_z;
float accel_angle_pitch, accel_angle_roll;

void setupImu() {
    Wire.begin();
    Wire.setClock(400000);

    //set the gyroscope control register to work at 104 Hz, 2000 dps and in bypass mode 0x4C 0100 110 0
    // writeRegister(LSM6DS3_CTRL2_G, 0x4C);
    Wire.beginTransmission(LSM6DS3_ADDRESS);
    Wire.write(LSM6DS3_CTRL2_G);
    Wire.write(0b01010100); // 208Hz 500DPS
    Wire.endTransmission();
      
    // Set the Accelerometer control register to work at 104 Hz, 4 g,and in bypass mode and enable ODR/4 0x4A 0100 10 1 0
    // low pass filter (check figure9 of LSM6DS3's datasheet)
    //writeRegister(LSM6DS3_CTRL1_XL, 0x4A);
    Wire.beginTransmission(LSM6DS3_ADDRESS);
    Wire.write(LSM6DS3_CTRL1_XL);
    Wire.write(0b01011010);
    Wire.endTransmission();

    // set gyroscope power mode to high performance and bandwidth to 16 MHz 0x00
    //writeRegister(LSM6DS3_CTRL7_G, 0x00);
    Wire.beginTransmission(LSM6DS3_ADDRESS);
    Wire.write(LSM6DS3_CTRL7_G);
    Wire.write(0x00);
    Wire.endTransmission();

    // Set the ODR config register to ODR/4 0x09
    //writeRegister(LSM6DS3_CTRL8_XL, 0x09);
    Wire.beginTransmission(LSM6DS3_ADDRESS);
    Wire.write(LSM6DS3_CTRL8_XL);
    Wire.write(0x09);
    Wire.endTransmission();
}

void calibrateGyro() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    
    gyro_calib_pitch = gyro_calib_roll = gyro_calib_yaw = 0;
    
    for (int count = 0; count < IMU_CALIBRATION_COUNT; count++) {
        readImu(false);
        gyro_calib_pitch += gyro_pitch;
        gyro_calib_roll += gyro_roll;
        gyro_calib_yaw += gyro_yaw;
        if (count % 50 == 0) digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(4);
    }

    gyro_calib_pitch /= IMU_CALIBRATION_COUNT;
    gyro_calib_roll /= IMU_CALIBRATION_COUNT;
    gyro_calib_yaw /= IMU_CALIBRATION_COUNT;

    // reset!!!!
    angle_pitch = 0;
    angle_roll = 0;
}

void readImu(bool applyGyroCalibration) {
    unsigned long time_passed = micros();
    int16_t gyro_x, gyro_y, gyro_z;
    
    Wire.beginTransmission(LSM6DS3_ADDRESS);
    Wire.write(LSM6DS3_OUTX_L_G);
    Wire.endTransmission();
    Wire.requestFrom(LSM6DS3_ADDRESS, 12);
    while(Wire.available() < 12); 
    gyro_x = Wire.read() | Wire.read() << 8; 
    gyro_y = Wire.read() | Wire.read() << 8;
    gyro_z = Wire.read() | Wire.read() << 8;
    
    accel_x = Wire.read() | Wire.read() << 8;
    accel_y = Wire.read() | Wire.read() << 8;
    accel_z = Wire.read() | Wire.read() << 8;
    
    // 500 DPS -> full scale is 500 degree every second -> a signed value of 32767 means 500 degree -> a value of 65.534 means 1 degree per second
    // 2000 DPS -> 32767 is 2000 degree -> a values of 16,3835 means 1 degree per second
    gyro_roll = gyro_y;
    gyro_pitch = gyro_x;
    gyro_yaw = -gyro_z; // invert z axis, default is counterclockwise

    if (applyGyroCalibration) {
        gyro_roll -= gyro_calib_roll;
        gyro_pitch -= gyro_calib_pitch;
        gyro_yaw -= gyro_calib_yaw;
    }

    #ifdef DEBUG
    Serial.print(F("Pitch:"));
    Serial.print(gyro_pitch);
    Serial.print(F("   Roll:"));
    Serial.print(gyro_roll);
    Serial.print(F("   Yaw:"));
    Serial.println(gyro_yaw);
    #endif
    time_passed = micros() - time_passed;
}

void calculateAttitude() {
    float accel_total;
    
    // move angles using gyro data
    //0.0000611 = 1 / (250Hz / 65.5)
    // 1 / (250Hz / 16.3835) = ?
    angle_pitch += gyro_pitch * 0.0000611;
    angle_roll += gyro_roll * 0.0000611;  

    // transfer rotation if yawed
    //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
    angle_pitch -= angle_roll * sin(gyro_yaw * 0.000001066);
    angle_roll += angle_pitch * sin(gyro_yaw * 0.000001066);   
    
    // calculate accel vector length
    accel_total = sqrtf((int32_t)accel_x*accel_x + (int32_t)accel_y*accel_y + (int32_t)accel_z*accel_z);
    
    // calculate accel_angles: if I rotate right on the Y axis (roll), the gravity "pulls me" on the X axis in negative
    // if I pitch up (rotation on the X axis), the gravity "pulls me" on the Y axis in positive
    if (abs(accel_y) < accel_total) {
        accel_angle_pitch = asinf(accel_y / accel_total) * 57.295; // 180/PI
    }
    if (abs(accel_x) < accel_total) {
       accel_angle_roll = -asinf(accel_x / accel_total) * 57.295; // 180/PI
    }

    accel_angle_pitch -= -1.2;
    accel_angle_roll -= -1.1;
    
    // apply complimentary filter
    angle_pitch = angle_pitch * 0.9996 + accel_angle_pitch * 0.0004;
    angle_roll = angle_roll * 0.9996 + accel_angle_roll * 0.0004;

    #ifdef DEBUG
    Serial.print(F("Angle_Pitch:"));
    Serial.print(angle_pitch);
    Serial.print(F("   Angle_Roll:"));
    Serial.println(angle_roll);
    #endif
}

