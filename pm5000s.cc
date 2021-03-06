#include <iostream>
#include <ios>
#include <limits>
#include <string>
#include <sstream>

#include "pm5000s.h"

using namespace pm5000s;

constexpr SerialPort::ErrorCode OK = SerialPort::ErrorCode::OK;

enum class Command {
    READ_SERIAL_NO = 1,
    READ_SW_VER = 2,
    READ_PRTCL_COEF = 3,
    SETUP_PRTCL_COEF = 4,
    OPEN_PRTCL_MSR = 5,
    CLOSE_PRTCL_MSR = 6,
    READ_PRTCL_MSR = 7,
    READ_DEVICE_PATH = 8,
    EXIT = 9
};

template <typename T>
bool Read(T& ret) {
    std::string line;
    std::getline(std::cin, line);

    std::stringstream ss{line};
    ss >> ret;

    return !ss.fail();
}

inline void ShowPrompt() {
    std::cout << "Enter Command Number you want to execute\n"
                 "\n"
              << static_cast<int>(Command::READ_SERIAL_NO)
              << ". Read Serial Number\n"
              << static_cast<int>(Command::READ_SW_VER)
              << ". Read SW Version Number\n"
              << static_cast<int>(Command::READ_PRTCL_COEF)
              << ". Read Particle Calibration Coefficient\n"
              << static_cast<int>(Command::SETUP_PRTCL_COEF)
              << ". Set up Particle Calibration Coefficient\n"
              << static_cast<int>(Command::OPEN_PRTCL_MSR)
              << ". Open Particle Measurement\n"
              << static_cast<int>(Command::CLOSE_PRTCL_MSR)
              << ". Close Particle Measurement\n"
              << static_cast<int>(Command::READ_PRTCL_MSR)
              << ". Read Particle Measurement\n"
              << static_cast<int>(Command::READ_DEVICE_PATH)
              << ". Read Device Path\n"
              << static_cast<int>(Command::EXIT) << ". Exit" << std::endl;
}

inline void ReadSerialNumber(const SerialPort& serial) {
    std::string serial_no;
    auto err = serial.ReadSerialNo(serial_no);
    if (err != OK) {
        std::cout << "Failed to read Serial Number: "
                  << SerialPort::StrError(err) << std::endl;
        return;
    }
    std::cout << "=> Serial Number: " << serial_no << std::endl;
}

inline void ReadSwVersion(const SerialPort& serial) {
    std::string sw_ver;
    auto err = serial.ReadSwVersion(sw_ver);
    if (err != OK) {
        std::cout << "Failed to read SW Version: " << SerialPort::StrError(err)
                  << std::endl;
        return;
    }
    std::cout << "=> SW Version Number: " << sw_ver << std::endl;
}

