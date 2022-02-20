//#PRAGMA ONCE

#ifndef READ_WAV_H
#define READ_WAV_H

#include <iostream>
#include <sstream>

#include <mutex>
#include <chrono>
#include <thread>

#include <math.h>
#include <cmath>

// for memory mapping
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iomanip>
#include <vector>
#include <climits>

#include </usr/include/alsa/asoundlib.h>

#include <string.h>

#define PCM_DEVICE "default"

struct WAV {
    std::string chunk_id = "";
    long chunk_size = 0;
    std::string chunk_format = "";
    
    std::string sub_chunk_id = "";
    long sub_chunk_size = 0;
    std::string sub_chunk_format = "";
    
    long channel = 0;
    long sample_rate = 0;
    long byte_rate = 0;
    long block_align = 0;
    long bit_per_sample = 0;
    long data_chunk_size = 0;
    
    long offset_first_sample = 0;
    
    long num_sample() {
        if (data_chunk_size != 0 && block_align != 0) {
            long rtn = data_chunk_size / block_align;
            return rtn;
        }
        else
            return 0;
    }
    
    long num_sample_per_second() {
        if (sample_rate != 0 && channel != 0) {
            long rtn = sample_rate * channel;
            return rtn;
        }
        else
            return 0;
    }
    
    long duration_milis() {
        if (num_sample() != 0 && num_sample_per_second() != 0) {
            long rtn = (num_sample() / num_sample_per_second()) * channel;
            rtn *= 1000;
            return rtn;
        }
        else
            return 0;
    }
    
    long bytes_per_sample() {
        if (bit_per_sample != 0) {
            long rtn = bit_per_sample / 8;
            return rtn;
        }
        else
            return 0;
    }
    
    long size_container_sample() {
        if (channel != 0 && num_sample() != 0 && bytes_per_sample() != 0) {
            long rtn = channel * num_sample() * bytes_per_sample();
            return rtn;
        }
        else
            return 0;
    }
    
    long size_sample_with_channel() {
        if (channel != 0 && num_sample()) {
            long rtn = channel * num_sample();
            return rtn;
        }
        else
            return 0;
    }
    
    long size_sample_in_sec() {
        if (num_sample_per_second() != 0 && bytes_per_sample() != 0) {
            long rtn = num_sample_per_second() * bytes_per_sample();
            return rtn;
        }
        else
            return 0;
    }
};

class IReaderWav {
public:
    virtual void onFileStructure(WAV file) = 0;
    virtual void onFileAllSampleToPlay(uint8_t* all_sample_sound, std::vector<int> offsetItem) = 0;
    virtual void onSampleAmpl(int* valueAmpl) = 0;
};

class ReaderWav {
    
public: 
    ReaderWav();
    void onReadFileStructure(IReaderWav* cc, const std::string file_name);
    void onDemo(IReaderWav* cc);
protected:
    mutable std::mutex mMutex;
    std::string isProcessorLittleEndianness();
    
    // helper
    long convertToDecimal(const std::string hex);
    std::string convertToASCII(const std::string hex);
    std::string getHexOnBufferByte(uint8_t* data, size_t& offset, const size_t limit, const size_t length, bool isLittleEndian);
    size_t findOffsetDataHeader(uint8_t* data, size_t& offset, const size_t indicator_reduce);
    
    WAV setWAV(uint8_t* data, size_t& offset);
    
    void onTimelapse(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end);
};
#endif