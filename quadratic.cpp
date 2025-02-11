#include "quadratic.h"

void solveQuadratic(double a, double b, double c) {
    if (a == 0) {
        throw std::invalid_argument("'a' != 0 !");
    }

    double discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        double root1 = (-b + std::sqrt(discriminant)) / (2 * a);
        double root2 = (-b - std::sqrt(discriminant)) / (2 * a);
        std::cout << "x1 = " << root1 << "\n";
        std::cout << "x2 = " << root2 << "\n";
    }
    else if (discriminant == 0) {
        double root = -b / (2 * a);
        std::cout << "x = " << root << "\n";
    }
    else {
        std::cout << "complex value.\n";
    }
}

double getValidCoefficient(const std::string& prompt) {
    double coefficient;
    while (true) {
        std::cout << prompt;
        if (std::cin >> coefficient) {
            return coefficient;
        }
        else {
            std::cout << "Error: not number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}