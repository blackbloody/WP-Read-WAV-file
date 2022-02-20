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
    
//    uint8_t* x;
//    for (size_t i = 0; i < countMultiplySampleWithChannel; i++) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));
//        {
//            std::lock_guard<std::mutex> lock(mMutex);
//            
//            int32_t sample = 0;
//            uint8_t* sampleByte = (uint8_t*)(&sample);
//            //char* sampleByte = (char*)(&sample);
//            
//            for (size_t b = 0; b < bytesPerSample; b++) {
//                sampleByte[b] = addr[offset++];
//            }
//            
//            if (bytesPerSample == 1) {
//                sample += CHAR_MIN;
//            }
//            // Read the MSB sign bit
//            else if (sampleByte[bytesPerSample - 1] & 0X80) {
//                for (size_t b = bytesPerSample; b < sizeof(sample); b++) {
//                    sampleByte[b] = 0XFF;
//                }
//            }
//            
//            //x[i] = (uint8_t)sample;
//            
//        }
//        std::cout << "\tIndex: " << i << "\n";
//    }
//    std::cout << "Last Offset: " << offset << "\n";
    
    if (munmap(addr, fileInfo.st_size) == -1)
    {
        close(fd);
    }
    close(fd);
    
    if (cc != nullptr) {
        cc->onFileStructure(obj);
    }
}

void ReaderWav::onTimelapse(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    {
        std::lock_guard<std::mutex> lock(mMutex);
        
        std::cout << "/////////\n";
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
        
    //    std::cout << "Elapsed time in seconds: " << 
    //        std::chrono::duration_cast<std::chrono::seconds > (end - start).count() << " sec" << std::endl;
        std::cout << "/////////\n";
    }
}

/*
void ReaderWav::onDemo(IReaderWav* cc) {
    
    struct stat fileInfo;
    uint8_t* addr;
    std::string file_name = "/home/venom/Downloads/Doremifasolasido.wav";
    const char* fileName = file_name.c_str();
    int fd = open(fileName, O_RDONLY);
    posix_fadvise(fd, 0, 0, 1);
    fstat(fd, &fileInfo);
    addr = static_cast<uint8_t*>(mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0));
    size_t length = fileInfo.st_size;
    size_t offset = 0;
    WAV obj = setWAV(addr, offset);
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    ////// =================================
    
    std::cout << "File size: " << fileInfo.st_size << "\n";
    std::cout << "File data size: " << obj.data_chunk_size << "\n\n";
    
    std::cout << "Byte Rate: " << obj.byte_rate << "\n";
    
    long frameSizeinByte = obj.channel * obj.bytes_per_sample();
    std::cout << "Frame size in byte: " << frameSizeinByte << "\n";
    
    long analogRate = frameSizeinByte * obj.sample_rate;
    std::cout << "Anolog rate: " << analogRate << " bytes/sec" << "\n";
    std::cout << "Byte Per Sec: " << obj.size_sample_in_sec() << "\n";
    
    long bytePer100Milis = analogRate * (0.1/1);
    std::cout << "Byte Per 100 Milis: " << bytePer100Milis << "\n";
    
    long unsigned int period_size = (long unsigned int)(bytePer100Milis / frameSizeinByte);
    std::cout << "Period size: " << period_size << "\n";
    
    long period = 2;
    long unsigned int buffer_size = (long unsigned int)(period * period_size);
    std::cout << "Buffer size: " << buffer_size << "\n";
    
    /////// =================================
    
    int dir;
    unsigned int sample_rate = (unsigned int)obj.sample_rate;
    unsigned int pcm;
    snd_pcm_t *handler;
    snd_pcm_hw_params_t *params;
    snd_pcm_sw_params_t *sw_params;
    snd_pcm_uframes_t frames = (snd_pcm_uframes_t)period_size;
    
    if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
        std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
        return;
    }
    
    if (snd_pcm_hw_params_malloc(&params) < 0) 
        std::cout << "cannot allocate hardware parameter structure: " << snd_strerror(pcm) << "\n";
    if (snd_pcm_hw_params_any(handler, params) < 0)
        std::cout << "cannot initialize hardware parameter structure: " << snd_strerror(pcm) << "\n";
    
    if (snd_pcm_hw_params_set_access(handler, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
        std::cout << "cannot set interleaved mode: " << snd_strerror(pcm) << "\n";
    if (snd_pcm_hw_params_set_format(handler, params, SND_PCM_FORMAT_S16_LE) < 0)
        std::cout << "cannot set format. " << snd_strerror(pcm) << "\n";
    if (snd_pcm_hw_params_set_channels(handler, params, obj.channel) < 0) 
        std::cout << "cannot set channels number. " << snd_strerror(pcm) << "\n";
    if (snd_pcm_hw_params_set_rate_near(handler, params, &sample_rate, 0) < 0) 
        std::cout << "cannot set rate. " << snd_strerror(pcm) << "\n";
    
//    if (snd_pcm_hw_params_set_periods(handler, params, 4, 0) < 0)
//        std::cout << "cannot set periods" << "\n";
    if (snd_pcm_hw_params_set_buffer_size(handler, params, (19872 * 4)) < 0)
        std::cout << "cannot set buffer size" << "\n";
    if (snd_pcm_hw_params_set_period_size(handler, params, 19872, 0) < 0)
        std::cout << "cannot set period size: " << "\n";
    if (pcm = snd_pcm_hw_params(handler, params) < 0)
        std::cout << "Can't set harware parameters. " << snd_strerror(pcm) << "\n";
    
    std::cout << "Period size: " << snd_pcm_hw_params_get_period_size(params, &frames, &dir) << "\n\n";
    
    pcm = snd_pcm_writei(handler, addr + offset, obj.data_chunk_size);
    std::cout << "buffer played: " << pcm << "\n";
    if (pcm == -EPIPE)
        snd_pcm_prepare(handler);
    else if (pcm < 0)
        std::cout << "cannot write to PCM device: " << snd_strerror(pcm) << "\n";
    
    snd_pcm_drain(handler);
    snd_pcm_close(handler);
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (munmap(addr, fileInfo.st_size) == -1) {
        close(fd);
    }
    close(fd);
}
*/

