#ifndef SILVERCLINIC_DATABASE_SESSION_H
#define SILVERCLINIC_DATABASE_SESSION_H

#include <sqlite3.h>
#include <string>
#include <stdexcept>
#include <functional>
#include "utils/StructuredLogger.h"

namespace SilverClinic {

class DatabaseSession {
public:
    explicit DatabaseSession(const std::string &path) {
        if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(m_db ? m_db : nullptr)));
        }
        utils::logStructured(utils::LogLevel::INFO, {"DB","open","Database",path,{}}, "DatabaseSession opened");
    }
    explicit DatabaseSession(sqlite3* existing) : m_db(existing), m_owned(false) {}
    ~DatabaseSession() { if (m_owned && m_db) { sqlite3_close(m_db); } }
    sqlite3* handle() const { return m_db; }

    // Transaction helper (returns true if block succeeded and committed)
    bool transaction(const std::function<bool(sqlite3*)> &fn) {
        if (sqlite3_exec(m_db, "BEGIN IMMEDIATE;", nullptr, nullptr, nullptr) != SQLITE_OK) return false;
        if (fn(m_db)) {
            if (sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr) == SQLITE_OK) return true;
        }
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
private:
    sqlite3* m_db {nullptr};
    bool m_owned {true};
};

} // namespace SilverClinic

#endif
