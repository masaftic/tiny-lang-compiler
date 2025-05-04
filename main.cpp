#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

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

    source = "write \"input an integer: \" \n read x";

    Lexer lexer(source);
    Parser parser(lexer);

    std::istringstream inputStream("2");
    std::ostringstream outputStream;
    Interpreter interpreter(std::cin, outputStream);

    try {
        vector<Statement*> program = parser.parse();
        if (parser.getErrors().size() > 0) {
            std::cerr << "Parser errors:\n";
            for (const auto& error : parser.getErrors()) {
                std::cerr << error << std::endl;
            }
            return 1;
        }

        string output;
        for (auto& stmt : program) {
            output += stmt->toString(0);
        }
        cout << "Parsed Program:\n" << output << endl;

        interpreter.interpret(program);

        // Output the result of interpretation
        std::cout << "Interpreter Output:\n" << outputStream.str();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}