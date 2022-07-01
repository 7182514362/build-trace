#ifndef DB_UTIL_H
#define DB_UTIL_H

#include <string_view>
#include <vector>
#include <sqlite3.h>
#include "Symbol.h"
#include "SymbolTable.h"
#include "SymbolRefTable.h"

class DBUtil {
public:
    DBUtil() = default;
    ~DBUtil() {
        Close();
    }
    bool Connect(std::string_view file);
    void Close();
    bool Execute(std::string_view sql);
    bool Query(std::string_view sql);

    bool CreateFileTbl();
    bool InsertFile(std::vector<std::string_view> &files);

    bool CreateSymbolTbl();
    bool InsertSymbol(SymbolTable &data);

    bool CreateSymbolRefTbl();
    bool InsertSymbolRef(SymbolRefTable &data);

private:
    sqlite3 *m_db{nullptr};
};

#endif