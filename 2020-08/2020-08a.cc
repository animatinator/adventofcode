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


class Interpreter {
  public:
    void LoadProgram(std::vector<Instruction> instructions) {
        instructions_ = instructions;
        pc_ = 0;
        acc_ = 0;
        seen_instructions_ = {};
    }

    void SetInfiniteLoopCallback(std::function<void(int, int)> callback) {
        infinite_loop_callback_ = callback;
    }

    void Execute() {
        for (;;) {
            if (seen_instructions_.find(pc_) != seen_instructions_.end()) {
                infinite_loop_callback_(pc_, acc_);
                return;
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
    }

  private:
    int pc_ = 0;
    int acc_ = 0;
    std::vector<Instruction> instructions_;
    std::unordered_set<int> seen_instructions_;
    std::function<void(int, int)> infinite_loop_callback_;
};


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


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<Instruction> instructions = ParseInstructions(argv[1]);
    Interpreter interpreter;
    interpreter.LoadProgram(instructions);
    interpreter.SetInfiniteLoopCallback([](int line, int acc) {
        std::cout << "Infinite loop found at line " << line << " with accumulator value " << acc << std::endl;
    });
    interpreter.Execute();
    std::cout << "Execution complete." << std::endl;
}