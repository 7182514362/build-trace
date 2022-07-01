
#include <stdarg.h>
#include "log.h"

void bt_log_config(FILE *stream) {
    g_log_fp = stream;
}

void bt_log(const char *level, const char *file, int line, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buf[4096];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    fprintf(g_log_fp, "[%s %s:%d] %s\n", level, file, line, buf);
    va_end(ap);
}