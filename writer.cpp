#include "sqlite3.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    sqlite3 *db;

    // Open (or create) the database
    if (sqlite3_open("data.db", &db) != SQLITE_OK) {
        std::cerr << "Failed to open data.db: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }

    // --- Removed WAL mode ---
    // sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);

    // Create table if it doesn't exist
    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS log("
        "seconds REAL, "
        "data REAL, "
        "time TEXT, "
        "Msg1ToFahduddinKhaja TEXT, "
        "Msg2ToFahduddinKhaja TEXT);",
        nullptr, nullptr, nullptr);

    // Get the starting time in seconds
    sqlite3_stmt *stmt0;
    sqlite3_prepare_v2(db, "SELECT strftime('%s','now');", -1, &stmt0, nullptr);
    sqlite3_step(stmt0);
    int start_seconds = sqlite3_column_int(stmt0, 0);
    sqlite3_finalize(stmt0);

    // Prepare insert statement
    sqlite3_stmt *stmt;
    const char *insertSQL =
        "INSERT INTO log(seconds, data, time, Msg1ToFahduddinKhaja, Msg2ToFahduddinKhaja) "
        "VALUES (strftime('%s','now') - ?, random()%100, strftime('%I:%M:%S %p','now','localtime'), 'FSAE is lame!!', 'jk');";

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing insert: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return 1;
    }

    // Continuous insert loop
    while (true) {
        sqlite3_bind_int(stmt, 1, start_seconds);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Insert failed: " << sqlite3_errmsg(db) << "\n";
        }

        sqlite3_reset(stmt); // reset for next insert
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
