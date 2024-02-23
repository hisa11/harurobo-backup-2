#include <mbed.h>
#include "FIRSTPENGUIN.hpp"
#include "SolenoidController.hpp"
#include "ServoController.hpp"

constexpr uint32_t can_id = 30;

BufferedSerial pc{USBTX, USBRX, 115200};
BufferedSerial uart{PB_6, PA_10, 15200};

CAN can{PA_11, PA_12, (int)1e6};
// CAN can{PB_12, PB_13, (int)1e6};
CANMessage msg;

Timer timer;

FirstPenguin penguin{can_id, can};

int main()
{
    printf("\nsetup\n");
    timer.start();
    auto pre = timer.elapsed_time();
    // Initialize controllers outside of the loop
    SolenoidController solenoidController(can, pc);
    ServoController servoController(can, pc);

    while (1)
    {
        if (uart.readable())
        {
            char buf;
            uart.read(&buf, sizeof(buf));
        }
        auto now = timer.elapsed_time();

        if (can.read(msg))
        {
            penguin.read(msg);
        }

        if (now - pre > 20ms)
        {
            for (auto e : penguin.receive)
                printf("% 5ld\t", e.enc);
            for (auto e : penguin.receive)
                printf("% 5ld\t", e.adc);
            printf("\n");
            for (auto &e : penguin.pwm)
                e = penguin.max / 4;

            // Run both controllers alternately
            // servoController.run();
            // ThisThread::sleep_for(1ms); // Give some time for the servo controller to execute its task
            printf("uketottayo");
            // solenoidController.run();

            penguin.pwm[0] = 20000;
            penguin.pwm[1] = 20000;
            penguin.pwm[2] = 20000;

            int res = penguin.send();
            if (!res)
            {
                printf("fail\n");
            }
            pre = now;
        }
    }
}
