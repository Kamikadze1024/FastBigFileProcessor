#include <iostream>
#include <memory>
#include "inputfilereader.hpp"

using namespace std;

int main() {
    std::unique_ptr<FileReader::InputFileReader> ifr;
    ifr.reset(new FileReader::InputFileReader());

    ifr->readInputFile();

    return 0;
}
