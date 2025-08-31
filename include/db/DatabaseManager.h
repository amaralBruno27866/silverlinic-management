#ifndef DB_DATABASE_MANAGER_H
#define DB_DATABASE_MANAGER_H

#include <string>
#include <sqlite3.h>

namespace db {

bool validateDatabaseIntegrity(sqlite3* db);

bool executeSQLCommand(sqlite3* db, const std::string& sql, const std::string& description);

bool createDatabaseTables(sqlite3* db);

bool insertSampleData(sqlite3* db);

} // namespace db

#endif // DB_DATABASE_MANAGER_H
