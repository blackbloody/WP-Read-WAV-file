//#PRAGMA ONCE

#ifndef READ_WAV_H
#define READ_WAV_H

#include <iostream>
#include <sstream>

#include <mutex>
#include <chrono>
#include <thread>

// for memory mapping
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iomanip>

class IReaderWav {
public:
    virtual void onFileStructure(std::string item_name) = 0;
};

class ReaderWav {
    
public: 
    ReaderWav();
    void onReadFileStructure(IReaderWav* cc, const std::string file_name);
protected:
    mutable std::mutex mMutex;
    std::string isProcessorLittleEndianness();
    
    long convertToDecimal(const std::string hex);
    std::string convertToASCII(const std::string hex);
    
    std::string getHexOnBufferByte(uint8_t* data, size_t& offset, const size_t limit, const size_t length, bool isLittleEndian);
};
#endif