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
    
    size_t offset = 0;
    
    std::cout << "Size Whole File: " << fileInfo.st_size << "\n";
    // ------ WAV Structure
    std::string chunkID = convertToASCII(getHexOnBufferByte(addr, offset, 4, 4, false));
    long chunkSize = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    std::string filedFormat = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    
    // ------ Format SubChunk
    std::string sbChunkID = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    long sbChunkSize = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    long sbFiledFormat = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    long sbNumChannel = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    long sbSampleRate = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    long sbByteRate = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    long sbBlockAlign = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    long sbBitPerSample = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 2, 2, true));
    
    std::cout << "Chunk: " << chunkID << "\n";
    std::cout << "Chunk Size: " << chunkSize << "\n";
    std::cout << "Chunk File Format: " << filedFormat << "\n";
    
    std::cout << "Sub Chunk ID: " << sbChunkID << "\n";
    std::cout << "Sub Chunk Size: " << sbChunkSize << "\n";
    std::cout << "Sub Chunk File Format: " << sbFiledFormat << "\n";
    std::cout << "Num Channel: " << sbNumChannel << "\n";
    std::cout << "Sample Rate: " << sbSampleRate << "\n";
    std::cout << "Byte Rate: " << sbByteRate << "\n";
    std::cout << "Block Align: " << sbBlockAlign << "\n";
    std::cout << "Bits Per Sample: " << sbBitPerSample << "\n";
    
    size_t valBeforeData = offset;
    
    bool isFirstStepCan = true;
    std::string aaa = "";
    while (offset < 1000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        {
            std::lock_guard<std::mutex> lock(mMutex);
            aaa = "";
            aaa = getHexOnBufferByte(addr, offset, offset + 4, 4, false);
            if (aaa == "64617461" || aaa == "44415441") {
                break;
            }
            
        }
    }
    offset -= 4;
    std::string dataChunkID = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    if (dataChunkID == "data" || dataChunkID == "DATA") {
        isFirstStepCan = true;
    }
    else
        isFirstStepCan = false;
    
    offset = valBeforeData;
    
    if (!isFirstStepCan) {
        aaa = "";
        while (offset < 1000) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            {
                std::lock_guard<std::mutex> lock(mMutex);
                aaa = "";
                offset -= 3;
                aaa = getHexOnBufferByte(addr, offset, offset + 4, 4, false);
                if (aaa == "64617461" || aaa == "44415441") {
                    break;
                }
            }
        }
        offset -= 4;
        dataChunkID = convertToASCII(getHexOnBufferByte(addr, offset, offset + 4, 4, false));
    }
    std::cout << "Data Chunk ID: " << dataChunkID << "\n";
    long dataChunkSize = convertToDecimal(getHexOnBufferByte(addr, offset, offset + 4, 4, true));
    size_t numSamples = dataChunkSize / sbBlockAlign;
    std::cout << "Data Chunk Size: " << dataChunkSize << "\n";
    std::cout << "Num Sample: " << numSamples << "\n";
    
    if (munmap(addr, fileInfo.st_size) == -1)
    {
        close(fd);
    }
    close(fd);
    
    if (cc != nullptr) {
        cc->onFileStructure(file_name);
    }
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
