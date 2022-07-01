
#include "Log.h"
#include "DBUtil.h"

bool DBUtil::Connect(std::string_view file) {
    Close();
    int err = sqlite3_open(file.data(), &m_db);
    if (err != SQLITE_OK) {
        LOG_ERROR << "Failed to connect: " << file;
        m_db = nullptr;
        return false;
    }
    return true;
}

void DBUtil::Close() {
    if (m_db != nullptr) {
        // sqlite3_close_v2(m_db);
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool DBUtil::Execute(std::string_view sql) {
    if (m_db == nullptr) {
        return false;
    }
    char *errMsg;
    int err = sqlite3_exec(m_db, sql.data(), nullptr, nullptr, &errMsg);
    if (err != SQLITE_OK) {
        LOG_ERROR << "Failed to execute: " << errMsg;
        m_db = nullptr;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DBUtil::Query(std::string_view sql) {
    return true;
}

bool DBUtil::CreateSymbolTbl() {
    std::string_view sql = R"#(CREATE TABLE IF NOT EXISTS symbol (
        id INT PRIMARY KEY NOT NULL,
        name TEXT NOT NULL,
        link_name TEXT NOT NULL,
        file_id INT NOT NULL,
        line INT NOT NULL,
        type INT NOT NULL,
        is_def INT NOT NULL,
        is_builtin INT NOT NULL
        );)#";
    return Execute(sql);
}

bool DBUtil::InsertSymbol(SymbolTable &data) {
    std::string_view sql = "INSERT INTO symbol(id,name,link_name,file_id,line,type,is_def,is_builtin) VALUES (?,?,?,?,?,?,?,?);";
    sqlite3_stmt *stmt;
    char *errMsg = 0;
    const char *tail = 0;
    int err = sqlite3_prepare_v2(m_db, sql.data(), sql.length(), &stmt, &tail);
    if (err != SQLITE_OK) {
        LOG_ERROR << "prepared failed: " << err;
        return false;
    }
    auto handler = [stmt](Symbol *sym) {
        sqlite3_bind_int(stmt, 1, sym->id);
        sqlite3_bind_text(stmt, 2, sym->name.data(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, sym->asmname.data(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, sym->file_id);
        sqlite3_bind_int(stmt, 5, sym->line);
        sqlite3_bind_int(stmt, 6, sym->type);
        sqlite3_bind_int(stmt, 7, sym->isDef);
        sqlite3_bind_int(stmt, 8, sym->isBuiltin);
        sqlite3_step(stmt);
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    };
    sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, &errMsg);
    data.ForEachSymbol(handler);
    sqlite3_exec(m_db, "END TRANSACTION", NULL, NULL, &errMsg);
    sqlite3_finalize(stmt);
    return true;
}

bool DBUtil::CreateSymbolRefTbl() {
    std::string_view sql = R"#(CREATE TABLE IF NOT EXISTS symbol_ref (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        src INT NOT NULL,
        dsts TEXT NOT NULL
        );)#";
    return Execute(sql);
}

bool DBUtil::InsertSymbolRef(SymbolRefTable &data) {
    std::string_view sql = "INSERT INTO symbol_ref(src,dsts) VALUES (?,?);";
    sqlite3_stmt *stmt;
    char *errMsg = 0;
    const char *tail = 0;
    int err = sqlite3_prepare_v2(m_db, sql.data(), sql.length(), &stmt, &tail);
    if (err != SQLITE_OK) {
        LOG_ERROR << "prepared failed: " << err;
        return false;
    }

    auto handler = [stmt](uint32_t caller, std::vector<uint32_t> &callees) {
        std::string calleeStr;
        for (uint32_t callee : callees) {
            calleeStr.append(std::to_string(callee)).append(",");
        }
        if (!calleeStr.empty()) {
            calleeStr.pop_back();
        }
        sqlite3_bind_int(stmt, 1, caller);
        sqlite3_bind_text(stmt, 2, calleeStr.data(), -1, nullptr);
        sqlite3_step(stmt);
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    };
    sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, &errMsg);
    data.ForEachSymRef(handler);
    sqlite3_exec(m_db, "END TRANSACTION", NULL, NULL, &errMsg);
    sqlite3_finalize(stmt);
    return true;
}

bool DBUtil::CreateFileTbl() {
    std::string_view sql = R"#(CREATE TABLE IF NOT EXISTS source_file (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name INT NOT NULL
        );)#";
    return Execute(sql);
}

bool DBUtil::InsertFile(std::vector<std::string_view> &files) {
    std::string_view sql = "INSERT INTO source_file(id,name) VALUES (?,?);";
    sqlite3_stmt *stmt;
    char *errMsg = 0;
    const char *tail = 0;
    int err = sqlite3_prepare_v2(m_db, sql.data(), sql.length(), &stmt, &tail);
    if (err != SQLITE_OK) {
        LOG_ERROR << "prepared failed: " << err;
        return false;
    }
    sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, &errMsg);
    for (int i = 0; i < files.size(); ++i) {
        sqlite3_bind_int(stmt, 1, i);
        sqlite3_bind_text(stmt, 2, files[i].data(), -1, nullptr);
        sqlite3_step(stmt);
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_exec(m_db, "END TRANSACTION", NULL, NULL, &errMsg);
    sqlite3_finalize(stmt);
    return true;
}