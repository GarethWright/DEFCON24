Principle:

Validate GPS subframe data

Device:

Raspberry Pi 2 Model B, OS: RASPBIAN
LCM1602 I2C (I2C_ADDR  = 0x27)
ublox NEO-M8N (mount on /dev/ttyUSB0)
LED and buzzer (GPIO 18)

Compile:

*Install wiringPi library (git://git.drogon.net/wiringPi)

*g++ -lwiringPi -lwiringPiDev -lpthread -lm dtgps.cpp -o dtgps