/*
 * Класс для чтения файлов
 * Кашицын Денис, 2021
 * <kamikadze1024@yandex.ru>
 */
#ifndef INPUTFILEREADER_HPP
#define INPUTFILEREADER_HPP

/*
 * Все константы подогнаны под длину строки.
 * Принимаю, что все данные в файле корректны
 * и "мусора" там быть не должно.
 */

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <map>

namespace FileReader {

//исключение парсинга json
class JsonParsingException final {
private:
    //сообщение об ошибке
    std::string m_msg;

public:
    JsonParsingException(std::string msg) : m_msg(msg) {};

    //возвратить сообщение об ошибке
    std::string what() { return m_msg; }
};

//исключение читальщика
class FileReaderException final {
private:
    //сообщение об ошибке
    std::string m_msg;
public:
    FileReaderException(std::string msg) : m_msg(msg) {};

    //возвратить сообщение об ошибке
    std::string what() { return m_msg; }
};

//читальщик
class InputFileReader final {
private:
    //константы
    static const int CONST_BLK_SZ = 70; //размер считываемого блока в байтах
    static const int CONST_ARR_SZ = 100; //размер массива

    //псевдонимы
    using BfsPath = boost::filesystem::path;
    using BiosMfs = boost::iostreams::mapped_file_source;

    //мап файла
    BiosMfs m_f;

    //контейнер для сумм расстояний по направлениям
    std::shared_ptr<std::map<std::string, double>> m_summs;

    /*
     * Временно, в этом классе, будет парсинг json.
     */
    void parseJson(std::string jsonMsg);

    //получить значение поля
    std::string getFieldValue(boost::property_tree::ptree const& pt,
                  std::string field);
public:
    InputFileReader(std::shared_ptr<std::map<std::string, double>>);

    /*
     * Прочитать входной файл.
     * В случае отсутствия файла,
     * несоответствия размеров мапа и
     * файла - вернет исключение.
     */
    void readInputFile();
};

}

#endif // INPUTFILEREADER_HPP
