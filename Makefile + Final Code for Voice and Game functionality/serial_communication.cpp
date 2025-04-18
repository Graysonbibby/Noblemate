#include "serial_communication.hpp"

int initializeComm(){
	const char* device = "/dev/ttyACM0";  // Change to correct port if needed
    int serial_port = open(device, O_RDWR);  // Open for reading & writing

    if (serial_port == -1) {
        std::cout << "Error: Unable to open serial port!" << std::endl;
        return 1;
    }

    // Configure serial port settings
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        std::cout << "Error: Could not get terminal attributes" << std::endl;
        return 1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 data bits
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        std::cout << "Error: Could not apply terminal settings" << std::endl;
        return 1;
    }
    
    std::cout << "✅ Successfully connected to " << device << " at 9600 baud" << std::endl;
    
    return serial_port;
}

void sendData(int serialPort, const char* sent_string){
    write(serialPort, sent_string, strlen(sent_string));
    std::cout << "Sent Successfully!" << std::endl;
}

std::string receiveData(int serial_port){
    std::string received_string;
    ssize_t n = read(serial_port, &received_string, 1);
    if (n > 0) {
        return received_string;
    }
    
    return "nc";
}
