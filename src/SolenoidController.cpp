#include "SolenoidController.hpp"

mbed::CANMessage msg2_1;
// ソレノイド制御用データの宣言
uint8_t solenoid[8];

SolenoidController::SolenoidController(CAN &can, BufferedSerial &serial) : can(can), serial(serial)
{
    // コンストラクタの実装
    // solenoid 配列を初期化
    memset(solenoid, 1, sizeof(solenoid));
}
char buf2; // シリアル受信バッファ

// ソレノイド制御の実行
void SolenoidController::run()
{

    serial.read(&buf2, sizeof(buf2));

    // ソレノイドの制御データを設定
    if (buf2 == 'x') // 左動作
    {
        solenoid[0] = 0;
        solenoid[1] = 1;
        printf("左動作\n");
        CANMessage msg2(0xf2, (const uint8_t *)solenoid, sizeof(solenoid));
        can.write(msg2);
    }
    else if (buf2 == 'v') // 右動作
    {
        solenoid[0] = 1;
        solenoid[1] = 0;
        printf("右動作\n");
        CANMessage msg2(0xf2, (const uint8_t *)solenoid, sizeof(solenoid));
        can.write(msg2);
    }
    else if (buf2 == 'z' || buf2 == 'Q' || buf2 == 'A') // 停止
    {
        solenoid[0] = 1;
        solenoid[1] = 1;
        printf("停止\n");
        CANMessage msg2(0xf2, (const uint8_t *)solenoid, sizeof(solenoid));
        can.write(msg2);
    }
    else
    {
    }

    // CAN メッセージを作成し送信
    // CANMessage msg2(0xf2, (const uint8_t *)solenoid, sizeof(solenoid));

    // can.write(msg2);
    // msg2_1 = msg2;

    // 20 ミリ秒待機
    // ThisThread::sleep_for(20ms);
}
