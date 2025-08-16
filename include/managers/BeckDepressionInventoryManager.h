#ifndef BECK_DEPRESSION_INVENTORY_MANAGER_H
#define BECK_DEPRESSION_INVENTORY_MANAGER_H

#include <vector>
#include <optional>
#include <sqlite3.h>
#include "forms/BeckDepressionInventory.h"
#include "utils/CSVUtils.h"

namespace SilverClinic {

    class BeckDepressionInventoryManager {
        sqlite3* m_db;
    public:
        explicit BeckDepressionInventoryManager(sqlite3* db) : m_db(db) {}

        bool create(const Forms::BeckDepressionInventory &form);
        bool update(const Forms::BeckDepressionInventory &form);
        std::optional<Forms::BeckDepressionInventory> getById(int id) const;
        std::vector<Forms::BeckDepressionInventory> listByCase(int caseProfileId) const;
        bool deleteById(int id);
        int importFromCSV(const std::string &filePath);
    private:
        int computeTotal(const Forms::BeckDepressionInventory &f) const;
        std::string computeSeverity(int total) const;
        Forms::BeckDepressionInventory mapRow(sqlite3_stmt* stmt) const;
    };
}

#endif
