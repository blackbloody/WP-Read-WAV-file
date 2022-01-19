#include "reader_wav.h"

ReaderWav::ReaderWav() {
}

void ReaderWav::test() {
    std::cout << "fdkfmdkfd";
}

//template<class R>
void ReaderWav::onReadFileStructure(IReaderWav* cc, const std::string file_name) {
    cc->onFileStructure(file_name);
}
