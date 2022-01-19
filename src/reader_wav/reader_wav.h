#ifndef READ_WAV_H
#define READ_WAV_H

#include <any>
#include <iostream>

class IReaderWav {
public:
    virtual void onFileStructure(std::string item_name) = 0;
};

class ReaderWav {
    
public: 
    ReaderWav();
    void test();
    void onReadFileStructure(IReaderWav* cc, const std::string file_name);
    
};
#endif