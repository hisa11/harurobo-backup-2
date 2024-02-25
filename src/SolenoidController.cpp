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
    // シリアル通信の読み取り
    int available = serial.readable();
    if (available > 0) {
        serial.read(&buf2, sizeof(buf2));
        printf("Received character: %c\n", buf2);
        ThisThread::sleep_for(20ms);
    }

    // ソレノイドの制御データを設定
    if (buf2 == 'x') // 左動作
    {
        printf("Left operation\n");
        solenoid[0] = 2;
        solenoid[1] = 1;
        buf2 = ' ';
        ThisThread::sleep_for(20ms);
    }
    else if (buf2 == 'v') // 右動作
    {
        printf("Right operation\n");
        solenoid[0] = 1;
        solenoid[1] = 2;
        buf2 = ' ';
        ThisThread::sleep_for(20ms);
    }
    else if (buf2 == 'z' || buf2 == 'Q' || buf2 == 'A') // 停止
    {
        printf("Stop\n");
        solenoid[0] = 1;
        solenoid[1] = 1;
        buf2 = ' ';

    }
    else
    {
        // 未知のコマンド
        printf("Unknown command\n");
        
    }

    // CAN メッセージを作成し送信
    CANMessage msg2(0xf2, (const uint8_t *)solenoid, sizeof(solenoid));
    can.write(msg2);
}
