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
#include <iostream>
#include <map>
#include "threadsafequeue.hpp"

namespace FileReader {

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
    static const int CONST_BLK_SZ = 60; //размер считываемого блока в байтах
    static const int CONST_ARR_SZ = 100; //размер массива

    //псевдонимы
    using BfsPath = boost::filesystem::path;
    using BiosMfs = boost::iostreams::mapped_file_source;

    //мап файла
    BiosMfs m_f;

    //контейнер для прочитанных из файла строк
    std::shared_ptr<Containers::ThreadsafeQueue<std::string>> m_readStrs;

public:
    InputFileReader(std::shared_ptr<Containers::ThreadsafeQueue<std::string>>);

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
