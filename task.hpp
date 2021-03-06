/*
 * Класс задачи.
 * Задача представляет собой:
 * 1) Парсинг строки
 * 2) Вычисление расстояния
 * 3) Возвращение пары: direction, расстояние.
 * Кашицын Денис, 2020
 */

#ifndef TASK_HPP
#define TASK_HPP

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>
#include <atomic>
#include <vector>
#include <memory>
#include "threadsafequeue.hpp"
#include "threadpool.hpp"

namespace Task {

//исключение парсинга json
class JsonParsingException final {
private:
    //сообщение об ошибке
    std::string m_msg;

public:
    JsonParsingException(std::string msg) : m_msg(msg) {};

    //возвратить сообщение об ошибке
    std::string what() { return m_msg; }
};

class Task {
private:
    //флаг работы
    std::atomic<bool>                                         m_flag;

    //флаг необходимости остановиться
    std::atomic<bool>                                        &m_flagCanStop;

    //контейнер для сумм расстояний по направлениям
    std::shared_ptr<std::map<std::string, double>>            m_summs;

    //потокобезопасная очередь
    std::shared_ptr<Containers::ThreadsafeQueue<std::string>> m_strings;

    //указатель на пул потоков
    std::shared_ptr<ThPool::ThreadPool>                       m_thdPool;

    //счетчик обработанных futures
    unsigned int                                              m_futuresCntr;

    //распарсить json
    void parseJson(std::string jsonMsg);

    //получить значение поля
    std::string getFieldValue(boost::property_tree::ptree const& pt,
                  std::string field);

    //вектор фьючерсов
    std::vector<std::future<std::pair<std::string, double>>> m_futuresVect;

    //задача из строки получить пару: направление; дистанция
    std::pair<std::string, double> calculateDistance(
            std::string &jsonStr);

public:
    Task(std::shared_ptr<std::map<std::string, double>>,
         std::shared_ptr<Containers::ThreadsafeQueue<std::string>>,
         std::atomic<bool> &,
         std::shared_ptr<ThPool::ThreadPool>);

    ~Task();

    //обработать все заявки
    void processAllTasks();

    //немедленно остановить поток
    void stopThread();
};

}

#endif // TASK_HPP
