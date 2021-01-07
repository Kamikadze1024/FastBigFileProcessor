/*
 * Класс генерации многомегабайтного
 * тестового файла.
 * Кашицын Денис <kamikadze1024@yandex.ru>
 * 2021
 */
#ifndef TESTFILEGENERATOR_HPP
#define TESTFILEGENERATOR_HPP

#include <string>
#include <map>
#include <memory>

namespace FileGenerator {

//исключение
class TestFileGeneratorExc final {
private:
    std::string m_msg;
public:
    TestFileGeneratorExc(std::string msg) : m_msg(msg) {}
    std::string what() const { return m_msg; }
};

//генератор тестового файла
class TestFileGenerator final {
private:
    /*
     * Константы
     */
    //путь к файлу
    const std::string CONST_TEST_FILE_PATH = "./";
    //имя файла
    const std::string CONST_TEST_FILE_NAME = "test_file2.txt";
    //размер тестового файла в строках
    const unsigned long int CONST_TEST_FILE_SIZE = 2000000;


public:
    TestFileGenerator();
    ~TestFileGenerator();

    //единственный интерфейсный метод генерации тест. файла
    std::shared_ptr<std::map<std::string, double>> createTestFile();

    //правило трех
    TestFileGenerator(TestFileGenerator &) = delete;
    TestFileGenerator(const TestFileGenerator &) = delete;
    TestFileGenerator& operator=(const TestFileGenerator &) = delete;
};

}

#endif // TESTFILEGENERATOR_HPP
