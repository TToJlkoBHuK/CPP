#ifndef STUDENT_H
#define STUDENT_H

#include "QuadraticEquation.h"
#include <vector>
#include <string>

class Student {
protected:
    std::string name;
public:
    Student(const std::string& name) : name(name) {}
    virtual std::vector<double> solve(const QuadraticEquation& eq) const = 0;
    std::string getName() const { return name; }
    virtual ~Student() = default;
};

class GoodStudent : public Student {
public:
    GoodStudent(const std::string& name) : Student(name) {}
    std::vector<double> solve(const QuadraticEquation& eq) const override;
};

class AverageStudent : public Student {
private:
    double probability;
public:
    AverageStudent(const std::string& name, double prob) : Student(name), probability(prob) {}
    std::vector<double> solve(const QuadraticEquation& eq) const override;
};

class BadStudent : public Student {
public:
    BadStudent(const std::string& name) : Student(name) {}
    std::vector<double> solve(const QuadraticEquation& eq) const override;
};

#endif