inline void ReadParticleCalibrationCoefficient(const SerialPort& serial) {
    unsigned char coeffi;
    auto err = serial.ReadCalibCoeff(coeffi);
    if (err != OK) {
        std::cout << "Failed to read Calibration Coefficient: "
                  << SerialPort::StrError(err) << std::endl;
        return;
    }
    std::cout << "=> Particle Calibration Coefficient: "
              << static_cast<int>(coeffi) << "/100" << std::endl;
}
inline void SetupParticleCalibrationCoefficient(const SerialPort& serial) {
    int input;
    while (true) {
        std::cout << "Enter Coefficient(10 ~ 250): ";
        std::cout.flush();

        if (!Read(input)) {
            std::cout << "Invalid Input for Coefficient" << std::endl;
            continue;
        }
        if (input < 10 || input > 250) {
            std::cout << "Invalid Coefficient Range" << std::endl;
            continue;
        }
        break;
    }
    unsigned char coeffi = static_cast<unsigned char>(input);

    unsigned char read_coeffi;
    auto err = serial.SetupCalibCoeff(coeffi, read_coeffi);
    if (err != OK) {
        std::cout << "Failed to set Calibration Coefficient: "
                  << SerialPort::StrError(err) << std::endl;
        return;
    }
    std::cout << "=> Set Calibration Coefficient to "
              << static_cast<int>(read_coeffi) << "/100" << std::endl;
}
inline void OpenParticleMeasurement(const SerialPort& serial) {
    auto err = serial.OpenParticleMeasurement();
    if (err != OK) {
        std::cout << "Failed to open particle measurement: "
                  << SerialPort::StrError(err) << std::endl;
        return;
    }

    std::cout << "=> Success to open particle measurment" << std::endl;
}
inline void CloseParticleMeasurment(const SerialPort& serial) {
    auto err = serial.CloseParticleMeasurement();
    if (err != OK) {
        std::cout << "Failed to close particle measurement: "
                  << SerialPort::StrError(err) << std::endl;
        return;
    }
    std::cout << "=> Success to close particle measurment" << std::endl;
}
inline void ReadParticleMeasurment(const SerialPort& serial) {
    uint32_t pm_0_3_um, pm_0_5_um, pm_1_0_um, pm_2_5_um, pm_5_0_um, pm_10_0_um;
    unsigned char alarm;
    auto err =
        serial.ReadParticleMeasurement(pm_0_3_um, pm_0_5_um, pm_1_0_um,
                                       pm_2_5_um, pm_5_0_um, pm_10_0_um, alarm);
    if (err != OK) {
        std::cout << "Failed to read particle measurement: "
                  << SerialPort::StrError(err) << std::endl;
        return;
    }

    std::cout << "-----------------------------------\n"
              << "  >0.3um: " << pm_0_3_um << " pcs/L\n"
              << "  >0.5um: " << pm_0_5_um << " pcs/L\n"
              << "  >1.0um: " << pm_1_0_um << " pcs/L\n"
              << "  >2.5um: " << pm_2_5_um << " pcs/L\n"
              << "  >5.0um: " << pm_5_0_um << " pcs/L\n"
              << "  >10.0um: " << pm_10_0_um << " pcs/L\n";
    if (alarm) {
        std::cout << "  " << SerialPort::AlarmToString(alarm) << '\n';
    }
    std::cout << "-----------------------------------" << std::endl;
}
inline void ReadDevicePath(const SerialPort& serial) {
    std::cout << "=> Device Path: " << serial.GetDevicePath();
    if (serial.IsOpened()) {
        std::cout << "(Opened)" << std::endl;
    } else {
        std::cout << "(Not Opened)" << std::endl;
    }
}

int main(int argc, char const* const* const argv) {
    std::cout << "pm5000s : v" << PM5000S_VERSION_MAJOR << '.'
              << PM5000S_VERSION_MINOR << '.' << PM5000S_VERSION_PATCH
              << std::endl;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <Serial Device>" << std::endl;
        return -1;
    }

    std::string device_path{argv[1]};
    SerialPort serial(device_path);

    if (!serial.IsOpened()) {
        std::cout << "Failed to open " << device_path << std::endl;
        return -1;
    }

    int cmd;
    std::string line;
    while (true) {
        ShowPrompt();

        if (!Read(cmd)) {
            std::cout << "\nInvalid Command" << std::endl;
            continue;
        }
        std::cout << '\n';

        switch (cmd) {
            case static_cast<int>(Command::READ_SERIAL_NO):
                ReadSerialNumber(serial);
                break;
            case static_cast<int>(Command::READ_SW_VER):
                ReadSwVersion(serial);
                break;
            case static_cast<int>(Command::READ_PRTCL_COEF):
                ReadParticleCalibrationCoefficient(serial);
                break;
            case static_cast<int>(Command::SETUP_PRTCL_COEF):
                SetupParticleCalibrationCoefficient(serial);
                break;
            case static_cast<int>(Command::OPEN_PRTCL_MSR):
                OpenParticleMeasurement(serial);
                break;
            case static_cast<int>(Command::CLOSE_PRTCL_MSR):
                CloseParticleMeasurment(serial);
                break;
            case static_cast<int>(Command::READ_PRTCL_MSR):
                ReadParticleMeasurment(serial);
                break;
            case static_cast<int>(Command::READ_DEVICE_PATH):
                ReadDevicePath(serial);
                break;
            case static_cast<int>(Command::EXIT):
                return 0;
            default:
                std::cout << "Invalid Command " << cmd << std::endl;
                continue;
        }

        std::cout << "Please enter \'Enter\' for next input" << std::endl;
        std::getline(std::cin, line);
    }

    return 0;
}
