
#include <cxxabi.h>
#include "Symbol.h"

std::string Symbol::Demangle(std::string_view mangled) {
    int status = -1;
    const char *demangled = abi::__cxa_demangle(mangled.data(), nullptr, nullptr, &status);
    std::string ret;
    if (demangled == nullptr) {
        ret = std::string(mangled);
    } else {
        ret = demangled;
        free((void *)demangled);
    }
    return ret;
}

std::string Symbol::DumpStr() {
    std::stringstream ret;
    ret << "[id: " << id << ", type: " << type << ", name: "
        << name << ", asmname: " << asmname << ", file: " << file_id
        << ", line" << line << ", isDef: " << isDef << ", isBuiltin: " << isBuiltin << "]";
    return ret.str();
}
