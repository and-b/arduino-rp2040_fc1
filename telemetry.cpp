#include "Arduino.h"
#include "fc1.h"
#include "imu.h"
#include "telemetry.h"

uint8_t telemetryData;
uint8_t telemetryIndex;
uint8_t checkSum;
uint16_t tempValue;

#define TELEMETRY_BUFFER_SIZE 32

void sendTelemetry() {
    telemetryIndex++;
    telemetryData = 0x00;
    switch(telemetryIndex) {
        case 1: //signature
            telemetryData = 'A';
            break;
        case 2: //signature
            telemetryData = 'B';
            break;
        case 3: // motor3 LSB
            tempValue = motorFrontLeftCW;
            telemetryData = tempValue;
            break;
        case 4: // motor3 MSB
            telemetryData = tempValue >> 8;
            break;
        case 5: // motor10 LSB
            tempValue = motorFrontRightCCW;
            telemetryData = tempValue;
            break;
        case 6: // motor10 MSB
            telemetryData = tempValue >> 8;
            break;
        case 7: // motor9 LSB
            tempValue = motorRearRightCW;
            telemetryData = tempValue;
            break;
        case 8: // motor9 MSB
            telemetryData = tempValue >> 8;
            break;
        case 9: // motor11 LSB
            tempValue = motorRearLeftCCW;
            telemetryData = tempValue;
            break;
        case 10: // motor11 MSB
            telemetryData = tempValue >> 8;
            break;
        case 11: // status
            telemetryData = start;
            break;
         case 12: // loop time LSB
            tempValue = loopTime;
            telemetryData = tempValue;
            break;
        case 13: // loop time MSB
            telemetryData = tempValue >> 8;
            break;
        case 14: // pitch angle between -89.99 and 89.99 LSB
            tempValue = (angle_pitch * 100) + 10000;
            telemetryData = tempValue;
            break;
        case 15: // pitch angle MSB
            telemetryData = tempValue >> 8;
            break;
        case 16: // roll angle between -89.99 and 89.99 LSB
            tempValue = (angle_roll * 100) + 10000;
            telemetryData = tempValue;
            break;
        case 17: // roll angle MSB
            telemetryData = tempValue >> 8;
            break;
        case 31: // error
            telemetryData = fcError;
            break;
        case 32: // checksum
            telemetryData = checkSum;
            break;
    }

    if (telemetryIndex <= TELEMETRY_BUFFER_SIZE) {
        checkSum ^= telemetryData;
        Serial.write(telemetryData);
    }

    if (telemetryIndex == 125) {
      telemetryIndex = 0;
      checkSum = 0;
    }
}