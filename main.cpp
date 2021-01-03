#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>

using namespace std;

int main() {
    boost::iostreams::mapped_file_source f;
    f.open("shortTestData.txt");

    const char *pStartPoint = static_cast<const char *>(f.data());
    char arr[256];
    memset(arr, 0, 256);

    if(f.is_open()) {
        std::size_t strCntr = 0;
        for(std::size_t i = 0; i < f.size(); i++) {
            arr[strCntr] = *(pStartPoint + i);
            if(*(pStartPoint + i) == '\n') {
                std::string res = arr;
                std::cout << res;
                strCntr = 0;
                memset(arr, 0, 256);
                continue;
            }
            strCntr++;
        }
    } else {
        std::cout << "Файл не открыт" << std::endl;
    }

    return 0;
}
