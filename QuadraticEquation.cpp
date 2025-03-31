#include "QuadraticEquation.h"
#include <cmath>
#include <algorithm>

QuadraticEquation::QuadraticEquation(double a, double b, double c) : a(a), b(b), c(c) {}

std::vector<double> QuadraticEquation::solve() const {
    std::vector<double> roots;
    if (a == 0) {
        if (b == 0) return roots;
        roots.push_back(-c / b);
        return roots;
    }
    double discriminant = b * b - 4 * a * c;
    if (discriminant > 0) {
        double sqrtD = std::sqrt(discriminant);
        roots.push_back((-b + sqrtD) / (2 * a));
        roots.push_back((-b - sqrtD) / (2 * a));
        std::sort(roots.begin(), roots.end());
    }
    else if (discriminant == 0) {
        roots.push_back(-b / (2 * a));
    }
    return roots;
}