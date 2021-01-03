#include <iostream>
#include <memory>
#include "inputfilereader.hpp"

using namespace std;

int main() {
    std::unique_ptr<FileReader::InputFileReader> ifr;
    ifr.reset(new FileReader::InputFileReader());

    try {
        ifr->readInputFile();
    } catch (FileReader::FileReaderException &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
