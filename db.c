#include "config.h"
#include "db.h"
#include <stdio.h>
#include <string.h>

static int DatabaseExec(const char *sql) {
    char *err = NULL;
    int rc = sqlite3_exec(gDb, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        char msg[512];
        snprintf(msg, sizeof(msg), "Database error: %s", err ? err : sqlite3_errmsg(gDb));
        SetStatus(msg);
        if (err) sqlite3_free(err);
        return 0;
    }
    return 1;
}

int DatabaseInit(void) {
    if (sqlite3_open(DB_NAME, &gDb) != SQLITE_OK) {
        if (gDb) { sqlite3_close(gDb); gDb = NULL; }
        SetStatus("SQLite database unavailable. Scores will not be saved.");
        return 0;
    }

    const char *schema =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS sessions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "player TEXT NOT NULL,"
        "wpm INTEGER NOT NULL,"
        "accuracy REAL NOT NULL,"
        "errors INTEGER NOT NULL,"
        "duration INTEGER NOT NULL,"
        "difficulty TEXT NOT NULL,"
        "created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))"
        ");";

    if (!DatabaseExec(schema)) {
        sqlite3_close(gDb);
        gDb = NULL;
        return 0;
    }
    return 1;
}

void SaveSessionToDb(const char *player, int wpm, double accuracy, int errors, int duration, const char *difficulty) {
    if (!gDbReady || !gDb) {
        SetStatus("Test completed (database not connected; result not saved).");
        return;
    }

    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "INSERT INTO sessions(player,wpm,accuracy,errors,duration,difficulty) "
        "VALUES(?,?,?,?,?,?);";

    if (sqlite3_prepare_v2(gDb, sql, -1, &stmt, NULL) != SQLITE_OK) {
        SetStatus("Could not prepare the database record.");
        return;
    }

    sqlite3_bind_text(stmt, 1, player, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, wpm);
    sqlite3_bind_double(stmt, 3, accuracy);
    sqlite3_bind_int(stmt, 4, errors);
    sqlite3_bind_int(stmt, 5, duration);
    sqlite3_bind_text(stmt, 6, difficulty, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        SetStatus("Could not save this session to the database.");
    } else {
        SetStatus("Test completed. Your result has been saved.");
    }
    sqlite3_finalize(stmt);
}

void ShowLeaderboard(void) {
    if (!gDbReady || !gDb) {
        MessageBoxA(gMain, "Database is not available.", "Typing Speed Leaderboard", MB_OK | MB_ICONWARNING);
        return;
    }

    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT player, MAX(wpm) AS best, ROUND(AVG(accuracy),1) "
        "FROM sessions GROUP BY player ORDER BY best DESC LIMIT 10;";

    char out[4096];
    CopySafe(out, sizeof(out), "TOP 10 PLAYERS\n\n");
    int used = (int)strlen(out);
    int rank = 1;

    if (sqlite3_prepare_v2(gDb, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW && used < (int)sizeof(out) - 160) {
            const unsigned char *player = sqlite3_column_text(stmt, 0);
            int best = sqlite3_column_int(stmt, 1);
            double avg = sqlite3_column_double(stmt, 2);
            used += snprintf(out + used, sizeof(out) - used,
                             "%2d. %-20s  %3d WPM  | Avg accuracy %.1f%%\n",
                             rank++, player ? (const char *)player : "Player", best, avg);
        }
    }
    sqlite3_finalize(stmt);

    if (rank == 1) strncat(out, "No saved results yet. Complete a test first.", sizeof(out) - strlen(out) - 1);
    MessageBoxA(gMain, out, "Typing Speed Leaderboard", MB_OK | MB_ICONINFORMATION);
}

void ShowHistory(void) {
    if (!gDbReady || !gDb) {
        MessageBoxA(gMain, "Database is not available.", "Recent Typing Sessions", MB_OK | MB_ICONWARNING);
        return;
    }

    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT player,wpm,accuracy,errors,duration,difficulty,created_at "
        "FROM sessions ORDER BY id DESC LIMIT 12;";

    char out[4096];
    CopySafe(out, sizeof(out), "RECENT TEST HISTORY\n\n");
    int used = (int)strlen(out);
    int rows = 0;

    if (sqlite3_prepare_v2(gDb, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW && used < (int)sizeof(out) - 220) {
            const unsigned char *p = sqlite3_column_text(stmt, 0);
            const unsigned char *d = sqlite3_column_text(stmt, 5);
            const unsigned char *date = sqlite3_column_text(stmt, 6);
            used += snprintf(out + used, sizeof(out) - used,
                             "%-12s %3d WPM  %5.1f%%  %2d errors  %3ds  %-12s  %s\n",
                             p ? (const char *)p : "Player",
                             sqlite3_column_int(stmt, 1),
                             sqlite3_column_double(stmt, 2),
                             sqlite3_column_int(stmt, 3),
                             sqlite3_column_int(stmt, 4),
                             d ? (const char *)d : "",
                             date ? (const char *)date : "");
            rows++;
        }
    }
    sqlite3_finalize(stmt);

    if (!rows) strncat(out, "No history available yet.", sizeof(out) - strlen(out) - 1);
    MessageBoxA(gMain, out, "Recent Typing Sessions", MB_OK | MB_ICONINFORMATION);
}