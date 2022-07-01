/*
 * Hook main() using LD_PRELOAD, because why not?
 * Obviously, this code is not portable. Use at your own risk.
 *
 * Compile using 'gcc hax.c -o hax.so -fPIC -shared -ldl'
 * Then run your program as 'LD_PRELOAD=$PWD/hax.so ./a.out'
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// #include <sqlite3.h>

#include "log.h"
#include "db_util.h"
#include "trace.h"
#include "util.h"

static int (*g_orig_main)(int, char **, char **);

static struct bt_env *g_trace_env;
static struct bt_command *g_trace_cmd;

#define ARG_MAX_LEN 4096
static char g_gcc_plugin_arg[ARG_MAX_LEN];

void add_gcc_plugin_arg() {
    const char *plugin_path = getenv("BT_GCC_PLUGIN");
    if (plugin_path == NULL || *plugin_path == 0) {
        return;
    }
    char **new_argv = (char **)bt_malloc_and_init(sizeof(char *) * (g_trace_cmd->argc + 2));
    if (new_argv == NULL) {
        return;
    }
    int ret = snprintf(g_gcc_plugin_arg, ARG_MAX_LEN, "-fplugin=%s -fplugin-arg-libssa-cmdid=%d", plugin_path, g_trace_cmd->cid);
    if (ret < 0) {
        free(new_argv);
        return;
    }
    int i;
    for (i = 0; i < g_trace_cmd->argc; ++i) {
        new_argv[i] = g_trace_cmd->argv[i];
    }
    new_argv[i] = g_gcc_plugin_arg;
    g_trace_cmd->nargc = g_trace_cmd->argc + 1;
    g_trace_cmd->nargv = new_argv;
}

static int before_main() {
    LOG_INFO("--- Before main ---");
    sqlite3 *db = bt_open_db(g_trace_env->db_path);
    if (db == NULL) {
        LOG_ERROR("Failed to open db: %s", g_trace_env->db_path);
        return -1;
    }
    if (bt_create_table(db) < 0) {
        LOG_ERROR("create table failed");
        return -1;
    }
    if (bt_insert_command(db, g_trace_cmd) < 0) {
        LOG_ERROR("insert command failed");
        return -1;
    }
    int cmd_id = bt_get_command_id(db);
    if (cmd_id < 0) {
        LOG_ERROR("get command id failed");
        return -1;
    }
    char buf[32] = {0};
    sprintf(buf, "%d", cmd_id);
    setenv("BT_CMD_ID", buf, 1);

    bt_close_db(db);
    g_trace_cmd->cid = cmd_id;
    if (g_trace_cmd->type == GCC_CMD) {
        // add_gcc_plugin_arg();
    }
    return 0;
}

static int after_main() {
    if (g_trace_env == NULL) {
        return 0;
    }
    LOG_INFO("--- %d After main ----", g_trace_cmd->pid);

    // todo

    bt_clear_env(g_trace_env);
    g_trace_env = NULL;

    return 0;
}

int main_hook(int argc, char **argv, char **envp) {
    int saved_errno = errno;
    g_trace_env = bt_init_env(argc, argv, envp);
    if (g_trace_env == NULL) {
        fprintf(stderr, "init env failed");
        errno = saved_errno;
        return g_orig_main(argc, argv, envp);
    }
    g_trace_cmd = g_trace_env->cmd;
    bt_log_config(g_trace_env->log_fstream);

    before_main();

    LOG_INFO("--- %d Start main ---", g_trace_cmd->pid);
    errno = saved_errno;
    int ret = g_orig_main(g_trace_cmd->nargc, g_trace_cmd->nargv, g_trace_cmd->envp);

    after_main();
    return ret;
}

int __libc_start_main(int (*main)(int, char **, char **), int argc, char **argv, int (*init)(int, char **, char **),
                      void (*fini)(void), void (*rtld_fini)(void), void *stack_end) {
    g_orig_main = main;

    typeof(&__libc_start_main) real_start_main = dlsym(RTLD_NEXT, "__libc_start_main");
    return real_start_main(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}
// some program may call exit(int), so after_main will not run
// such as as(assembler), ld(linker)
void exit(int status) {
    typeof(&_exit) real_exit = dlsym(RTLD_NEXT, "exit");
    after_main();
    return real_exit(status);
}
// __asm__(".symver exit, exit@GLIBC_2.2.5");