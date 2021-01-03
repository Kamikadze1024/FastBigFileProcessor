#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>

using namespace std;

int main() {
    boost::iostreams::mapped_file_source f;
    f.open("shortTestData.txt");

    const char *pStartPoint = static_cast<const char *>(f.data());
    char arr[255];

    if(f.is_open()) {
        for(std::size_t i = 0; i < f.size(); i++) {
            arr[i] = *(pStartPoint + i);
            if(arr[i] == '\n') {
                std::string res = arr;
                std::cout << arr << std::endl;
            }
        }
    } else {
        std::cout << "Файл не открыт" << std::endl;
    }

    return 0;
}
