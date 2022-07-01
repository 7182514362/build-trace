#ifndef TRACE_H
#define TRACE_H

#include "util.h"
#include <sqlite3.h>
#include <stdio.h>

enum bt_cmd_type { UNKOWN_CMD,
                   GCC_CMD,
                   LD_CMD,
                   AR_CMD };

struct bt_command {
    // command id
    int cid;
    // parent command id
    int pcid;
    int pid;
    int ppid;
    int argc;
    int nargc; /* new argc */
    char **argv;
    char **nargv; /* new argv */
    char **envp;
    enum bt_cmd_type type;
};

int bt_gen_command_id();

int bt_is_gcc(const char *tool);

struct bt_env {
    struct bt_command *cmd;
    // int (*orig_main)(int, char **, char **);
    // char **envp;
    const char *out_path;
    char *log_path;
    char *db_path;
    FILE *log_fstream;
    // sqlite3 *db;
};

struct bt_command *bt_init_trace_cmd(int argc, char **argv, char **envp);
struct bt_env *bt_init_env(int argc, char **argv, char **envp);
void bt_clear_env(struct bt_env *env);

#endif