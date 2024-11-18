#include <sqlite3.h>
#include <stdio.h>

int init_db(const char *db_name) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(db_name, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS commands (id INTEGER PRIMARY KEY, timestamp TEXT, command TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    sqlite3_close(db);
    return rc;
}

void record_command(const char *command) {
    printf("Recording command: %s\n", command);  // Debugging output
    sqlite3 *db;
    char *err_msg = 0;
    char sql[256];

    sprintf(sql, "INSERT INTO commands (timestamp, command) VALUES (datetime('now'), '%s');", command);

    int rc = sqlite3_open("cmd.db", &db);
if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    return;
}

rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
} else {
    printf("Command recorded successfully.\n");
}

sqlite3_close(db);
}
