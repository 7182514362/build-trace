#ifndef UTIL_H
#define UTIL_H

char *bt_malloc_and_init(int size);

int bt_check_file_exist(const char *file);
int bt_check_dir_exist(const char *dir);

char *bt_copy_string(char *src);

int bt_end_with(const char *target, const char *suffix);

#endif