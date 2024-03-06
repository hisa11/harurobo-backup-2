#include <mbed.h>
#include "FIRSTPENGUIN.hpp"
#include "SolenoidController.hpp"
#include "ServoController.hpp"
#include "hassya.hpp"

constexpr uint32_t can_id = 30;

BufferedSerial pc{PB_6, PA_10, 2600000};
CAN can{PA_11, PA_12, (int)1e6};
// CAN can1{PA_12, PA_13, (int)1e6};
CANMessage msg;
Timer timer;
FirstPenguin penguin{can_id, can};

DigitalIn button1(PC_13);
DigitalIn button2(PC_12);

int suuti = 0;
int gohan = 0;
int tamagokoppepan = 0;
int cylinder3_time = 0;
int main()
{
    button1.mode(PullUp);
    button2.mode(PullUp);
    timer.start();
    SolenoidController solenoidController(can, pc);
    ServoController servoController(can, pc);

    char command[1];                // Command and number combination array
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
            // break;
        }
        command[idx] = '\0';   // Add string terminator
        printf("%s", command); // Print received command

        // Process motor control commands
        char buf;
        int read_result = pc.read(&buf, sizeof(buf));
        if (read_result > 0) {
            pc.write(&buf, sizeof(buf));
            switch (buf) {
                case 'D':
                    penguin.pwm[0] = 17000;
                    penguin.pwm[1] = 0;
                    penguin.pwm[2] = 0;
                    penguin.send();
                    break;
                case 'd':
                    penguin.pwm[0] = -17000;
                    penguin.pwm[1] = 0;
                    penguin.pwm[2] = 0;
                    penguin.send();
                    break;
                case 'f':
                    penguin.pwm[0] = 0;
                    penguin.pwm[1] = 0;
                    penguin.pwm[2] = 5000;
                    penguin.send();
                    break;
                case 'g':
                    penguin.pwm[0] = 0;
                    penguin.pwm[1] = 0;
                    penguin.pwm[2] = -5000;
                    penguin.send();
                    break;
                case 'i':
                    if (button2 == 1)
                    {
                        penguin.pwm[0] = 3000;
                        penguin.pwm[3] = 0;
                        penguin.pwm[2] = 0;
                    }
                    else
                    {
                        penguin.pwm[1] = 0;
                        penguin.pwm[0] = 0;
                        penguin.pwm[2] = 0;
                    }
                    penguin.send();
                    break;
                case 'y':
                    if (button1 == 1)
                    {
                        penguin.pwm[1] = -3000;
                        penguin.pwm[0] = 0;
                        penguin.pwm[2] = 0;
                    }
                    else
                    {
                        penguin.pwm[0] = 0;
                        penguin.pwm[1] = 0;
                        penguin.pwm[2] = 0;
                    }
                    penguin.send();
                    break;

                default:
                    penguin.pwm[0] = 0;
                    penguin.pwm[1] = 0;
                    penguin.pwm[2] = 0;
                    penguin.send();
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
                case 'M':
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
                case 'm':
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
                case '0':
                    number_L = 0;
                    break;
            }
        }

        // Calculate motor PWM values

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
        pwm0[0] = static_cast<int16_t>(((number_r - number_R) * 90 - number_L * 30));
        pwm0[1] = static_cast<int16_t>(((number_r + number_R) * 90 - number_L * 30));
        pwm0[2] = static_cast<int16_t>(-((number_r + number_R) * 90 + number_L * 30));
        pwm0[3] = static_cast<int16_t>(-((number_r - number_R) * 90 + number_L * 30));

        // Process motor speed control
        // Send motor PWM values via CAN
        CANMessage msg0(1, (const uint8_t *)pwm0, 8);
        can.write(msg0);
        printf("pwm: %d\n", pwm0);
    }
}
