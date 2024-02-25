#include "ServoController.hpp"

mbed::CANMessage msg4_1;

// サーボ制御用データの宣言
uint8_t servo1_data[8];


ServoController::ServoController(CAN &can, BufferedSerial &serial) : can(can), serial(serial)
{
    // コンストラクタの実装
    // servo1_data 配列を初期化
    memset(servo1_data, 0, sizeof(servo1_data));
}

char buf1; // シリアル受信バッファ
// サーボ制御の実行
void ServoController::run()
{


    serial.set_blocking(false);
    serial.read(&buf1, sizeof(buf1));

    // サーボの制御データを設定
    
    if (buf1 == '1') // 左
    {
        servo1_data[3] = 0;
        printf("左に移動\n");
        auto msg4 = CANMessage{servo_can_id, servo1_data, sizeof(servo1_data)};
        can.write(msg4);
    }
    else if (buf1 == '2') // 正面
    {
        servo1_data[3] = 128;
        printf("正面に移動\n");
        auto msg4 = CANMessage{servo_can_id, servo1_data, sizeof(servo1_data)};
        can.write(msg4);
    }
    else if (buf1 == '3') // 右
    {
        servo1_data[3] = 255;
        printf("右に移動\n");
        auto msg4 = CANMessage{servo_can_id, servo1_data, sizeof(servo1_data)};
        can.write(msg4);
    }
    else
    {
    }

    // CAN メッセージを作成し送信

    // 20 ミリ秒待機
    // ThisThread::sleep_for(20ms);
}
