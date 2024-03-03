#include "hassya.hpp"

void process_serial_input(BufferedSerial& pc, CAN& can) {
    if (pc.readable()) {
        char buf;
        pc.read(&buf, sizeof(buf));
        if (buf == 'w') {
            suuti = 12;
            gohan = 1;
        } else if (buf == 's') {
            suuti = 0;
        }
        if (buf == 'a') {
            suuti = -120;
        }
        if (gohan == 1) {
            ThisThread::sleep_for(1ms);
            suuti = 0;
            gohan = 0;
        }
        int16_t output = suuti;
        uint8_t DATA[8] = {};
        DATA[0] = output >> 1;
        DATA[1] = output;
        CANMessage msg44(0x1FF, DATA, 8);
        can.write(msg44);
    }
}
