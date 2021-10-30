# Arduino NANO RP2040 Flight Controller

The new Arduino NANO RP2040 

## Motor configuration

The motors are connected in this configuration

    
    D4   CW              CCW D5
             O       O
                 O
             O       O
    D7  CCW              CW  D6

## RC

The RC reads I-Bus protocol using Serial1 (UART0)

## Setup function

The setup function performs these tasks:
- initialize I2C protocol and check the IMU
- calibrate the IMU gyros
- enable PCINT to receive PWM from RC
- enable PWM outs to command the ESCs

### IMU Orientation

The Nano RP2040 must be placed with the USB connector on the right side. 
The dot is on the top left of the chip


## Loop function

In the main loop, we read input from RC and from IMU gyros, calculate the altitude and then implement 3 PIDs for pitch, roll and yaw. Finally we send pulses to ESCS. 

## Telemetry

## Board building

TopLeft: 4,2
TopRight: 5,2
BottomLeft: 4,1
BottomRight: 5,1
