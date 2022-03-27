#include "reader_wav.h"

void ReaderWav::onSample() {
    
    unsigned char buffer[16*1024];/* some random data */
    
    int pcm;
    unsigned int i;
    snd_pcm_t *handler;
    snd_pcm_sframes_t frames;
    
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (i = 0; i < sizeof(buffer); i++)
        buffer[i] = random() & 0xff;
    
    pcm = snd_pcm_open(&handler, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    if (pcm < 0) {
        std::cout << "PLAYBACK OPEN ERROR: " << snd_strerror(pcm) << "\n";
        return;
    }
    
    pcm = snd_pcm_set_params(handler, 
        SND_PCM_FORMAT_U8, // format
        SND_PCM_ACCESS_RW_INTERLEAVED, // access
        1, // channel
        48000, // rate
        1, // 0 = disallow alsa-lib resample stream, 1 = allow resampling 
        500000 // latency (0.5 seconds ... format is in microseconds)
        );
    if (pcm < 0) {
        std::cout << "cannot set hardware parameter : " << snd_strerror(pcm) << "\n";
        return;
    }
    
    start = std::chrono::steady_clock::now();
    for (i = 0; i < 16; i++) {
        frames = snd_pcm_writei(handler, buffer, sizeof(buffer));
        if (frames < 0)
            frames = snd_pcm_recover(handler, frames, 0);
        if (frames < 0) {
            std::cout << "cannot writei buffer: " << snd_strerror(frames);
            break;
        }
    }
    onTimelapse(start, std::chrono::steady_clock::now(), "build-sample\n", 0, 0);
    
    pcm = snd_pcm_drain(handler);
    if (pcm < 0)
        std::cout << "drain failed: " << snd_strerror(pcm) << "\n";
    snd_pcm_close(handler);
}