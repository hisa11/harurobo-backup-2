#ifndef ASIMAWARI_HPP
#define ASIMAWARI_HPP

#include <cstdint>

class Asimawari {
public:
    void processCommand(char* command);
    int16_t pwm0[4] = {0, 0, 0, 0}; // モタドラ1
private:
    int valueLY = 0;
    int valueLX = 0;
    int valueRL = 0;
    int keisu1 = 30; ////係数
    int keisu2 = 30; ////係数
};

#endif /* ASIMAWARI_HPP */
