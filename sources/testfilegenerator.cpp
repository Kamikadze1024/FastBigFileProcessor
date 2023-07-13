#include "../include/testfilegenerator.hpp"
#include <boost/filesystem.hpp>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace FileGenerator {

TestFileGenerator::TestFileGenerator() {
    //выделяю память под суммы
    m_dirSumm.reset(new std::map<std::string, float>());

    //инициализирую память под суммы
    m_dirSumm->insert(std::pair<std::string, float>("1", 0));
    m_dirSumm->insert(std::pair<std::string, float>("-1", 0));

    //инициализирую "кусок" строки
    m_firstPartStringPattern
            = "{\"timestamp\":\"1583020800577000000\",\"direction\"";
}

TestFileGenerator::~TestFileGenerator() {

}

//запись набора строк в файл
void TestFileGenerator::writeGeneratedStrings() {
    std::ofstream f(CONST_TEST_FILE_PATH  + CONST_TEST_FILE_NAME,
                   std::ios::app | std::ios::out );

    if(!f.is_open()) {
        std::string excMsg = "Не удалось открыть файл на запись";
        throw TestFileGeneratorExc(excMsg);
    }

    for(unsigned int i = 0; i < m_generatedStrings.size(); i++) {
        f << m_generatedStrings.at(i);
    }

    f.close();
}

/*
 * Сгенерировать число из диапазона [l;r]
 * с точностью в 2 знака после зпт.
 */
float TestFileGenerator::genFloatRandNum2(float l, float r) {
    int l_ = l * 100;
    int r_ = r * 100;
    return static_cast<float>(rand() % (r_ - l_) + l_) / 100;
}

/*
 * Сгенерировать число из диапазона [l;r]
 * с точностью в 4 знака после зпт.
 */
float TestFileGenerator::genFloatRandNum4(float l, float r) {
    int l_ = l * 10000;
    int r_ = r * 10000;
    return static_cast<float>(rand() % (r_ - l_) + l_) / 10000;
}

//генерация набора строк с конкретным направлением и к-вом
void TestFileGenerator::generateStringsPack(std::string &direction,
                                            unsigned int cnt) {
    for(unsigned int i = 0; i < cnt; i++) {
        //генерирую скорость (2 знака после запятой)
        float speed = genFloatRandNum2(0.01f, 100.00f);
        //генерирую время    (4 знака после запятой)
        float time  = genFloatRandNum4(0.0001f, 100.0000f);

        //вычисляю distance для подсчитанных speed и time
        float distance = speed * time;

        /*
         * Прибавляю distance к сумме направлений по данному
         * direction
         */
        m_dirSumm->at(direction) = m_dirSumm->at(direction)
                                    + distance;

        //формирую json строку
        std::stringstream ss;
        ss << m_firstPartStringPattern;
        ss << ":";
        ss << "\"";
        ss << direction;
        ss << "\",";
        ss << "\"speed\":\"";
        ss << std::setprecision(4);
        ss << speed;
        ss << "\",";
        ss << "\"time\":\"";
        ss << std::setprecision(6);
        ss << time;
        ss << "\"}";
        ss << std::endl;

        //превращаю ее в std::string
        std::string generatedString = ss.str();

        //кладу в буфер сгенерированных строк
        m_generatedStrings.push_back(std::move(generatedString));
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
        this->generateStringsPack(dirMinusOneSpec, minusOneDir);
    }  catch (TestFileGeneratorExc &e) {
        throw;
    }
}

//единственный интерфейсный метод генерации тест. файла
std::shared_ptr<std::map<std::string, float> > TestFileGenerator::createTestFile() {

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
    }  catch (TestFileGeneratorExc &e) {
        throw;
    }

    //возвращаю пары направление, сумма потребителю
    return m_dirSumm;
}

}
