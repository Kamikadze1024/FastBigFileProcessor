#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include "inputfilereader.hpp"

using namespace std;

int main() {
    //умный указатель на потокобезопасную очередь строк
    std::shared_ptr<Containers::ThreadsafeQueue<std::string>> strings;
    strings.reset(new Containers::ThreadsafeQueue<std::string>);

    //умный указатель на контейнер с суммами по-направлениям
    std::shared_ptr<std::map<std::string, double>> summs;
    summs.reset(new std::map<std::string, double>);

    //инициализирую контейнер
    summs->insert(std::pair<std::string, double>("1", 0));
    summs->insert(std::pair<std::string, double>("-1", 0));

    //инициализирую объект читателя
    //"Читатель" строк
    std::unique_ptr<FileReader::InputFileReader> ifr;
    ifr.reset(new FileReader::InputFileReader(strings));

    //читаю входной файл
    auto futRead = std::async(std::launch::async,
                              &FileReader::InputFileReader::readInputFile,
                              ifr.get());
    try {
        futRead.get();
    } catch (FileReader::FileReaderException &e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << "========= Результат работы ========" << std::endl;
    std::cout << "Сумма расстояний направления 1 = "
        << summs->at("1") <<std::endl;
    std::cout << "Сумма расстояний направления -1 = "
        << summs->at("-1") <<std::endl;

    return 0;
}
