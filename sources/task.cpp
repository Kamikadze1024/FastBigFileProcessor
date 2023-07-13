#include "../include/task.hpp"
#include <sstream>

namespace Task {

Task::Task(std::shared_ptr<std::map<std::string, double>> summs,
           std::shared_ptr<Containers::ThreadsafeQueue<std::string>> strings,
           std::atomic<bool> &needStop,
           std::shared_ptr<ThPool::ThreadPool> thdPool)
    : m_flagCanStop(needStop),
      m_summs(summs),
      m_strings(strings),
      m_thdPool(thdPool) {
    m_flag.store(true);
    m_futuresCntr = 0;
}

Task::~Task() {
    //дообработать имеющиеся фьючерсы если остались необработанные
    if(m_futuresCntr < (m_futuresVect.size() - 1)) {
        for(; m_futuresCntr < m_futuresVect.size(); m_futuresCntr++) {
            //коплю суммы
            auto resultPair = m_futuresVect.at(m_futuresCntr).get();

            if((resultPair.first == "1") || (resultPair.first == "-1")) {
                m_summs.get()->at(resultPair.first)
                        = m_summs.get()->at(resultPair.first)
                            + resultPair.second;
            }
        }
    }
}

//немедленно остановить поток
void Task::stopThread() {
    m_flag.store(false);
}

std::string Task::getFieldValue(boost::property_tree::ptree const& pt,
                                      std::string field) {
    boost::property_tree::ptree::const_iterator end = pt.end();
    boost::property_tree::ptree::const_iterator it;

    std::string res;
    for(it = pt.begin(); it != end; ++it) {
        if(it->first.compare(field) == 0) {
            res = it->second.get_value<std::string>();
            break;
        }
    }

    return res;
}

void Task::parseJson(std::string jsonMsg) {
    std::istringstream iss(jsonMsg);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(iss, pt);

    if(pt.empty()) {
        std::string excMsg = "Дерево свойств пусто";
        throw JsonParsingException(excMsg);
    }

    std::string speed = getFieldValue(pt, "speed");
    double dSpeed     = std::stod(speed);

    std::string time  = getFieldValue(pt, "time");
    double dTime      = std::stod(time);

    double distance = dSpeed * dTime;

    std::string direction = getFieldValue(pt, "direction");

    m_summs.get()->at(direction) = m_summs.get()->at(direction)
            + distance;
}

//задача из строки получить пару: направление; дистанция
//по-факту, полный аналог parseJson
std::pair<std::string, double> Task::calculateDistance(
        std::string &jsonStr) {

    std::istringstream iss(jsonStr);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(iss, pt);

    if(pt.empty()) {
        std::string excMsg = "Дерево свойств пусто";
        throw JsonParsingException(excMsg);
    }

    std::string speed = getFieldValue(pt, "speed");
    double dSpeed     = std::stod(speed);

    std::string time  = getFieldValue(pt, "time");
    double dTime      = std::stod(time);

    double distance = dSpeed * dTime;

    std::string direction = getFieldValue(pt, "direction");

    std::pair<std::string, double> res;
    res.first  = direction;
    res.second = distance;
    return res;
}

void Task::processAllTasks() {
    while(m_flag.load()) {
        //цикл передачи задач пулу потоков
        while(!m_strings->empty()) {
            std::string str;
            m_strings->wait_and_pop(str);

            //пакую в задачу и передаю в пул и получаю фьючерс
            std::future<std::pair<std::string, double>> taskFut
                    = m_thdPool->submit(std::bind(&Task::calculateDistance,
                                        this,
                                        std::move(str)));

            //сохраняю фьючерс в вектор
            m_futuresVect.push_back(std::move(taskFut));
        }

        /*
         * Цикл получения результата
         * пробегаюсь по вектору фьючерсов, начиная с нуля.
         * Т.к. новые фьючерсы добавляются вконец, то не
         * будет ситуации, когда я "выйду" за границы
         * массивов
         */
        for(; m_futuresCntr < m_futuresVect.size(); m_futuresCntr++) {
            //коплю суммы
            auto resultPair = m_futuresVect.at(m_futuresCntr).get();

            if((resultPair.first == "1") || (resultPair.first == "-1")) {
                m_summs.get()->at(resultPair.first)
                        = m_summs.get()->at(resultPair.first)
                            + resultPair.second;
            } else {
                std::string excMsg = "Некорректный direction в строке";
                throw JsonParsingException(excMsg);
            }
        }

        //не завершать поток раньше чем все суммы будут подсчитаны
        if((m_flagCanStop.load()) && (m_futuresCntr >= m_futuresVect.size())) {
            std::cout << "m_futuresCntr = " << m_futuresCntr << std::endl;
            m_flag.store(false);
        }
    }
}

}
