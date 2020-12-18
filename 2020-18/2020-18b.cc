#include <exception>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>
#include <string>
#include <vector>


enum Operator {
    PLUS = 0,
    MULT = 1,
};

enum TokenType {
    L_BRACE = 0,
    R_BRACE = 1,
    INT = 2,
    OPERATOR_PLUS = 3,
    OPERATOR_MULT = 4,
};

std::string TokenTypeDebugString(TokenType type) {
    switch (type) {
        case TokenType::L_BRACE:
            return "L_BRACE";
        case TokenType::R_BRACE:
            return "R_BRACE";
        case TokenType::INT:
            return "INT";
        case TokenType::OPERATOR_PLUS:
            return "OPERATOR_PLUS";
        case TokenType::OPERATOR_MULT:
            return "OPERATOR_MULT";
        default:
            return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    long long data = -123;

    std::string DebugString() {
        std::stringstream stream;
        stream << "TOKEN(" << TokenTypeDebugString(type);
        if (data != -123) {
            stream << ", " << data;
        }
        stream << ")";
        return stream.str();
    }
};


void Assert(bool passed, std::string message = "") {
    if (!passed) {
        std::stringstream msg;
        msg << "Assertion failed: " << message << ".";
        throw std::runtime_error(msg.str());
    }
}

std::runtime_error CreateInvalidCharacterException(char c) {
    std::stringstream msg;
    msg << "Invalid character in input: '" << c << "'!";
    return std::runtime_error(msg.str());
}

std::queue<Token> LexProgramLine(const std::string& program) {
    std::queue<Token> result;

    for (const char c : program) {
        if (c == ' ') {
            continue;
        } else if ('0' <= c && c <= '9') {
            result.push({TokenType::INT, c - '0'});
        } else {
            switch (c) {
                case '(':
                    result.push({TokenType::L_BRACE});
                    break;
                case ')':
                    result.push({TokenType::R_BRACE});
                    break;
                case '+':
                    result.push({TokenType::OPERATOR_PLUS});
                    break;
                case '*':
                    result.push({TokenType::OPERATOR_MULT});
                    break;
                default:
                    throw CreateInvalidCharacterException(c);
            }
        }
    }

    return result;
}

std::vector<std::queue<Token>> Lex(std::string file_name) {
    std::vector<std::queue<Token>> programs;
    std::ifstream infile(file_name);
    std::string program_text;

    while (std::getline(infile, program_text)) {
        programs.push_back(LexProgramLine(program_text));
    }

    return programs;
}

long long EvaluateOperator(Operator op, long long left, long long right) {
    switch (op) {
        case Operator::PLUS:
            return left + right;
        case Operator::MULT:
            return left * right;
        default:
            throw std::runtime_error("Unknown operator!");
    }
}

long long EvaluateBracketedExpression(std::queue<Token>& program);

long long EvaluateExpression(std::queue<Token>& program) {
    long long accumulator;
    Token t = program.front();
    if (t.type == TokenType::L_BRACE) {
        accumulator = EvaluateBracketedExpression(program);
    } else {
        Assert(t.type == TokenType::INT);
        accumulator = t.data;
        program.pop();
    }
    while (!program.empty()) {
        Token t = program.front();
        Operator op;
        switch (t.type) {
            case TokenType::R_BRACE:
                return accumulator;
            case TokenType::OPERATOR_PLUS:
                op = Operator::PLUS;
                break;
            case TokenType::OPERATOR_MULT:
                op = Operator::MULT;
                break;
            default:
                throw std::runtime_error("Unexpected token!");
        }

        program.pop();
        long long argument;
        t = program.front();

        if (op == Operator::PLUS) {
            // Plus is high-precedence so we evaluate either a single int or a bracketed expression immediately.
            if (t.type == TokenType::L_BRACE) {
                argument = EvaluateBracketedExpression(program);
            } else {
                Assert(t.type == TokenType::INT);
                argument = t.data;
                program.pop();
            }
        } else {
            // Multiplication has low precedence so we parse the whole remaining expression before
            // applying the operator.
            argument = EvaluateExpression(program);
        }

        accumulator = EvaluateOperator(op, accumulator, argument);
    }
    return accumulator;
}

long long EvaluateBracketedExpression(std::queue<Token>& program) {
    Assert(program.front().type == TokenType::L_BRACE);
    program.pop();
    long long result = EvaluateExpression(program);
    Assert(program.front().type == TokenType::R_BRACE);
    program.pop();
    return result;
}

long long EvaluateProgram(std::queue<Token>& program) {
    return EvaluateExpression(program);
}

std::vector<long long> EvaluateMultiplePrograms(std::vector<std::queue<Token>> programs) {
    std::vector<long long> result;

    for (auto& program : programs) {
        result.push_back(EvaluateProgram(program));
    }

    return result;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<std::queue<Token>> programs = Lex(std::string(argv[1]));
    std::vector<long long> results = EvaluateMultiplePrograms(programs);

    std::cout << "Results by line:" << std::endl;
    for (const long long result : results) {
        std::cout << " - " << result << std::endl;
    }

    long long sum_of_results = std::accumulate(results.begin(), results.end(), 0LL);
    std::cout << "Sum of results: " << sum_of_results << std::endl;
}
