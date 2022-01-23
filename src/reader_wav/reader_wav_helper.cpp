#include "reader_wav.h"

long ReaderWav::convertToDecimal(const std::string hex) {
    long result;
    std::stringstream ss(hex);
    ss >> std::hex >> result;
    return result;
    
}

std::string ReaderWav::convertToASCII(const std::string hex) {
    std::string ascii = "";
    for (size_t i = 0; i < hex.length(); i += 2) {
        //taking two characters from hex string
        std::string part = hex.substr(i, 2);
        //changing it into base 16
        char ch = std::stoul(part, nullptr, 16);
        //putting it into the ASCII string
        ascii += ch;
        }
    return ascii;
}

//std::string ReaderWav::getHexOnBufferByte(uint8_t* data, size_t* offset, size_t* limit) {
//    std::string tempHex = "ASD";
//    offset++;
////    for (; offset < limit; offset++) {
////        std::stringstream sss;
////        sss << std::hex << std::setfill('0');
////        sss << std::hex << std::setw(2) << static_cast<long>(data[offset[0]]);
////        tempHex += sss.str();
////        std::cout << offset[0] << "\n";
////    }
//    return tempHex;
//}