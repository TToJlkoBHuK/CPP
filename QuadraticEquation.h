#ifndef QUADRATIC_EQUATION_H
#define QUADRATIC_EQUATION_H

#include <vector>

class QuadraticEquation {
private:
    double a, b, c;
public:
    QuadraticEquation(double a, double b, double c);
    std::vector<double> solve() const;
};

#endif