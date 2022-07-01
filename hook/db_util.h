#ifndef DB_UTIL_H
#define DB_UTIL_H

#include <sqlite3.h>
#include "trace.h"

// sqlite3 *g_db;

sqlite3 *bt_open_db(const char *file);
void bt_close_db(sqlite3* db);


int bt_create_table(sqlite3 *db);

int bt_insert_command(sqlite3 *db, struct bt_command *cmd);

int bt_get_command_id(sqlite3* db);

int bt_create_workspace_tbl(sqlite3* db);

#endif