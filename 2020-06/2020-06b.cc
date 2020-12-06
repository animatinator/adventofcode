#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

typedef std::vector<std::string> GroupDeclaration;

std::vector<GroupDeclaration> ParseCustomsDeclarations(const std::string& file_path) {
    std::ifstream infile(file_path);
    std::string line;
    GroupDeclaration current_declaration;
    std::vector<GroupDeclaration> results;

    while (std::getline(infile, line)) {
        if (line.empty()) {
            results.push_back(current_declaration);
            current_declaration = {};
        } else {
            current_declaration.push_back(line);
        }
    }

    results.push_back(current_declaration);

    return results;
}

std::unordered_set<char> GetAsSet(const std::string& answers) {
    return std::unordered_set<char>(answers.begin(), answers.end());
}

template<typename T>
std::unordered_set<T> SetUnion(const std::unordered_set<T>& first, const std::unordered_set<T>& second) {
    std::unordered_set<T> result;

    for (const T& element : first) {
        auto search = second.find(element);
        if (search != second.end()) {
            result.insert(element);
        }
    }

    return result;
}

int CountUnanimouslyAnsweredQuestions(const GroupDeclaration& declaration) {
    std::unordered_set<char> unanimous_answers = std::accumulate(
        std::next(declaration.begin()), declaration.end(),
        GetAsSet(declaration.at(0)),
        [](std::unordered_set<char> total, const std::string& current) { return SetUnion(total, GetAsSet(current)); } );
    return unanimous_answers.size();
}

int GetSumOfUnanimouslyAnsweredQuestions(const std::vector<GroupDeclaration>& declarations) {
    return std::accumulate(
        std::next(declarations.begin()), declarations.end(),
        CountUnanimouslyAnsweredQuestions(declarations.at(0)),
        [](int total, const GroupDeclaration& current) { return total + CountUnanimouslyAnsweredQuestions(current); });
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<GroupDeclaration> declarations = ParseCustomsDeclarations(std::string(argv[1]));
    int sum_of_answers = GetSumOfUnanimouslyAnsweredQuestions(declarations);
    std::cout << "Sum of questions answered by all groups: " << sum_of_answers << std::endl;
}
