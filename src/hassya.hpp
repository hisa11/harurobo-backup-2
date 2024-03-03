#ifndef HASSYA_HPP
#define HASSYA_HPP

#include "mbed.h"

extern int suuti;
extern int gohan;

void process_serial_input(BufferedSerial& pc, CAN& can);

#endif // HASSYA_HPP
