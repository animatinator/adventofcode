#include <exception>
#include <iostream>
#include <sstream>
#include <vector>


constexpr int kAdd = 1;
constexpr int kMultiply = 2;
constexpr int kTerminate = 99;


class Interpreter {
    public:
        Interpreter(std::vector<int> input): memory_(input) {}

        int Interpret() {
            while (memory_[progc_] != kTerminate) {
                InterpretInstruction();
            }

            return memory_[0];
        }

    private:
        int progc_ = 0;
        std::vector<int> memory_;

        int ConsumeInt() {
            int result = memory_[progc_];
            progc_++;
            return result;
        }

        void DoAdd() {
            int l = ConsumeInt();
            int r = ConsumeInt();
            int dest = ConsumeInt();
            memory_[dest] = memory_[l] + memory_[r];
        }

        void DoMultiply() {
            int l = ConsumeInt();
            int r = ConsumeInt();
            int dest = ConsumeInt();
            memory_[dest] = memory_[l] * memory_[r];
        }

        void InterpretInstruction() {
            int instruction = ConsumeInt();
            switch (instruction) {
                case kAdd: 
                    DoAdd();
                    break;
                case kMultiply:
                    DoMultiply();
                    break;
                default: {
                    std::stringstream stream;
                    stream << "Illegal operator at position " << progc_-1 << ": " << memory_[progc_];
                    throw std::runtime_error(stream.str());
                }
            }
        }
};


int main() {
    std::vector<int> input = {1,1,1,4,99,5,6,0,99};
    Interpreter interpreter(input);
    std::cout << "Result: " << interpreter.Interpret();
}