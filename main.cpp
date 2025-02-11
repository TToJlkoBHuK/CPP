#include "quadratic.h"

int main() {
    std::cout << "Programm for find value ax^2 + bx + c = 0\n";
    while (true) {
        try {
            double a = getValidCoefficient("a: ");
            double b = getValidCoefficient("b: ");
            double c = getValidCoefficient("c: ");

            solveQuadratic(a, b, c);

            std::cout << "\nWould you like more? (yes/no): ";
            std::string answer;
            std::cin >> answer;

            if (answer != "yes") {
                std::cout << "Thanks for using programm.\n";
                break;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}