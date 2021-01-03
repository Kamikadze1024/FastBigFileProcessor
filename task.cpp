#include "task.hpp"
#include <sstream>

namespace Task {

Task::Task() {

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

}
