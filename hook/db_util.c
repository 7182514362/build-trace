// #include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "util.h"
#include "db_util.h"
#include "log.h"

static int handle_busy(void *a, int b) {
    usleep(5000);
    return 1;
}

sqlite3 *bt_open_db(const char *file) {
    sqlite3 *db;
    int err = sqlite3_open(file, &db);
    if (err != SQLITE_OK) {
        return NULL;
    }
    sqlite3_busy_handler(db, handle_busy, NULL);
    sqlite3_exec(db, "PRAGMA cache_size=8000;PRAGMA synchronous=OFF;PRAGMA temp_store=MEMORY;", NULL, NULL, NULL);
    return db;
}

void bt_close_db(sqlite3 *db) {
    if (db != NULL) {
        sqlite3_close(db);
    }
}

int bt_create_table(sqlite3 *db) {
    if (db == 0) {
        return -1;
    }
    char *errmsg;
    const char *sql = "CREATE TABLE IF NOT EXISTS command (\
        cid INTEGER PRIMARY KEY AUTOINCREMENT,\
        time TEXT NOT NULL,\
        command TEXT,\
        pcid INT NOT NULL,\
        pid INT NOT NULL,\
        ppid INT NOT NULL);";
    int err = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if (err != SQLITE_OK) {
        LOG_ERROR("sqlite3_exec: %s", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    return 0;
}
int bt_create_workspace_tbl(sqlite3 *db) {
    if (db == 0) {
        return -1;
    }
    char *errmsg;
    const char *sql = "CREATE TABLE IF NOT EXISTS workspace (\
        id INTEGER PRIMARY KEY AUTOINCREMENT,\
        path TEXT NOT NULL\
    );";
    int err = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if (err != SQLITE_OK) {
        LOG_ERROR("sqlite3_exec: %s", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    return 0;
}

static void get_format_time(char *time_str, int max_size) {
    time_t curtime;
    time(&curtime);
    struct tm *time_tm = localtime(&curtime);
    strftime(time_str, max_size, "%Y-%m-%d %H:%M:%S", time_tm);
}

static int get_cmd_str(struct bt_command *cmd, char **buf) {
    int len = 0;
    for (int i = 0; i < cmd->argc; ++i) {
        len += strlen(cmd->argv[i]) + 1;
    }
    *buf = (char *)bt_malloc_and_init(len);
    int written = 0;
    for (int i = 0; i < cmd->argc; ++i) {
        int nBytes = snprintf(*buf + written, len - written, "%s ", cmd->argv[i]);
        if (nBytes < 0) {
            break;
        }
        written += nBytes;
    }

    return written;
}

int bt_insert_command(sqlite3 *db, struct bt_command *cmd) {
    if (db == NULL) {
        return -1;
    }
    char time_str[64];
    get_format_time(time_str, sizeof(time_str));
    char *cmd_str;
    int len = get_cmd_str(cmd, &cmd_str);

    sqlite3_stmt *stmt = NULL;
    const char *sql = "INSERT INTO command(time,command,pcid,pid,ppid) VALUES(?,?,?,?,?);";
    sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_bind_text(stmt, 1, time_str, -1, NULL);
    sqlite3_bind_text(stmt, 2, cmd_str, -1, NULL);
    sqlite3_bind_int(stmt, 3, cmd->pcid);
    sqlite3_bind_int(stmt, 4, cmd->pid);
    sqlite3_bind_int(stmt, 5, cmd->ppid);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);
    free(cmd_str);
    return 0;
}

int bt_get_command_id(sqlite3 *db) {
    if (db == NULL) {
        return -1;
    }
    char *errmsg = NULL;
    char **dbResult;
    int nRow = 0, nColumn = 0; // nRow  查找出的总行数,nColumn 存储列
    int ret = sqlite3_get_table(db, "select seq from sqlite_sequence where name='command';", &dbResult, &nRow, &nColumn, &errmsg);
    if (NULL != errmsg) {
        sqlite3_free_table(dbResult);
        errmsg = NULL;
        return -1;
    }
    int cmd_id = atoi(dbResult[nColumn]);
    sqlite3_free_table(dbResult);
    return cmd_id;
}