void ReaderWav::onDemo(IReaderWav* cc) {
    
    struct stat fileInfo;
    uint8_t* addr;
    std::string file_name = "/home/venom/Downloads/Doremifasolasido.wav";
    const char* fileName = file_name.c_str();
    int fd = open(fileName, O_RDONLY);
    posix_fadvise(fd, 0, 0, 1);
    fstat(fd, &fileInfo);
    addr = static_cast<uint8_t*>(mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0));
    size_t length = fileInfo.st_size;
    size_t offset = 0;
    WAV obj = setWAV(addr, offset);
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    unsigned int tmp;
    int pcm;
    snd_pcm_t *handler;
    
    if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
        std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
        return;
    }
    
    unsigned int channels = obj.channel;
    unsigned int sampleRate = obj.sample_rate;
    int isResampling = 1;
    //unsigned int latency_duration_milis = 3604897959;
    //unsigned int latency_duration_milis = 1000000000;
    //unsigned int latency_duration_milis = 3604000;
    //unsigned int latency_duration_milis = 3604897;
    //unsigned int latency_duration_milis = 3000000;
    unsigned int latency_duration_milis = 3000;
    
    if ((pcm = snd_pcm_set_params(handler, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 
        channels, sampleRate, isResampling, latency_duration_milis)) < 0)
            std::cout << "cannot set sound parameter" << "\n";
            
    snd_pcm_uframes_t frames, count;
    snd_pcm_uframes_t bufsize, period_size;
    frames = 0;
    count = 0;
    count = obj.offset_first_sample;
    
    snd_pcm_prepare(handler);
    
    snd_pcm_get_params(handler, &bufsize, &period_size);
    snd_pcm_uframes_t size_play = obj.num_sample_per_second() / 2;
    
    
    std::cout << "Period size: " << period_size << "\n";
    std::cout << "Buffer size: " << bufsize << "\n";
    std::cout << "Sample size: " << obj.num_sample() << "\n";
    std::cout << "Played size: " << size_play << "\n\n";
    
    
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    
    int i = 0;
    while (i < 6) {
        
        if (i > 0) {
            count += obj.num_sample_per_second();
            count += 1;
        }
        frames = snd_pcm_writei(handler, addr + count, size_play);
        onTimelapse(start, std::chrono::steady_clock::now());
        if (frames == -EPIPE) {
            snd_pcm_prepare(handler);
        }
        if (frames < 0)
            frames = snd_pcm_recover(handler, frames, 0);
        if (frames < 0) {
            std::cout << "Error: " << snd_strerror(frames);
        }
        
        i++;
    }
    /*
    frames = snd_pcm_writei(handler, addr + count, size_play);
    onTimelapse(start, std::chrono::steady_clock::now());
    if (frames == -EPIPE) {
        snd_pcm_prepare(handler);
    }
    if (frames < 0)
        frames = snd_pcm_recover(handler, frames, 0);
    if (frames < 0) {
        std::cout << "Error: " << snd_strerror(frames);
    }
    //
    count += obj.num_sample_per_second();
    count += 1;
    frames = snd_pcm_writei(handler, addr + count, size_play);
    onTimelapse(start, std::chrono::steady_clock::now());
    
    if (frames == -EPIPE) {
        snd_pcm_prepare(handler);
    }
    if (frames < 0)
        frames = snd_pcm_recover(handler, frames, 0);
    if (frames < 0) {
        std::cout << "Error: " << snd_strerror(frames);
    }
    //
    count += obj.num_sample_per_second();
    count += 1;
    frames = snd_pcm_writei(handler, addr + count, size_play);
    onTimelapse(start, std::chrono::steady_clock::now());
    
    if (frames == -EPIPE) {
        snd_pcm_prepare(handler);
    }
    if (frames < 0)
        frames = snd_pcm_recover(handler, frames, 0);
    if (frames < 0) {
        std::cout << "Error: " << snd_strerror(frames);
    }
    */
    
    snd_pcm_drain(handler);
    snd_pcm_close(handler);
    
    
    
    std::cout << "\n _end_ \n";
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (munmap(addr, fileInfo.st_size) == -1) {
        close(fd);
    }
    close(fd);
}