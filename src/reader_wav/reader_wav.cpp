#include "reader_wav.h"


ReaderWav::ReaderWav() {
}

std::string ReaderWav::isProcessorLittleEndianness() {
    int j = 1;
    std::string littleEndianness = (*(char*)(&j) == 1) ? "true" : "false";
    return littleEndianness;
}

//template<class R>
void ReaderWav::onReadFileStructure(IReaderWav* cc, const std::string file_name) {
    
    struct stat fileInfo;
    uint8_t* addr;
    int fd;
    {
        //std::lock_guard<std::mutex> lock(mMutex);
        const char* fileName = file_name.c_str();
        fd = open(fileName, O_RDONLY);
        posix_fadvise(fd, 0, 0, 1);
        fstat(fd, &fileInfo);
        addr = static_cast<uint8_t*>(mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0));
    }
    
    std::cout << "Size Whole File: " << fileInfo.st_size << "\n";
    size_t offset = 0;
    WAV obj = setWAV(addr, offset);
    
    size_t containerCount = (obj.duration_milis() / 1000) + 1;
    size_t sizeAllSample = obj.num_sample_per_second() * obj.bytes_per_sample();
    std::vector<int> offsetItem;
    offsetItem.reserve(containerCount);
    for (size_t i = 0; i < containerCount; i++) 
    {
        offsetItem.push_back(offset);
        std::cout << "Sample Offset " << i << ": " << offset << "\n";
        offset += sizeAllSample;
    }    

    //uint8_t* allSample = new uint8_t[sizeAllSample * containerCount];
    //memcpy(allSample, addr + obj.offset_first_sample, sizeAllSample * containerCount);
    
    uint8_t* allSample = new uint8_t[sizeAllSample * containerCount];
    memcpy(allSample, addr + offsetItem[3], sizeAllSample * containerCount);
    std::cout << "Size 222: " << sizeAllSample * containerCount << "\n";
    
    if (cc != nullptr) {
        std::lock_guard<std::mutex> lock(mMutex);
        cc->onFileAllSampleToPlay(addr, offsetItem);   
    }
    
    if (munmap(addr, fileInfo.st_size) == -1)
    {
        close(fd);
    }
    close(fd);
    
    if (cc != nullptr) {
        cc->onFileStructure(obj);
    }
}

void ReaderWav::onTimelapse(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, snd_pcm_uframes_t frames, 
    std::string msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    {
        std::lock_guard<std::mutex> lock(mMutex);
        
        std::cout << "/////////\n";
        
        std::cout << msg;
        std::cout << frames << "\n";
        
        std::cout << "Elapsed time in nanoseconds: " << 
            std::chrono::duration_cast<std::chrono::nanoseconds > (end - start).count() << " ns" << std::endl;
        
        std::cout << "Elapsed time in microseconds: " << 
            std::chrono::duration_cast<std::chrono::microseconds > (end - start).count() << " µs" << std::endl;
            
        std::cout << "Elapsed time in milliseconds: " << 
            std::chrono::duration_cast<std::chrono::milliseconds > (end - start).count() << " ms" << std::endl;
        
        float millis = (float)std::chrono::duration_cast<std::chrono::milliseconds > (end - start).count();
        float sec = millis / 1000;
        
        std::cout << "Elapsed time in seconds: " << 
            sec << " sec" << std::endl;
            
        std::cout << "/////////\n";
    }
}