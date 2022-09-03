#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <string_view>
#include <sstream>

struct Symbol {
    uint32_t id{0};
    uint64_t type{0};
    std::string name;
    std::string_view asmname;
    std::string_view file;
    int file_id;
    uint32_t line{0};
    uint32_t column{0};
    bool isDef{false};
    bool isBuiltin{false};
    /* demangle c++ symbols */
    static std::string Demangle(std::string_view mangled);

    std::string DumpStr();
};

#endif