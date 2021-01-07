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
#include <string>
#include <vector>

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
    const std::string CONST_TEST_FILE_PATH  = "./";
    //имя файла
    const std::string CONST_TEST_FILE_NAME  = "test_file2.txt";
    //размер тестового файла в строках
    const unsigned int CONST_TEST_FILE_SIZE = 3;

    //паттерн первой части строки
    std::string        m_firstPartStringPattern;

    /*
     * Умный указатель на пары direction, сумма.
     * Нужен для того, чтобы знать суммы по направлениям
     * в сгенерированном файле
     */
    std::shared_ptr<std::map<std::string, float>> m_dirSumm;

    //вектор строк для записи
    std::vector<std::string>                        m_generatedStrings;

    //генерация набора строк с конкретным направлением и к-вом
    void generateStringsPack(std::string &direction, unsigned int cnt);

    //генерация набора строк
    void generateStrings();

    //запись набора строк в файл
    void writeGeneratedStrings();

    /*
     * Сгенерировать число из диапазона [l;r]
     * с точностью в 2 знака после зпт.
     */
    float genFloatRandNum2(float l, float r);

    /*
     * Сгенерировать число из диапазона [l;r]
     * с точностью в 4 знака после зпт.
     */
    float genFloatRandNum4(float l, float r);
public:
    TestFileGenerator();
    ~TestFileGenerator();

    //единственный интерфейсный метод генерации тест. файла
    std::shared_ptr<std::map<std::string, float>> createTestFile();

    //правило трех
    TestFileGenerator(TestFileGenerator &) = delete;
    TestFileGenerator(const TestFileGenerator &) = delete;
    TestFileGenerator& operator=(const TestFileGenerator &) = delete;
};

}

#endif // TESTFILEGENERATOR_HPP
