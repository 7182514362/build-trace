#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string_view>

enum LogType : uint8_t {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};
class Log {
public:
    Log(LogType type, std::string_view file, int line) {
        m_stream << "[" << m_logTypeStr[type] << " " << file << ":" << line << "] ";
    }
    ~Log() {
        m_stream << std::endl;
    }
    std::ofstream &Stream() {
        return m_stream;
    }

    static void Config(const std::string &file) {
        m_stream.open(file);
    }

private:
    static std::ofstream m_stream;
    static std::string_view m_logTypeStr[];
};

#define LOG(Type) Log(Type, __FILE__, __LINE__).Stream()

#define LOG_TRACE LOG(LogType::TRACE)
#define LOG_DEBUG LOG(LogType::DEBUG)
#define LOG_INFO LOG(LogType::INFO)
#define LOG_WARN LOG(LogType::WARN)
#define LOG_ERROR LOG(LogType::ERROR)

#endif