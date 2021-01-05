#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include <atomic>
#include "inputfilereader.hpp"
#include "task.hpp"

using namespace std;

/*
 * Флаг, говорящий потоку парсинга о том, что
 * новых строк не будет, и можно завершать работу
 * когда обработает все имеющиеся в буфере строки
 */
std::atomic<bool> canStopParsing;

//поток чтения входного файла
void readerThdFunc(std::shared_ptr<FileReader::InputFileReader> reader) {
    try {
        reader->readInputFile();
    } catch (FileReader::FileReaderException &e) {
        std::cout << e.what() << std::endl;
        return;
    }
}

//поток парсинга и подсчета расстояния
void parserThdFunc(std::shared_ptr<Task::Task> task) {
    try {
        task->processAllTasks();
    } catch (Task::JsonParsingException &e) {
        std::cout << e.what() << std::endl;
        return;
    }
}

int main() {
    //поток парсинга останавливать нельзя
    canStopParsing.store(false);

    //пул потоков
    std::shared_ptr<ThPool::ThreadPool> thdPool;
    thdPool.reset(new ThPool::ThreadPool());

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
    std::shared_ptr<FileReader::InputFileReader> ifr;
    ifr.reset(new FileReader::InputFileReader(strings,
                                              canStopParsing));

    //инициализирую объект - обработчик задач
    std::shared_ptr<Task::Task> taskProc;
    taskProc.reset(new Task::Task(summs,
                                  strings,
                                  canStopParsing,
                                  thdPool));

    //запускаю поток парсинга
    auto parseFut = thdPool->submit(std::bind(parserThdFunc, taskProc));
    //читаю входной файл
    auto readFut = thdPool->submit(std::bind(readerThdFunc, ifr));

    //по-факту, дождаться окончания выполнения задач
    readFut.get();
    parseFut.get();

    //дождаться окончания парсинга и выдать результат
    std::cout << "========= Результат работы ========" << std::endl;
    std::cout << "Сумма расстояний направления 1 = "
        << summs->at("1") <<std::endl;
    std::cout << "Сумма расстояний направления -1 = "
        << summs->at("-1") <<std::endl;

    return 0;
}
