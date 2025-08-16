#ifndef BECK_ANXIETY_INVENTORY_MANAGER_H
#define BECK_ANXIETY_INVENTORY_MANAGER_H

#include <vector>
#include <optional>
#include <sqlite3.h>
#include "forms/BeckAnxietyInventory.h"
#include "utils/CSVUtils.h"

namespace SilverClinic {
    class BeckAnxietyInventoryManager {
        sqlite3* m_db;
    public:
        explicit BeckAnxietyInventoryManager(sqlite3* db) : m_db(db) {}

        bool create(const Forms::BeckAnxietyInventory &form);
        bool update(const Forms::BeckAnxietyInventory &form);
        std::optional<Forms::BeckAnxietyInventory> getById(int id) const;
        std::vector<Forms::BeckAnxietyInventory> listByCase(int caseProfileId) const;
        bool deleteById(int id);
        int importFromCSV(const std::string &filePath);
    private:
        int computeTotal(const Forms::BeckAnxietyInventory &f) const;
        std::string computeSeverity(int total) const;
        Forms::BeckAnxietyInventory mapRow(sqlite3_stmt* stmt) const;
    };
}

#endif
