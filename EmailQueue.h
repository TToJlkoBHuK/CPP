#ifndef EMAIL_QUEUE_H
#define EMAIL_QUEUE_H

#include "QuadraticEquation.h"
#include <queue>
#include <tuple>
#include <string>

class EmailQueue {
private:
    std::queue<std::tuple<QuadraticEquation, std::vector<double>, std::string>> emails;
public:
    void addEmail(const QuadraticEquation& eq, const std::vector<double>& solution, const std::string& studentName);
    std::queue<std::tuple<QuadraticEquation, std::vector<double>, std::string>> getEmails() const;
};

#endif