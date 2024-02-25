#include "asimawari.hpp"
#include <cstdlib>

void Asimawari::processCommand(char* command) {
    if (command[0] == 'r') {
        // 'a'の次の文字から数字を取得し、整数に変換して出力
        valueLY = atoi(&command[1]);
    } else if (command[0] == 'R') {
        valueLX = atoi(&command[1]);
    } else if (command[0] == 'L') {
        valueRL = atoi(&command[1]);
    }

    pwm0[0] = (valueLX + valueLY) * keisu1 + valueRL * keisu2;
    pwm0[1] = (valueLX - valueLY) * keisu1 - valueRL * keisu2;
    pwm0[2] = (valueLX - valueLY) * keisu1 + valueRL * keisu2;
    pwm0[3] = (valueLX + valueLY) * keisu1 - valueRL * keisu2;
}
