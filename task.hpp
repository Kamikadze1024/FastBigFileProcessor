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
    //контейнер для сумм расстояний по направлениям
    std::shared_ptr<std::map<std::string, double>> m_summs;

    //распарсить json
    void parseJson(std::string jsonMsg);

    //получить значение поля
    std::string getFieldValue(boost::property_tree::ptree const& pt,
                  std::string field);
public:
    Task();
};

}

#endif // TASK_HPP
