#include "testfilegenerator.hpp"
#include <boost/filesystem.hpp>

namespace FileGenerator {

TestFileGenerator::TestFileGenerator() {
    m_dirSumm.reset(new std::map<std::string, double>());
}

TestFileGenerator::~TestFileGenerator() {

}

//запись набора строк в файл
void TestFileGenerator::writeGeneratedStrings() {

}

//генерация набора строк с конкретным направлением и к-вом
void TestFileGenerator::generateStringsPack(std::string &direction,
                                            unsigned int cnt) {
    for(unsigned int i = 0; i < cnt; i++) {

    }
}

//генерация набора строк
void TestFileGenerator::generateStrings() {
    /*
     * Псевдослучайно генерирую число строк для
     * направлений "1" и "-1", так чтобы их количество
     * в сумме было не менее CONST_TEST_FILE_SIZE
     */

    //половина интервала
    unsigned int half = this->CONST_TEST_FILE_SIZE / 2;
    unsigned int oneDir = 1 + static_cast<unsigned int>(rand()) % half;
    unsigned int minusOneDir = this->CONST_TEST_FILE_SIZE
            - oneDir;

    //спецификаторы направлений
    std::string dirOneSpec      = "1";
    std::string dirMinusOneSpec = "-1";

    //проверяю полученный результат на корректность
    if((minusOneDir + oneDir) != this->CONST_TEST_FILE_SIZE) {
        std::string excStr = "Некорректно сгенерировались \
            количества строк направлений 1 и -1";
        throw TestFileGeneratorExc(excStr);
    }

    //генерирую пакет строк с направлением "1"
    try {
        this->generateStringsPack(dirOneSpec, oneDir);
    }  catch (TestFileGeneratorExc &e) {
        throw;
    }

    //генерирую пакет строк с направлением "-1"
    try {
        this->generateStringsPack(dirMinusOneSpec, oneDir);
    }  catch (TestFileGeneratorExc &e) {
        throw;
    }
}

//единственный интерфейсный метод генерации тест. файла
std::shared_ptr<std::map<std::string, double>> TestFileGenerator
::createTestFile() {

    //есть уже файл?
    boost::filesystem::path testFilePath(CONST_TEST_FILE_PATH
                                         + CONST_TEST_FILE_NAME);

    //если есть - удаляю
    if(boost::filesystem::exists(testFilePath)) {
        boost::filesystem::remove(testFilePath);
    }

    //генерирую новый
    try {
        this->generateStrings();
    } catch (TestFileGeneratorExc &e) {
        throw;
    }

    //записываю в файл
    try {
        this->writeGeneratedStrings();
    }  catch (...) {
        throw;
    }

    //возвращаю пары направление, сумма потребителю
    return m_dirSumm;
}

}
