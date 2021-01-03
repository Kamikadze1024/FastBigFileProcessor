#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include "inputfilereader.hpp"
#include "task.hpp"

using namespace std;

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
    ifr.reset(new FileReader::InputFileReader(strings));

    //инициализирую объект - обработчик задач
    std::shared_ptr<Task::Task> taskProc;
    taskProc.reset(new Task::Task(summs, strings));

    //читаю входной файл
    std::thread readerThread(readerThdFunc, ifr);
    std::thread parserThread(parserThdFunc, taskProc);

    //дождаться окончания чтения
    readerThread.join();

    //указать парсеру, что он может завершиться, когда закончит парсить
    taskProc->canStop();
    //дождаться окончания парсинга
    parserThread.join();


    std::cout << "========= Результат работы ========" << std::endl;
    std::cout << "Сумма расстояний направления 1 = "
        << summs->at("1") <<std::endl;
    std::cout << "Сумма расстояний направления -1 = "
        << summs->at("-1") <<std::endl;

    return 0;
}
