#ifndef SYMBOL_REF_TABLE
#define SYMBOL_REF_TABLE

#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <functional>

class SymbolRefTable {
public:
    void Insert(uint32_t caller, uint32_t callee) {
        m_symbolRef[caller].push_back(callee);
    }

    void ForEachSymRef(std::function<void(uint32_t from, std::vector<uint32_t> &to)> handler) {
        for (auto &pair : m_symbolRef) {
            handler(pair.first, pair.second);
        }
    }

private:
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_symbolRef;
};

#endif