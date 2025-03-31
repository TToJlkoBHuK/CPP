#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H

#include "EmailQueue.h"
#include <map>
#include <set>
#include <string>

class Instructor {
private:
    std::map<std::string, int> results;
    std::set<std::string> allStudents;

    void initializeStudents(const EmailQueue& queue);

public:
    void checkEmails(const EmailQueue& queue);
    void publishResults(const std::string& filename = "") const;
};

#endif