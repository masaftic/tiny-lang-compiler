#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include "token.h"

class SymbolRegistry {
private:
    std::unordered_map<std::string, float> table;

public:
    void set(const std::string& name, float value) {
        table[name] = value;
    }

    float get(const std::string& name) const {
        auto it = table.find(name);
        if (it == table.end()) {
            throw std::runtime_error("Undefined variable: " + name);
        }
        return it->second;
    }
};


#endif // SYMBOLTABLE_H