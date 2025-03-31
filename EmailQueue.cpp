#include "EmailQueue.h"

void EmailQueue::addEmail(const QuadraticEquation& eq, const std::vector<double>& solution, const std::string& studentName) {
    emails.push(std::make_tuple(eq, solution, studentName));
}

std::queue<std::tuple<QuadraticEquation, std::vector<double>, std::string>> EmailQueue::getEmails() const {
    return emails;
}