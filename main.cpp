#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include <atomic>
#include "inputfilereader.hpp"
#include "task.hpp"
#include "testfilegenerator.hpp"

using namespace std;

/*
 * Флаг, говорящий потоку парсинга о том, что
 * новых строк не будет, и можно завершать работу
 * когда обработает все имеющиеся в буфере строки
 */
std::atomic<bool> canStopParsing;

//поток чтения входного файла
int readerThdFunc(std::shared_ptr<FileReader::InputFileReader> reader) {
    try {
        reader->readInputFile();
    } catch (FileReader::FileReaderException &e) {
        std::cout << e.what() << std::endl;
        reader->stopThread();
        //останавливаю поток парсинга, чтобы не работал вхолостую
        canStopParsing.store(true);
        return false;
    }
    return true;
}

//поток парсинга и подсчета расстояния
int parserThdFunc(std::shared_ptr<Task::Task> task) {
    try {
        task->processAllTasks();
    } catch (Task::JsonParsingException &e) {
        std::cout << e.what() << std::endl;
        task->stopThread();
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {

    //простенький разбор аргументов командной строки
    if(argc > 1) {
        std::string readArg(argv[1]);
        if(readArg == "generate") {
            /*
             * Сбрасываю время для корректной генерации
             * случайных чисел.
             */
            std::srand(time(nullptr));

            //генерация тестового файла
            FileGenerator::TestFileGenerator tfg;

            //указатель на статистику файла
            std::shared_ptr<std::map<std::string, float>> crFileStat;
            crFileStat = nullptr;
            try {
                crFileStat = tfg.createTestFile();
            } catch (FileGenerator::TestFileGeneratorExc &e) {
                std::cout << e.what() << std::endl;
            }

            if(crFileStat == nullptr) {
                std::cout << "Некорректная генерация" << std::endl;
            }

            std::cout << "1 : " << crFileStat->at("1") << std::endl;
            std::cout << "-1 : " << crFileStat->at("-1") << std::endl;

            return 0;
        }
    }

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
    bool readRes  = readFut.get();
    bool parseRes = parseFut.get();

    if(!(readRes && parseRes)) {
        std::cout << "Ошибка времени выполнения" << std::endl;
        return -1;
    }

    //дождаться окончания парсинга и выдать результат
    std::cout << "========= Результат работы ========" << std::endl;
    std::cout << "Сумма расстояний направления 1 = "
        << summs->at("1") <<std::endl;
    std::cout << "Сумма расстояний направления -1 = "
        << summs->at("-1") <<std::endl;

    return 0;
}
