#include "Log.h"

std::ofstream Log::m_stream;
std::string_view Log::m_logTypeStr[5] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
