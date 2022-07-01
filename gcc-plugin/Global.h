#ifndef GLOBAL_H
#define GLOBAL_H
#include <string>

class Global {
public:
    static Global &GetInstance() {
        static Global instance;
        return instance;
    }

    bool Init(int cmdId);

    const std::string &GetOutPath() {
        return m_outPath;
    }
    const std::string &GetDBPath() {
        return m_dbPath;
    }
    const std::string &GetLogPath() {
        return m_logPath;
    }

    std::string_view GetCWD() {
        return m_cwd;
    }

private:
    Global() = default;

private:
    std::string m_outPath;
    std::string m_dbPath;
    std::string m_logPath;
    std::string m_cwd;
};

#endif