#include "pm5000s.h"

#include <iostream>

using namespace pm5000s;

int main(int argc, char const* const* const argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Serial Device>" << std::endl;
        return -1;
    }

    constexpr SerialPort::ErrorCode OK = SerialPort::ErrorCode::OK;

    SerialPort::ErrorCode err = OK;

    SerialPort serial(argv[1]);
    std::string serial_no;
    if (err = serial.ReadSerialNo(serial_no); err != OK) {
        std::cerr << SerialPort::StrError(err) << std::endl;
        return static_cast<int>(err);
    }
    std::cout << "Serial Number: " << serial_no << std::endl;

    std::string sw_ver;
    if (err = serial.ReadSwVersion(sw_ver); err != OK) {
        std::cerr << SerialPort::StrError(err) << std::endl;
        return static_cast<int>(err);
    }
    std::cout << "Software Version Number: " << sw_ver << std::endl;

    return 0;
}
