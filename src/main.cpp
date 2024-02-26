#include <mbed.h>
#include "FIRSTPENGUIN.hpp"
#include "SolenoidController.hpp"
#include "ServoController.hpp"

constexpr uint32_t can_id = 30;

BufferedSerial pc{USBTX, USBRX, 115200};
CAN can{PA_11, PA_12, (int)1e6};
CANMessage msg;
Timer timer;
FirstPenguin penguin{can_id, can};

int number_r = 0;
int number_R = 0;

int16_t pwm0[4] = {0, 0, 0, 0}; // モタドラ1

int main()
{
    // printf("\nsetup\n");
    timer.start();
    auto pre = timer.elapsed_time();
    SolenoidController solenoidController(can, pc);
    ServoController servoController(can, pc);

    char command[11]; // コマンドと数字の組み合わせを格納する配列
    while (1)
    {
        pc.set_blocking(false);
        pc.read(command, sizeof(command)); // パソコンからのデータを読み込む
        // pc.write("asagohan", 8); // 文字列と長さを指定して書き込む
        // pc.write("\n", 1); // 改行文字を指定して書き込
        printf(command);
        printf("\n");

        auto now = timer.elapsed_time();

        if (command[0] == 'r')
        {                                 // 先頭が'a'の場合
            number_r = atoi(&command[1]); // 後ろの数字を取得
        }
        else if (command[0] == 'R')
        {
            number_R = atoi(&command[1]);
        }
        pwm0[0] = number_r;

        if (can.read(msg))
        {
            penguin.read(msg);
        }

        if (now - pre > 20ms)
        {
            for (auto e : penguin.receive)
                // printf("% 5ld\t", e.enc);
                for (auto e : penguin.receive)
                    // printf("% 5ld\t", e.adc);
                    // printf("\n");
                    for (auto &e : penguin.pwm)
                        e = penguin.max / 4;

            servoController.run();
            solenoidController.run();
            // printf("uketottayo");

            penguin.pwm[1] = 50000;
            penguin.pwm[2] = 50000;

            pre = now;
            int res = penguin.send();
            if (!res)
            {
                // printf("fail\n");
            }
        }
        CANMessage msg0(2, (const uint8_t *)pwm0, 8);
        can.write(msg0);
        // ThisThread::sleep_for(20ms);
    }
}
