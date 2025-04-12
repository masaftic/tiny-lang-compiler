#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <cctype> // Include for isdigit, isalpha, isalnum, isspace
#include "lexer.h"

using namespace std;


int main(int argc, char **argv)
{
    if (argc != 2)
    {
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
    Token token = lexer.nextToken();
    while (token.type != Token::Type::ENDOFFILE)
    {
        cout << token.toString() << '\n';
        token = lexer.nextToken();
    }

    cout << token.toString() << '\n';
    cout << "Lexer finished." << endl;

    return 0;
}