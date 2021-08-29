#include "Arduino.h"
#include "fc1.h"
#include "rc.h"

#define IBUS_BUFFER_SIZE 32
static uint8_t buffer[IBUS_BUFFER_SIZE];
static uint8_t bufferIndex = 0;
uint16_t checksum = 0;

void decodeIBus() {
    // lenght: 0x20
    // Command: 0x40
    // ch1 LSB+MSB
    // ..
    // ch14 LSB-MSB
    // checksum
    while (Serial1.available() > 0) {
        uint8_t value = Serial1.read();
        if (bufferIndex == 0 && value != 0x20) {continue;}
        if (bufferIndex == 1 && value != 0x40) {bufferIndex = 0; continue;}
        if (bufferIndex < IBUS_BUFFER_SIZE) buffer[bufferIndex] = value;
        bufferIndex++;

        if (bufferIndex == IBUS_BUFFER_SIZE) {
            bufferIndex = 0;
            uint16_t expected = buffer[IBUS_BUFFER_SIZE-1] * 256 + buffer[IBUS_BUFFER_SIZE-2];
            checksum =0;
            for (int i=0; i < IBUS_BUFFER_SIZE-2; i++) {checksum+=buffer[i];}
            if ((expected + checksum) == 0xFFFF) {
                if (buffer[1] == 0x40) { 
                    chRoll = buffer[3] * 256 + buffer[2];
                    chPitch = buffer[5] * 256 + buffer[4];
                    chThrottle = buffer[7] * 256 + buffer[6];
                    chYaw = buffer[9] * 256 + buffer[8];
                }
            }
        }
    }   
}