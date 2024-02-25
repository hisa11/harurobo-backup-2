#include <mbed.h>
#include "FIRSTPENGUIN.hpp"
#include "SolenoidController.hpp"
#include "ServoController.hpp"
#include "asimawari.hpp"

constexpr uint32_t can_id = 30;

BufferedSerial pc{USBTX, USBRX, 115200};
CAN can{PA_11, PA_12, (int)1e6};
CANMessage msg;
Timer timer;
FirstPenguin penguin{can_id, can};
Asimawari asimawari;

int main() {
    printf("\nsetup\n");
    timer.start();
    auto pre = timer.elapsed_time();
    SolenoidController solenoidController(can, pc);
    ServoController servoController(can, pc);

    while (1) {
        pc.set_blocking(false);
        char command[5];
        pc.read(command, sizeof(command));

        auto now = timer.elapsed_time();

        if (can.read(msg)) {
            penguin.read(msg);
        }

        if (now - pre > 20ms) {
            for (auto e : penguin.receive)
                printf("% 5ld\t", e.enc);
            for (auto e : penguin.receive)
                printf("% 5ld\t", e.adc);
            printf("\n");
            for (auto &e : penguin.pwm)
                e = penguin.max / 4;

            servoController.run();
            solenoidController.run();
            printf("uketottayo");

            penguin.pwm[1] = 50000;
            penguin.pwm[2] = 50000;

            pre = now;
            int res = penguin.send();
            if (!res) {
                printf("fail\n");
            }
        }

        asimawari.processCommand(command);
    }
}
