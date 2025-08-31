#pragma once

#include <sqlite3.h>
#include <string>

namespace db {
    bool validateDatabaseIntegrity(sqlite3* db);
    bool executeSQLCommand(sqlite3* db, const std::string& sql, const std::string& description);
    bool createDatabaseTables(sqlite3* db);
    bool insertSampleData(sqlite3* db);
}
