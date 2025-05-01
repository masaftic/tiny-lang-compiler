#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();

    Lexer lexer(source);
    Parser parser(lexer);

    try {
        vector<Statement*> program = parser.parse();

        for (auto& stmt : program) {
            stmt->print(0);
        }

        Interpreter interpreter;
        interpreter.interpret(program);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}