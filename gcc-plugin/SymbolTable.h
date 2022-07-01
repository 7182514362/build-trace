#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <functional>
#include "Log.h"
#include "Symbol.h"

class SymbolTable {
public:
    ~SymbolTable();
    void Insert(Symbol *sym) {
        if (sym == nullptr || m_table.find(sym->id) != m_table.end()) {
            return;
        }
        if (m_table.find(sym->id) != m_table.end()) {
            LOG_WARN << "sym id exist: " << sym->DumpStr();
        }
        m_table.insert({sym->id, sym});
    }

    Symbol *GetSymbol(uint32_t id) {
        auto itor = m_table.find(id);
        if (itor != m_table.end()) {
            return itor->second;
        }
        return nullptr;
    }

    void ForEachSymbol(std::function<void(Symbol *)> handler) {
        for (auto &pair : m_table) {
            handler(pair.second);
        }
    }

private:
    std::unordered_map<uint32_t, Symbol *> m_table;
};

#endif