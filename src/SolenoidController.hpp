#ifndef SOLENOID_CONTROLLER_HPP
#define SOLENOID_CONTROLLER_HPP

#include "mbed.h"

class SolenoidController {
public:
    SolenoidController(CAN& can, BufferedSerial& serial);

    void run();

private:
    CAN& can;
    BufferedSerial& serial;
    uint8_t solenoid[8] = {1, 1, 1, 1, 1, 1, 1, 1}; // ソレノイド制御用データの初期化
};

#endif // SOLENOID_CONTROLLER_HPP
