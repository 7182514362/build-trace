
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>
#include "log.h"
#include "db_util.h"
#include "trace.h"

struct cmd_entry {
    const char *name;
    enum bt_cmd_type type;
};

static struct cmd_entry g_cmd_map[] = {
    {"gcc-7", GCC_CMD},
    {"g++-7", GCC_CMD},
    {"gcc-9", GCC_CMD},
    {"g++-9", GCC_CMD},
    {"gcc-10", GCC_CMD},
    {"g++-10", GCC_CMD},
    {"xgcc", GCC_CMD},
    {"xg++", GCC_CMD},
    {"gcc", GCC_CMD},
    {"g++", GCC_CMD},
    {"cc", GCC_CMD},
    {"c++", GCC_CMD},
    {"CC", GCC_CMD},
    {"ld.bfd", LD_CMD},
    {"ld.gold", LD_CMD},
    {"ld.lld", LD_CMD},
    {"lld", LD_CMD},
    {"ld", LD_CMD},
    {"ar", AR_CMD},
    {"", UNKOWN_CMD}};

static enum bt_cmd_type parse_cmd_type(char *name) {
    int len = sizeof(g_cmd_map) / sizeof(g_cmd_map[0]);
    enum bt_cmd_type type = UNKOWN_CMD;
    int i;
    for (i = 0; i < len; ++i) {
        if (bt_end_with(name, g_cmd_map[i].name)) {
            type = g_cmd_map[i].type;
            break;
        }
    }
    return type;
}

/*
 * 0-29 30-47 48-63
 */

int bt_gen_command_id() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

int bt_is_gcc(const char *tool) {
    if (strcasecmp(tool, "gcc") == 0 || strcasecmp(tool, "g++") == 0) {
        return 1;
    }
    if (strstr(tool, "gcc") != NULL || strstr(tool, "g++") != NULL) {
        return 1;
    }
    return 0;
}

struct bt_command *bt_init_trace_cmd(int argc, char **argv, char **envp) {
    struct bt_command *cmd = (struct bt_command *)bt_malloc_and_init(sizeof(struct bt_command));
    if (cmd == NULL) {
        return NULL;
    }
    cmd->argc = argc;
    cmd->argv = argv;
    cmd->nargc = argc;
    cmd->nargv = argv;
    cmd->type = parse_cmd_type(argv[0]);
    cmd->cid = 0;
    cmd->envp = envp;
    return cmd;
}

struct bt_env *bt_init_env(int argc, char **argv, char **envp) {
    struct bt_env *env;
    env = (struct bt_env *)bt_malloc_and_init(sizeof(struct bt_env));
    if (env == NULL) {
        return NULL;
    }

    const char *out_path = getenv("BT_OUT_PATH");
    if (out_path == NULL || *out_path == 0) {
        free(env);
        return NULL;
    }
    if (bt_check_dir_exist(out_path)) {
        env->out_path = out_path;
        char buf[4096];
        int err = snprintf(buf, 4096, "%s/buildtrace.log", out_path);
        if (err > 0) {
            env->log_fstream = fopen(buf, "a+");
            env->log_path = bt_copy_string(buf);
        }

        err = snprintf(buf, 4096, "%s/buildtrace.sqlite", out_path);
        if (err > 0) {
            // env->db = bt_open_db(buf);
            env->db_path = bt_copy_string(buf);
        }
    }

    env->cmd = bt_init_trace_cmd(argc, argv, envp);
    if (env->cmd == NULL) {
        bt_clear_env(env);
        return NULL;
    }
    env->cmd->pid = getpid();
    env->cmd->ppid = getppid();
    char *cid = getenv("BT_CMD_ID");
    if (cid == NULL || *cid == 0) {
        LOG_WARN("BT_CMD_ID invalid");
        env->cmd->pcid = 0;
    } else {
        env->cmd->pcid = atoi(cid);
    }
    // env->envp = envp;
    return env;
}

void bt_clear_env(struct bt_env *env) {
    if (env == NULL) {
        return;
    }
    struct bt_command *cmd = env->cmd;
    if (cmd->nargv != cmd->argv) {
        free(cmd->nargv);
    }
    if (env->log_fstream != NULL) {
        fclose(env->log_fstream);
    }
    // if (env->db != NULL) {
    //     sqlite3_close(env->db);
    // }
    if (env->db_path != NULL) {
        free(env->db_path);
    }
    if (env->log_path != NULL) {
        free(env->log_path);
    }
    free(env);
}