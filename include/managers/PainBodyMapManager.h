#ifndef PAIN_BODY_MAP_MANAGER_H
#define PAIN_BODY_MAP_MANAGER_H

#include <vector>
#include <optional>
#include <sqlite3.h>
#include "forms/PainBodyMap.h"
#include "utils/CSVUtils.h"

namespace SilverClinic {
    class PainBodyMapManager {
        sqlite3* m_db;
    public:
        explicit PainBodyMapManager(sqlite3* db) : m_db(db) {}
        bool create(const Forms::PainBodyMap &form);
        bool update(const Forms::PainBodyMap &form);
        std::optional<Forms::PainBodyMap> getById(int id) const;
        std::vector<Forms::PainBodyMap> listByCase(int caseProfileId) const;
        bool deleteById(int id);
        int importFromCSV(const std::string &filePath);
    private:
        Forms::PainBodyMap mapRow(sqlite3_stmt* stmt) const;
    };
}

#endif
