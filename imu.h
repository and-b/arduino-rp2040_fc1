#ifndef _IMU_H_
#define _IMU_H_

void setupImu();
void calibrateGyro();
void readImu(bool applyGyroCalibration);
void calculateAttitude();

#endif