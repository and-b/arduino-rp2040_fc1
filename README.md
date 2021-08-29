# Arduino Flight Controller

This is my first attempt writing a flight controller for a Quadcopter with Arduino Nano

I didnt start from zero, but I studied a couple of implementations:
- multiwii
- Brokking

## Motor configuration

The motors are connected in this configuration, the same used in Multiwii

    
    D3   CW              CCW D10
             O       O
                 O
             O       O
    D11 CCW              CW  D9

## RC

The RC chnnels are connected in this order:
- D2: throttle
- D4: roll
- D5: pitch
- D6: yaw

## Setup function

The setup function performs these tasks:
- initialize I2C protocol and check the IMU
- calibrate the IMU gyros
- enable PCINT to receive PWM from RC
- enable PWM outs to command the ESCs

### IMU Orientation
The MPU6050 must be placed with the headers on the left side. The dot on the chip must be in the top left corner.

Data is read always in the sequence X,Y,Z. According to the datasheet, viewing from the top, the X axis is pointing to left and rotation is positive nose up. The Y axis is pointing to the front, and rotation is positive to the right. No indication is given for the rotation of the Z axis. Experimentally, Z is positive when rotating counterclockwise, and negative clockwise. So we need to multiply Z by -1


## Loop function

In the main loop, we read input from RC and from IMU gyros, calculate the altitude and then implement 3 PIDs for pitch, roll and yaw. Finally we send pulses to ESCS. 

## Telemetry
