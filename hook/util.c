
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h>

#include "util.h"

char *bt_malloc_and_init(int size) {
    if (size <= 0) {
        return NULL;
    }
    char *ret = (char *)malloc(size);
    if (ret == NULL) {
        return NULL;
    }
    memset(ret, 0, size);
    return ret;
}

int bt_check_file_exist(const char *file) {
    struct stat buf;
    stat(file, &buf);
    return S_ISREG(buf.st_mode);
}

int bt_check_dir_exist(const char *dir) {
    struct stat buf;
    stat(dir, &buf);
    return S_ISDIR(buf.st_mode);
}

char *bt_copy_string(char *src) {
    if (src == NULL) {
        return NULL;
    }
    int len = strlen(src);
    char *ret = (char *)bt_malloc_and_init((len + 1) * sizeof(char));
    if (ret == NULL) {
        return NULL;
    }
    strncpy(ret, src, len);
    return ret;
}

int bt_end_with(const char *target, const char *suffix) {
    int len1 = strlen(target);
    int len2 = strlen(suffix);
    if (len1 < len2 || len1 <= 0 || len2 <= 0) {
        return 0;
    }
    int i = len1 - 1, j = len2 - 1;
    while (i >= 0 && j >= 0) {
        if (target[i] != suffix[j]) {
            return 0;
        }
        --i;
        --j;
    }
    return 1;
}

void bt_lock_file(FILE *fp) {
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    fcntl(fileno(fp), F_SETLKW, &lock);
}

void bt_unlock_file(FILE *fp) {
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    fcntl(fileno(fp), F_SETLK, &lock);
}

static const char *g_ignore_cmd[] = {"objdump", "head", "tail", "xargs", "rm", "tar", "ls", "pwd",
                                     "basename", "expr", "realpath", "file", "mkdir", "uname", "grep",
                                     "sed", "cat", "sort", "find", "md5sum", "sha256sum", "chmod", "chown",
                                     "gzip", "man", "perl", NULL};

static const char *g_trace_cmd[] = {"cc", "c++", "gcc", "g++", "gcc-7", "g++-7", "gcc-9", "g++-9", "xgcc", "xg++", "cc1", "cc1plus", "as", "collect2", "ld", "gold", "ar", "ld.gold",
                                    "ld.bfd", "ld.bsd", "lld", "objcopy", "cp", "mv", "strip", NULL};

int bt_need_trace(char *exec) {
    if (exec == NULL) {
        return 0;
    }
    int i = 0;
    for (; g_trace_cmd[i] != NULL; ++i) {
        if (bt_end_with(exec, g_trace_cmd[i])) {
            return 1;
        }
    }
    return 0;
}