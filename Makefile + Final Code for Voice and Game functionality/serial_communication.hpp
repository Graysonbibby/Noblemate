#ifndef SERIAL_COMM_HPP
#define SERIAL_COMM_HPP

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

int initializeComm();
void sendData(int serialPort, const char* sent_string);
std::string receiveData(int serialPort);

#endif
