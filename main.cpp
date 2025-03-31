#include "QuadraticEquation.h"
#include "Student.h"
#include "EmailQueue.h"
#include "Instructor.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>

std::vector<QuadraticEquation> readEquations(const std::string& filename) {
    std::vector<QuadraticEquation> equations;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double a, b, c;
        if (iss >> a >> b >> c) {
            equations.emplace_back(a, b, c);
        }
    }
    return equations;
}

int main() {
    srand(42);

    auto equations = readEquations("equations.txt");

    GoodStudent good("Alice");
    AverageStudent average("Bob", 0.5);
    BadStudent bad("Charlie");

    EmailQueue queue;

    for (const auto& eq : equations) {
        queue.addEmail(eq, good.solve(eq), good.getName());
        queue.addEmail(eq, average.solve(eq), average.getName());
        queue.addEmail(eq, bad.solve(eq), bad.getName());
    }

    Instructor instructor;
    instructor.checkEmails(queue);
    instructor.publishResults();

    return 0;
}