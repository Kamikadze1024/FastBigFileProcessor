#include "inputfilereader.hpp"
#include <iostream>

namespace FileReader {

InputFileReader::InputFileReader(std::shared_ptr<
                                 Containers::ThreadsafeQueue<std::string>
                                 > strs, std::atomic<bool> &readComplete)
    : m_readStrs(strs), m_readComplete(readComplete) {
    m_needStop.store(false);
}

//немедленно остановить поток
void InputFileReader::stopThread() {
    m_needStop.store(true);
}

//прочитать входной файл
void InputFileReader::readInputFile() {
    //проверяю путь на доступность
    BfsPath p("testDataFile.txt");
    if(!boost::filesystem::exists(p)) {
        throw FileReaderException("Файл testDataFile.txt недоступен");
    }

    //получаю размер файла в байтах
    boost::uintmax_t size = boost::filesystem::file_size(p);

    //маплю файл
    m_f.open(p);

    //указатель на начало смапленного файла
    const char *pStartPoint = static_cast<const char *>(m_f.data());

    //массив, в который буду читать байты
    char arr[CONST_ARR_SZ];
    //очищаю массив
    memset(arr, 0, CONST_ARR_SZ);

    if(m_f.is_open()) {
        //размер файла не равен размеру мапа
        if(size != m_f.size()) {
            m_f.close();
            std::string excStr = "Размер мапа не равен размеру файла";
            throw FileReaderException(excStr);
        }

        //счетчик позиции байта в конкретной строке
        std::size_t strCntr = 0;
        for(boost::uintmax_t i = 0; i < size; i++) {

            //нужно завершить работу
            if(m_needStop.load()) {
                break;
            }

            //считываю блок
            memcpy(arr, (pStartPoint + i), CONST_BLK_SZ);
            i += CONST_BLK_SZ;
            strCntr += CONST_BLK_SZ;

            //дочитываю до конца строки
            for(boost::uintmax_t j = i; j < size; j++) {
                //если встретил конец строки, перешел на следующую строку
                if(*(pStartPoint + j) == '\n') {
                    std::string res = arr;
                    //std::cout << res << std::endl;
                    //опускаю строку в очередь
                    m_readStrs->push(res);

                    strCntr = 0;
                    memset(arr, 0, CONST_ARR_SZ);
                    break;
                }

                arr[strCntr] = *(pStartPoint + j);
                strCntr++;
                i = j;
            }
        }

        std::cout << std::endl;

        //размапливаю файл
        m_f.close();

        //чтение закончил, сообщаю об этом другим потокам
        m_readComplete.store(true);
    } else {
        std::string excMsg = "Файл не открыт";
        throw FileReaderException(excMsg);
    }
}

}
