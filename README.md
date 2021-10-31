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

### IMU Orientation

The Nano RP2040 must be placed with the USB connector on the right side. 
The dot is on the top left of the chip


## Loop function

In the main loop, we read input from RC and from IMU gyros, calculate the altitude and then implement 3 PIDs for pitch, roll and yaw. Finally we send pulses to ESCS. 

