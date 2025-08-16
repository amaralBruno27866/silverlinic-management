#ifndef SILVERCLINIC_ID_ALLOCATOR_H
#define SILVERCLINIC_ID_ALLOCATOR_H

#include <sqlite3.h>
#include <string>
#include <optional>
#include <algorithm>
#include "utils/StructuredLogger.h"

namespace SilverClinic {

class IdAllocator {
public:
    // Ensures IDs are monotonically increasing; minStart enforces a base (e.g. 400001 for case_profile)
    static std::optional<int> next(sqlite3* db, const std::string &table, int minStart = 1, const std::string &column = "id") {
        std::string sql = "SELECT MAX(" + column + ") FROM " + table;
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            utils::logStructured(utils::LogLevel::ERROR, {"DB","id_max","Table",table,{}}, "prepare failed for MAX query");
            return std::nullopt;
        }
        int nextId = minStart;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int maxId = sqlite3_column_type(stmt,0)==SQLITE_NULL ? 0 : sqlite3_column_int(stmt,0);
            nextId = std::max(maxId + 1, minStart);
        }
        sqlite3_finalize(stmt);
        return nextId;
    }
};

}

#endif
