#include <Utils.hpp>

std::string intToHex(int number) {
    std::stringstream s;
    s << std::hex << number;  // Convert integer to hexadecimal
    return s.str();  // Return the hexadecimal string
}