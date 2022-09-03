#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

char *bt_malloc_and_init(int size);

int bt_check_file_exist(const char *file);
int bt_check_dir_exist(const char *dir);

char *bt_copy_string(char *src);

int bt_end_with(const char *target, const char *suffix);

void bt_lock_file(FILE *fp);
void bt_unlock_file(FILE *fp);

int bt_need_trace(char *exec);

#endif