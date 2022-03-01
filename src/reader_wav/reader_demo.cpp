#include "reader_wav.h"

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
    
    std::cout << "Data size: " << obj.data_chunk_size << "\n";
    std::cout << "Byte rate: " << obj.byte_rate << "\n";
    std::cout << "Sample details: " << obj.num_sample_per_second() << "\n";
    std::cout << "Sample edit: " << (((obj.num_sample_per_second()) * (obj.bit_per_sample / 8)) * obj.channel) << "\n";
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    unsigned int channels = obj.channel;
    unsigned int sampleRate = obj.sample_rate;
    
    unsigned int tmp;
    int pcm;
    snd_pcm_t *handler;
    snd_pcm_hw_params_t *params;
    
    if ((pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0))< 0) {
        std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
        return;
    }
    
    long microS = 100000;
    //long microS = 100000;
    pcm = snd_pcm_set_params(handler, 
        SND_PCM_FORMAT_S16, // format
        SND_PCM_ACCESS_RW_INTERLEAVED, // access
        channels, // channel
        sampleRate, // rate
        1, // 0 = disallow alsa-lib resample stream, 1 = allow resampling 
        microS // latency (0.5 seconds ... format is in microseconds)
        );
    if (pcm < 0) {
        std::cout << "cannot set hardware parameter : " << snd_strerror(pcm) << "\n";
        return;
    }
    
    snd_pcm_uframes_t bufsize, period_size;
    snd_pcm_get_params(handler, &bufsize, &period_size);
    std::cout << "Period size: " << period_size << "\n";
    std::cout << "Buffer size: " << bufsize << "\n";
    
    long samplePlay = obj.num_sample_per_second() * (0.1 / 1);
    long frames = (samplePlay * (obj.bit_per_sample / 8)) * obj.channel;
    std::cout << "Sample size: " << frames << "\n";
    
    while (1) {
        
        uint8_t* sample = new uint8_t[(long)frames];
        memcpy(sample, addr + offset, (long)frames);

        snd_pcm_uframes_t frames_play;
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        frames_play = snd_pcm_writei(handler, sample, samplePlay);
        onTimelapse(start, std::chrono::steady_clock::now(), frames_play, "");

        if (frames_play < 0)
            frames_play = snd_pcm_recover(handler, frames_play, 0);
        if (frames < 0) {
            std::cout << "cannot writei buffer: " << snd_strerror(frames);
            break;
        }
        offset += frames;
        if (offset > obj.data_chunk_size)
            break;
    
    }
    
    pcm = snd_pcm_drain(handler);
    if (pcm < 0)
        std::cout << "drain failed: " << snd_strerror(pcm) << "\n";
    snd_pcm_close(handler);
    
    std::cout << "\n _end_ \n";
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (munmap(addr, fileInfo.st_size) == -1) {
        close(fd);
    }
    close(fd);
}