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
    
    WAV obj;
    
    size_t offset = 0;
    
    std::cout << "Size Whole File: " << fileInfo.st_size << "\n";
    // ------ WAV Structure
    obj.chunk_id = convertToASCII(getHexOnBufferByte(addr, offset, 4, 4, false));
    obj.chunk_size = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    obj.chunk_format = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    
    // ------ Format SubChunk
    obj.sub_chunk_id = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    obj.sub_chunk_size = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    obj.sub_chunk_format = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    obj.channel = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    obj.sample_rate = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    obj.byte_rate = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    obj.block_align = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    obj.bit_per_sample = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    
    offset = findOffsetDataHeader(addr, offset, 0);
    std::string dataChunkID = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    obj.data_chunk_size = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    obj.offset_first_sample = offset;
    
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
