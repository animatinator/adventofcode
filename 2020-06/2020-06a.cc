#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

std::vector<std::string> ParseCustomsDeclarations(const std::string& file_path) {
    std::ifstream infile(file_path);
    std::string line;
    std::string current_declaration;
    std::vector<std::string> results;

    while (std::getline(infile, line)) {
        if (line.empty()) {
            results.push_back(current_declaration);
            current_declaration = "";
        } else {
            current_declaration.append(line);
        }
    }

    results.push_back(current_declaration);

    return results;
}

int CountAnsweredQuestions(const std::string& declaration) {
    std::unordered_set<char> answered_questions(declaration.begin(), declaration.end());
    return answered_questions.size();
}

int GetSumOfAnsweredQuestions(const std::vector<std::string>& declarations) {
    return std::accumulate(
        std::next(declarations.begin()), declarations.end(),
        CountAnsweredQuestions(declarations.at(0)),
        [](int total, const std::string& current) { return total + CountAnsweredQuestions(current); });
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<std::string> declarations = ParseCustomsDeclarations(std::string(argv[1]));
    int sum_of_answers = GetSumOfAnsweredQuestions(declarations);
    std::cout << "Sum of questions answered by all groups: " << sum_of_answers << std::endl;
}
