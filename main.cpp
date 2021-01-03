#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

using namespace std;

int main() {

    //проверяю путь на доступность
    boost::filesystem::path p("shortTestData.txt");
    if(!boost::filesystem::exists(p)) {
        std::cout << "Файл shortTestData.txt недоступен" << std::endl;
        return 0;
    }

    //получаю размер файла в байтах
    boost::uintmax_t size = boost::filesystem::file_size(p);

    //TODO: мап и файл одинакового размера???

    boost::iostreams::mapped_file_source f;
    //маплю файл
    f.open(p);

    //указатель на начало смапленного файла
    const char *pStartPoint = static_cast<const char *>(f.data());
    char arr[256];
    memset(arr, 0, 256);

    if(f.is_open()) {
        //счетчик позиции байта в конкретной строке
        std::size_t strCntr = 0;
        for(boost::uintmax_t i = 0; i < size; i++) {
            arr[strCntr] = *(pStartPoint + i);
            //если встретил конец строки, перешел на следующую строку
            if(*(pStartPoint + i) == '\n') {
                std::string res = arr;
                std::cout << res;
                strCntr = 0;
                memset(arr, 0, 256);
                continue;
            }
            strCntr++;
        }

        //размапливаю файл
        f.close();
    } else {
        std::cout << "Файл не открыт" << std::endl;
    }

    return 0;
}
