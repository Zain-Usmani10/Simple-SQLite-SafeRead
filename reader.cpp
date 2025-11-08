#include "sqlite3.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

int main() {
    sqlite3 *src, *dst;

    // Count existing snapshots
    int count = 0;
    for (const auto &entry : fs::directory_iterator(".")) {
        std::string name = entry.path().filename().string();
        if (name.rfind("Snapshot", 0) == 0 && name.size() >= 3 && name.substr(name.size()-3) == ".db") {
            count++;
        }
    }

    // Create new snapshot name
    std::string snapshotName = "Snapshot" + std::to_string(count + 1) + ".db";

    // Open source database (data.db)
    if (sqlite3_open_v2("data.db", &src, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        std::cerr << "Error: could not open data.db\n";
        return 1;
    }

    // Open destination database (new snapshot)
    if (sqlite3_open(snapshotName.c_str(), &dst) != SQLITE_OK) {
        std::cerr << "Error: could not create snapshot\n";
        sqlite3_close(src);
        return 1;
    }

    // Copy data using SQLite backup API
    sqlite3_backup *b = sqlite3_backup_init(dst, "main", src, "main");
    if (b) {
        sqlite3_backup_step(b, -1);
        sqlite3_backup_finish(b);
        std::cout << "Snapshot created successfully: " << snapshotName << "\n";
    } else {
        std::cerr << "Backup failed: " << sqlite3_errmsg(dst) << "\n";
        sqlite3_close(src);
        sqlite3_close(dst);
        return 1;
    }

    // --- Export to CSV ---
    std::string csvName = snapshotName.substr(0, snapshotName.size() - 3) + ".csv";
    std::ofstream csvFile(csvName);
    if (!csvFile.is_open()) {
        std::cerr << "Could not create CSV file\n";
    } else {
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(dst, "SELECT * FROM log;", -1, &stmt, nullptr) == SQLITE_OK) {
            // Write CSV header
            csvFile << "seconds,data,time,Msg1ToFahduddinKhaja,Msg2ToFahduddinKhaja\n";

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
                    csvFile << sqlite3_column_text(stmt, i);
                    if (i < sqlite3_column_count(stmt) - 1)
                        csvFile << ",";
                }
                csvFile << "\n";
            }
            sqlite3_finalize(stmt);
            std::cout << "CSV created: " << csvName << "\n";
        } else {
            std::cerr << "Failed to read snapshot for CSV\n";
        }
    }

    sqlite3_close(src);
    sqlite3_close(dst);
}
