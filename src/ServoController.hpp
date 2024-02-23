#ifndef SERVO_CONTROLLER_HPP
#define SERVO_CONTROLLER_HPP

#include "mbed.h"

constexpr uint32_t servo_can_id = 149;
extern uint8_t servo1_data[8];

class ServoController {
public:
    ServoController(CAN& can, BufferedSerial& serial);

    void run();

    void kirikae();

private:
    CAN& can;
    BufferedSerial& serial;
    uint8_t servo1_data[8] = {0}; // サーボ制御用データの初期化
};

#endif
