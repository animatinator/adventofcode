#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>


enum Command {
    NOOP = 0,
    ACC = 1,
    JMP = 2,
};


struct Instruction {
    Command instruction;
    int argument;
};


struct InfiniteLoopException : public std::exception {};


class Interpreter {
  public:
    void LoadProgram(std::vector<Instruction> instructions) {
        instructions_ = instructions;
        pc_ = 0;
        acc_ = 0;
        seen_instructions_ = {};
    }

    int Execute() {
        while (pc_ < instructions_.size()) {
            if (seen_instructions_.find(pc_) != seen_instructions_.end()) {
                throw InfiniteLoopException();
            }
            seen_instructions_.insert(pc_);

            Instruction instruction = instructions_.at(pc_);
            switch(instruction.instruction) {
                case Command::NOOP:
                    ++pc_;
                    break;
                case Command::ACC:
                    ++pc_;
                    acc_ += instruction.argument;
                    break;
                case Command::JMP:
                    pc_ += instruction.argument;
                    break;
                default:
                    std::stringstream error_msg;
                    error_msg << "Unrecognised instruction: " << instruction.instruction;
                    throw std::runtime_error(error_msg.str());
            }
        }

        return acc_;
    }

  private:
    int pc_ = 0;
    int acc_ = 0;
    std::vector<Instruction> instructions_;
    std::unordered_set<int> seen_instructions_;
};


Command SwapNoopAndJump(Command command) {
    if (command == Command::NOOP) {
        return Command::JMP;
    } else if (command == Command::JMP) {
        return Command::NOOP;
    } else {
        throw std::runtime_error("Incorrect SwapNoopAndJump call");
    }
}

Command ToCommand(std::string command_name) {
    if (command_name == "nop") {
        return Command::NOOP;
    } else if (command_name == "acc") {
        return Command::ACC;
    } else if (command_name == "jmp") {
        return Command::JMP;
    } else {
        std::stringstream error_msg;
        error_msg << "Unrecognised command: " << command_name;
        throw std::runtime_error(error_msg.str());
    }
}

std::vector<Instruction> ParseInstructions(std::string file_path) {
    std::vector<Instruction> results;
    std::ifstream infile(file_path);

    std::string command_name;
    int argument;

    while (infile >> command_name >> argument) {
        Command command = ToCommand(command_name);
        Instruction instruction {command, argument};
        results.push_back(instruction);
    }

    return results;
}

std::vector<std::vector<Instruction>> ParsePossiblePrograms(std::string file_path) {
    std::vector<Instruction> canonical_program = ParseInstructions(file_path);
    std::vector<std::vector<Instruction>> results;
    results.push_back(canonical_program);

    for (int i = 0; i < canonical_program.size(); i++) {
        const auto& [comm, arg] = canonical_program.at(i);

        if (comm == Command::NOOP || comm == Command::JMP) {
            Command replacement = SwapNoopAndJump(comm);
            std::vector<Instruction> modification(canonical_program.begin(), canonical_program.end());
            modification.at(i).instruction = replacement;
            results.push_back(modification);
        }
    }

    return results;
}

int FindReturnValueOfSuccessfulProgram(std::vector<std::vector<Instruction>> programs) {
    for (const std::vector<Instruction> program : programs) {
        Interpreter interpreter;
        interpreter.LoadProgram(program);
        try {
            return interpreter.Execute();
        } catch (InfiniteLoopException e) {
            continue;
        }
    }
    throw std::runtime_error("None of the possible programs succeeded!");
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<std::vector<Instruction>> programs = ParsePossiblePrograms(argv[1]);
    int result = FindReturnValueOfSuccessfulProgram(programs);
    std::cout << "Accumulator after successful run: " << result << std::endl;
}