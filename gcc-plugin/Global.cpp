
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include "Global.h"

bool Global::Init(int cmdId) {
    char *outPath = std::getenv("BT_OUT_PATH");
    if (outPath == nullptr || *outPath == 0) {
        return false;
    }
    m_outPath = outPath;
    m_dbPath = m_outPath + "/compilations";
    if (!std::filesystem::is_directory(m_dbPath)) {
        std::filesystem::create_directory(m_dbPath);
    }
    // m_dbPath = tmp + "/" + std::to_string(cmdId) + "_" + std::to_string(getpid()) + ".sqlite";
    m_logPath = m_outPath + "/ssa.log";
    m_cwd = std::filesystem::current_path().string();
    return true;
}
