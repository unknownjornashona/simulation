#include <iostream>
#include <sstream>
#include <string>
#include <bitset>
#include <fstream>
#include <stdexcept>

class BaseConverter {
public:
    // 从十进制转换
    void convertFromDecimal(int decimal, int base) {
        switch (base) {
            case 2: // 二进制
                log("Converting to binary.");
                log("Binary: " + std::bitset<32>(decimal).to_string());
                break;
            case 8: // 八进制
                log("Converting to octal.");
                log("Octal: " + std::to_string(decimalToOctal(decimal)));
                break;
            case 10: // 十进制
                log("Decimal: " + std::to_string(decimal));
                break;
            case 16: // 十六进制
                log("Hexadecimal: " + toHex(decimal));
                break;
            default:
                throw std::invalid_argument("Invalid base for conversion.");
        }
    }

    // 转换为十进制
    int convertToDecimal(const std::string& number, int base) {
        int decimal = 0;
        std::istringstream ss(number);
        if (base == 2) {
            ss >> std::bitset<32>(number);
            decimal = (int)ss.str().to_ulong();
        } else if (base == 8) {
            ss >> std::oct >> decimal;
        } else if (base == 10) {
            ss >> decimal;
        } else if (base == 16) {
            ss >> std::hex >> decimal;
        } else {
            throw std::invalid_argument("Invalid base for conversion.");
        }
        
        log("Converted to decimal: " + std::to_string(decimal));
        return decimal;
    }

    // 保存日志
    void log(const std::string& message) {
        std::ofstream logFile("conversion_log.txt", std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Could not open log file.");
        }
        logFile << message << std::endl;
        logFile.close();
    }

private:
    int decimalToOctal(int decimal) {
        int octal = 0, place = 1;
        while (decimal != 0) {
            int remainder = decimal % 8;
            octal += remainder * place;
            decimal /= 8;
            place *= 10;
        }
        return octal;
    }

    std::string toHex(int decimal) {
        std::ostringstream ss;
        ss << std::hex << decimal;
        return ss.str();
    }
};

int main() {
    try {
        BaseConverter converter;
        int choice, base;
        std::string number;

        std::cout << "Choose conversion type:\n";
        std::cout << "1. Convert from Decimal\n";
        std::cout << "2. Convert to Decimal\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            // 从十进制转换
            int decimal;
            std::cout << "Enter a decimal number: ";
            std::cin >> decimal;

            std::cout << "Choose base to convert to:\n";
            std::cout << "2. Binary\n";
            std::cout << "8. Octal\n";
            std::cout << "10. Decimal\n";
            std::cout << "16. Hexadecimal\n";
            std::cin >> base;

            converter.convertFromDecimal(decimal, base);
        } else if (choice == 2) {
            // 转换为十进制
            std::cout << "Enter number: ";
            std::cin >> number;

            std::cout << "Enter base of the number (2, 8, 10, 16): ";
            std::cin >> base;

            int decimal = converter.convertToDecimal(number, base);
            std::cout << "Decimal: " << decimal << std::endl;
        } else {
            std::cout << "Invalid choice!" << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
