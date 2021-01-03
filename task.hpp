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
#include "threadsafequeue.hpp"

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
    std::atomic<bool>                                        m_flag;

    //контейнер для сумм расстояний по направлениям
    std::shared_ptr<std::map<std::string, double>>           m_summs;

    //потокобезопасная очередь
    std::shared_ptr<Containers::ThreadsafeQueue<std::string>> m_strings;

    //распарсить json
    void parseJson(std::string jsonMsg);

    //получить значение поля
    std::string getFieldValue(boost::property_tree::ptree const& pt,
                  std::string field);
public:
    Task(std::shared_ptr<std::map<std::string, double>>,
         std::shared_ptr<Containers::ThreadsafeQueue<std::string>>);

    //обработать все заявки
    void processAllTasks();
};

}

#endif // TASK_HPP
