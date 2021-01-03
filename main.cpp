#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

using namespace std;

static const int CONST_BLK_SZ = 70; //размер считываемого блока в байтах

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
        std::cout << "File   size = " << size << std::endl;
        std::cout << "Mapped size = " << f.size() << std::endl;

        //счетчик позиции байта в конкретной строке
        std::size_t strCntr = 0;
        for(boost::uintmax_t i = 0; i < size; i++) {

            //считываю блок
            memcpy(arr, (pStartPoint + i), CONST_BLK_SZ);
            i += CONST_BLK_SZ;
            strCntr += CONST_BLK_SZ;

            //дочитываю до конца строки
            for(boost::uintmax_t j = i; j < size; j++) {
                arr[strCntr] = *(pStartPoint + j);

                //если встретил конец строки, перешел на следующую строку
                if(*(pStartPoint + j) == '\n') {
                    std::string res = arr;
                    std::cout << res;
                    strCntr = 0;
                    memset(arr, 0, 256);
                    break;
                }
                strCntr++;
                i = j;
            }
        }

        //размапливаю файл
        f.close();
    } else {
        std::cout << "Файл не открыт" << std::endl;
    }

    return 0;
}
