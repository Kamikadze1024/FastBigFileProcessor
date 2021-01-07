#include "testfilegenerator.hpp"

namespace FileGenerator {

TestFileGenerator::TestFileGenerator() {

}

TestFileGenerator::~TestFileGenerator() {

}

//единственный интерфейсный метод генерации тест. файла
std::shared_ptr<std::map<std::string, double>> TestFileGenerator
::createTestFile() {
    std::shared_ptr<std::map<std::string, double>> res;
    return res;
}

}
