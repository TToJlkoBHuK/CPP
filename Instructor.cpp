#include "Instructor.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>


void Instructor::initializeStudents(const EmailQueue& queue) {
    auto emails = queue.getEmails();
    while (!emails.empty()) {
        const auto& email = emails.front();
        const std::string& studentName = std::get<2>(email);
        allStudents.insert(studentName);
        emails.pop();
    }
    for (const auto& name : allStudents) {
        results[name] = 0;
    }
}

bool compareSolutions(const std::vector<double>& correct, const std::vector<double>& student) {
    if (correct.size() != student.size()) return false;
    for (size_t i = 0; i < correct.size(); ++i) {
        if (std::abs(correct[i] - student[i]) > 1e-6) return false;
    }
    return true;
}

void Instructor::checkEmails(const EmailQueue& queue) {
    initializeStudents(queue);

    auto emails = queue.getEmails();
    while (!emails.empty()) {
        const auto& email = emails.front();
        const QuadraticEquation& eq = std::get<0>(email);
        const std::vector<double>& studentSolution = std::get<1>(email);
        const std::string& studentName = std::get<2>(email);

        auto correctSolution = eq.solve();
        if (compareSolutions(correctSolution, studentSolution)) {
            results[studentName]++;
        }
        emails.pop();
    }
}

void Instructor::publishResults(const std::string& filename) const {
    if (results.empty()) {
        std::cout << "No results to publish.\n";
        return;
    }

    std::vector<std::pair<std::string, int>> sortedResults(results.begin(), results.end());
    std::sort(sortedResults.begin(), sortedResults.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    auto printTable = [&](std::ostream& os) {
        os << std::setw(20) << "Student Name" << " | " << std::setw(6) << "Correct" << "\n";
        os << "----------------------+--------\n";
        for (const auto& pair : sortedResults) {
            os << std::setw(20) << pair.first << " | " << std::setw(6) << pair.second << "\n";
        }
        };

    if (!filename.empty()) {
        std::ofstream file(filename);
        if (file.is_open()) {
            printTable(file);
            file.close();
            std::cout << "Results saved to " << filename << std::endl;
        }
        else {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
        }
    }
    else {
        printTable(std::cout);
    }
}