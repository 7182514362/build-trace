#include "SymbolTable.h"

SymbolTable::~SymbolTable() {
    for (auto &pair : m_table) {
        if (pair.second != nullptr) {
            delete pair.second;
        }
    }
}
