#include "reader_wav.h"


ReaderWav::ReaderWav() {
}

std::string ReaderWav::isProcessorLittleEndianness() {
    int j = 1;
    std::string littleEndianness = (*(char*)(&j) == 1) ? "true" : "false";
    return littleEndianness;
}

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
    
    std::cout << "size file: " << fileInfo.st_size << "\n";
    size_t offset = 0;
    WAV obj = setWAV(addr, offset);
    
    std::cout << "sample count : " << obj.num_sample() << "\n";
    std::cout << "sample rate: " << obj.sample_rate << "\n";
    
    // Calculate the length in seconds of the sample
    float T = (float)obj.num_sample() / (float)obj.sample_rate;
    std::cout << "a) " << T << "\n";
    
    // Calculate the number of equidistant points in time
    int n = (int) (T * obj.sample_rate) / 2;
    std::cout << "b) " << n << "\n";
    
    // Calculate the time interval at each equidistant point
    float h = (T / n);
    std::cout << "c) " << h << "\n";
    
    bool isBigEndian = false;
    unsigned int j=1;
    char *xx =(char*)&j;
    if (*xx==0)
        isBigEndian = true;
        
    uint8_t* bData = new uint8_t[obj.data_chunk_size];
    memcpy(bData, addr + offset, obj.data_chunk_size);
    
    // this array is the value of the signal at time i*h
    int x[n];
    for (size_t i = 0; i < n*2; i+=2) {
        int b1 = bData[i];
        int b2 = bData[i + 1];
        if (b1 < 0) b1 += 0x100;
        if (b2 < 0) b2 += 0x100;
        
        int value;
        
        //Store the data based on the original Endian encoding format
        if (!isBigEndian) value = (b1 << 8) + b2;
        else value = b1 + (b2 << 8);
        x[i/2] = value;
    }
    
    double amplitude = 0;
    double frequency = 0;
    
    std::cout << "time domain_: " << n << "\n";
    std::cout << "time domain: " << (n/2) << "\n";
    
    size_t caout__ = 1;
    
    double f[n/2];
    for (size_t j = 0; j < n/2; j++) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        {
            //std::lock_guard<std::mutex> lock(mMutex);
            
            double firstSum = 0;
            double secondSum = 0;
            
            for (size_t k = 0; k < n; k++) {
                double twoPInjk = ((2 * M_PI) / n) * (j * k);
                firstSum += x[k] * cos(twoPInjk);
                secondSum += x[k] * sin(twoPInjk);
            }
            
            f[j] = abs( sqrt(pow(firstSum,2) + pow(secondSum,2)) );
            
            amplitude = 2 * f[j]/n;
            frequency = j * h / T * (float)obj.sample_rate;
        }
        caout__++;
        
        
        std::cout << j << " <><><><><><><><><><> \n";
        std::cout << "Amp: " << amplitude << "\n";
        std::cout << "Freq: " << frequency << "\n";
        
        if (caout__ == 8)
            break;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    {
        std::lock_guard<std::mutex> lock(mMutex);
        std::cout << f[0] << "\n";
        std::cout << f[7] << "\n";   
    }
    
    std::cout << "End: " << caout__ << "\n";
    
    if (munmap(addr, fileInfo.st_size) == -1)
    {
        close(fd);
    }
    close(fd);
    
    if (cc != nullptr) {
        cc->onFileStructure(obj);
    }
}