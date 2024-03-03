#include <mbed.h>
#include "FIRSTPENGUIN.hpp"
#include "SolenoidController.hpp"
#include "ServoController.hpp"
#include "hassya.hpp"

constexpr uint32_t can_id = 30;

BufferedSerial pc{PB_6, PA_10, 2600000};
CAN can{PA_11, PA_12, (int)1e6};
CANMessage msg;
Timer timer;
FirstPenguin penguin{can_id, can};

int suuti = 0;
int gohan = 0;

int main()
{
    timer.start();
    SolenoidController solenoidController(can, pc);
    ServoController servoController(can, pc);

    char command[7];                // Command and number combination array
    int16_t pwm0[4] = {0, 0, 0, 0}; // Motor control array

    auto pre = timer.elapsed_time(); // Previous time
    int number_r = 0, number_R = 0, number_L = 0;
    int loop_count = 0;

    while (1)
    {
        // Read command from serial input
        int idx = 0;
        while (idx < sizeof(command) - 1) // Ensure space for null terminator
        {
            int n = pc.read(&command[idx], sizeof(command) - 1 - idx);
            if (n <= 0)
            {
                ThisThread::sleep_for(10ms); // Wait if no data available
            }
            else
            {
                idx += n;
            }
        }
        command[idx] = '\0';   // Add string terminator
        printf("%s", command); // Print received command

        // Process motor control commands
        char buf;
        pc.read(&buf, sizeof(buf));
        pc.write(&buf, sizeof(buf));
        switch (buf)
        {
        case 'D':
            penguin.pwm[0] = 7000;
            break;
        case 'd':
            penguin.pwm[0] = -7000;
            break;
        case 'k':
            penguin.pwm[1] = 7000;
            break;
        case 'y':
            penguin.pwm[1] = -7000;
            break;
        default:
            penguin.pwm[0] = 0;
            penguin.pwm[1] = 0;
            break;
        }
        switch (buf)
        {
        case 'Z':
            number_r = 120;
            break;
        case 'X':
            number_r = 90;
            break;
        case 'C':
            number_r = 30;
            break;
        case 'V':
            number_r = -30;
            break;
        case 'B':
            number_r = -90;
            break;
        case 'N':
            number_r = -120;
            break;
        default:
            number_r = 0;
            break;
        }
        switch (buf)
        {
        case 'z':
            number_R = -120;
            break;
        case 'x':
            number_R = -60;
            break;
        case 'c':
            number_R = -30;
            break;
        case 'v':
            number_R = 30;
            break;
        case 'b':
            number_R = 60;
            break;
        case 'n':
            number_R = 120;
            break;
        default:
            number_R = 0;
            break;
        }
        switch (buf)
        {
        case '1':
            number_L = 90;
            break;
        case '2':
            number_L = 60;
            break;
        case '3':
            number_L = 30;
            break;
        case '4':
            number_L = -30;
            break;
        case '5':
            number_L = -60;
            break;
        case '6':
            number_L = -90;
            break;
        default:
            number_L = 0;
            break;
        }

        penguin.send();
        // Calculate motor PWM values
        pwm0[0] = static_cast<int16_t>(((number_r - number_R) * 90 + number_L * 90));
        pwm0[1] = static_cast<int16_t>(((number_r + number_R) * 90 - number_L * 90));
        pwm0[2] = static_cast<int16_t>(-((number_r + number_R) * 90 + number_L * 90));
        pwm0[3] = static_cast<int16_t>(-((number_r - number_R) * 90 - number_L * 90));

        // Process motor speed control
        // Send motor PWM values via CAN
        CANMessage msg0(1, (const uint8_t *)pwm0, 8);
        can.write(msg0);
        printf("pwm: %d\n", pwm0);

        // Read feedback from CAN
        if (can.read(msg))
        {
            penguin.read(msg);
        }

        // Perform additional tasks every 20 milliseconds
        auto now = timer.elapsed_time();
        if (now - pre > 20ms)
        {
            // Print received encoder and ADC values
            for (auto e : penguin.receive)
                printf("% 5ld\t", e.enc);
            for (auto e : penguin.receive)
                printf("% 5ld\t", e.adc);
            printf("\n");

            // Set default PWM values for servo and solenoid controllers
            for (auto &e : penguin.pwm)
                e = penguin.max / 4;

            // Run servo and solenoid controllers
            servoController.run();
            solenoidController.run();
            process_serial_input(pc, can);

            // Update previous time
            pre = now;

            // Reset loop count and command buffer every 4 iterations
            loop_count++;
            if (loop_count >= 20)
            {
                loop_count = 0;
                memset(command, 0, sizeof(command));
                while (pc.readable())
                {
                    char c;
                    pc.read(&c, 1);
                }
            }
        }
    }
}
