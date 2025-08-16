#ifndef SCL90R_MANAGER_H
#define SCL90R_MANAGER_H

#include <vector>
#include <optional>
#include <sqlite3.h>
#include "forms/SCL90R.h"
#include "utils/CSVUtils.h"

namespace SilverClinic {
    class SCL90RManager {
        sqlite3* m_db;
    public:
        explicit SCL90RManager(sqlite3* db) : m_db(db) {}
        bool create(const Forms::SCL90R &form);
        bool update(const Forms::SCL90R &form);
        std::optional<Forms::SCL90R> getById(int id) const;
        std::vector<Forms::SCL90R> listByCase(int caseProfileId) const;
        bool deleteById(int id);
        int importFromCSV(const std::string &filePath);
    private:
        Forms::SCL90R mapRow(sqlite3_stmt* stmt) const;
        void computeAndPersistDerived(int id);
    };
}

#endif
