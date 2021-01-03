/*
 * Класс для чтения файлов
 * Кашицын Денис, 2021
 * <kamikadze1024@yandex.ru>
 */

#ifndef INPUTFILEREADER_HPP
#define INPUTFILEREADER_HPP

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

namespace FileReader {

class InputFileReader {
private:
    //константы
    static const int CONST_BLK_SZ = 70; //размер считываемого блока в байтах

    //псевдонимы
    using BfsPath = boost::filesystem::path;
    using BiosMfs = boost::iostreams::mapped_file_source;

public:
    InputFileReader();

    //прочитать входной файл
    void readInputFile();
};

}

#endif // INPUTFILEREADER_HPP
