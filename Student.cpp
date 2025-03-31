#include "Student.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

std::vector<double> GoodStudent::solve(const QuadraticEquation& eq) const {
    return eq.solve();
}

std::vector<double> AverageStudent::solve(const QuadraticEquation& eq) const {
    if (static_cast<double>(rand()) / RAND_MAX < probability) {
        return eq.solve();
    }
    else {
        std::vector<double> wrong = eq.solve();
        if (!wrong.empty()) {
            for (double& root : wrong) {
                root += (static_cast<double>(rand()) / RAND_MAX - 0.5) * 2;
            }
        }
        else {
            wrong.push_back(0);
        }
        return wrong;
    }
}

std::vector<double> BadStudent::solve(const QuadraticEquation& eq) const {
    return { 0 };
}