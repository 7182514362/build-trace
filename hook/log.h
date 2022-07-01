#ifndef LOG_H
#define LOG_H

#include <stdio.h>

enum log_level {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

// static char *g_log_file;
static FILE *g_log_fp = NULL;

void bt_log_config(FILE *stream);

void bt_log(const char *level, const char *file, int line, char *fmt, ...);

#define LOG_TRACE(...) bt_log("TRACE", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) bt_log("DEBUG", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) bt_log("INFO ", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) bt_log("WARN ", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) bt_log("ERROR", __FILE__, __LINE__, __VA_ARGS__)

#endif