#include "inputfilereader.hpp"
#include <iostream>
#include <sstream>

namespace FileReader {

InputFileReader::InputFileReader(std::shared_ptr<std::map<std::string, double>>
                                 summs) : m_summs(summs) { }

//прочитать входной файл
void InputFileReader::readInputFile() {
    //проверяю путь на доступность
    BfsPath p("shortTestData.txt");
    if(!boost::filesystem::exists(p)) {
        throw FileReaderException("Файл shortTestData.txt недоступен");
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

            //считываю блок
            memcpy(arr, (pStartPoint + i), CONST_BLK_SZ);
            i += CONST_BLK_SZ;
            strCntr += CONST_BLK_SZ;

            //дочитываю до конца строки
            for(boost::uintmax_t j = i; j < size; j++) {
                //если встретил конец строки, перешел на следующую строку
                if(*(pStartPoint + j) == '\n') {
                    std::string res = arr;
                    std::cout << res;

                    /*
                     * Временно, паршу json прямо здесь
                     */
                    try {
                        parseJson(res);
                    }  catch (JsonParsingException &e) {
                        throw FileReaderException(e.what());
                    }


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
    } else {
        std::string excMsg = "Файл не открыт";
        throw FileReaderException(excMsg);
    }
}

std::string InputFileReader::getFieldValue(boost::property_tree::ptree const& pt,
                                      std::string field) {
    boost::property_tree::ptree::const_iterator end = pt.end();
    boost::property_tree::ptree::const_iterator it;

    std::string res;
    for(it = pt.begin(); it != end; ++it) {
        if(it->first.compare(field) == 0) {
            res = it->second.get_value<std::string>();
            break;
        }
    }

    return res;
}

void InputFileReader::parseJson(std::string jsonMsg) {
    std::istringstream iss(jsonMsg);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(iss, pt);

    if(pt.empty()) {
        std::string excMsg = "Дерево свойств пусто";
        throw JsonParsingException(excMsg);
    }

    std::string speed = getFieldValue(pt, "speed");
    double dSpeed     = std::stod(speed);

    std::string time  = getFieldValue(pt, "time");
    double dTime      = std::stod(time);

    double distance = dSpeed * dTime;

    std::string direction = getFieldValue(pt, "direction");

    m_summs.get()->at(direction) = m_summs.get()->at(direction)
            + distance;
}

}
