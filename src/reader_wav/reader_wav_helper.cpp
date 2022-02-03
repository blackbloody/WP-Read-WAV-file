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

std::string ReaderWav::getHexOnBufferByte(uint8_t* data, size_t& offset, const size_t limit, const size_t length, bool isLittleEndian) { 
    std::string tempHex = "";
    //std::cout << "Offset: " << offset << "\n";
    for (; offset < limit; offset++) {
        std::stringstream sss;
        sss << std::hex << std::setfill('0');
        sss << std::hex << std::setw(2) << static_cast<long>(data[offset]);
        tempHex += sss.str();
    }
    if (isLittleEndian == true) {
        std::string b[length];
        std::string tempByteHex = "";
        
        size_t indexHex = 1;
        size_t indexStarter = length - 1;
        for (size_t i = 0; i < tempHex.size(); i++) {
            tempByteHex += tempHex[i];
            if (indexHex < 2)
                indexHex++;
            else {
                b[indexStarter] = tempByteHex;
                indexStarter--;
                tempByteHex = "";
                indexHex = 1;
            }
        }
        
        tempHex = "";
        for (size_t i = 0; i < length; i++)
            tempHex += b[i];
    }
    //std::cout << tempHex << "\n";
    return tempHex;
}

size_t ReaderWav::findOffsetDataHeader(uint8_t* data, size_t& offset, const size_t indicator_reduce) {
    
    size_t valBeforeData = offset;
    std::string aaa = "";
    while (offset < 1000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        {
            std::lock_guard<std::mutex> lock(mMutex);
            aaa = "";
            offset -= indicator_reduce;
            aaa = getHexOnBufferByte(data, offset, offset + 4, 4, false);
            if (aaa == "64617461" || aaa == "44415441") {
                break;
            }
            
        }
    }
    offset -= 4;
    std::string dataChunkID = convertToASCII(getHexOnBufferByte(data, offset, offset + 4, 4, false));
    if (dataChunkID == "data" || dataChunkID == "DATA")
        return offset - 4;
    else
        return 0 + findOffsetDataHeader(data, valBeforeData, 3);
}