#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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