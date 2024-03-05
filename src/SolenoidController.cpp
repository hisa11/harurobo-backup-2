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
    int available = serial.readable();{
    if (available > 0) {
        serial.read(&buf2, sizeof(buf2));
        printf("Received character: %c\n", buf2);
        ThisThread::sleep_for(2ms);
    }

    // ソレノイドの制御データを設定
    if (buf2 == 'W') // 前　下げ
    {
        printf("前下げ\n");
        solenoid[0] = 1;
        // solenoid[1] = 1;
        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    else if (buf2 == 'A') // 真ん中　下げ
    {
        printf("真ん中下げ\n");
        solenoid[1] = 2;
        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    else if(buf2 == 'S') //後ろ下げ
    {
        printf("後ろ下げ\n");

        solenoid[2] = 2;
        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    else if (buf2 == 'w') // 前上げ
    {
        printf("前上げ\n");
        solenoid[0] =2 ;
        // solenoid[1] = 1;
        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    else if (buf2 == 'a') // 真ん中上げ
    {
        printf("真ん中上げ\n");
        // solenoid[0] = 1;
        solenoid[1] =1 ;
        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    else if(buf2 == 's') //後ろ上げ
    {
        printf("後ろ上げ\n");
        solenoid[2] = 1;
        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    
    else if(buf2 == 'r') //後ろ上げ
    {
        printf("後ろ上げ\n");
        solenoid[0] = 2;
        solenoid[2] = 1;
        solenoid[1] = 1;

        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    else if(buf2 == 'l') //後ろ上げ
    {
        printf("後ろ上げ\n");
        solenoid[0] = 1;
        solenoid[2] = 2;
        solenoid[1] = 2;


        buf2 = ' ';
        ThisThread::sleep_for(2ms);
    }
    // else if(buf2 == 'w')
    else
    {
        // 未知のコマンド
        // printf("Unknown command\n");
        solenoid[0] = 0;
        solenoid[1] = 0;
        solenoid[2] = 0;
        
    }

    // CAN メッセージを作成し送信
    CANMessage msg2(0xf2, (const uint8_t *)solenoid, sizeof(solenoid));
    can.write(msg2);
    }
}
