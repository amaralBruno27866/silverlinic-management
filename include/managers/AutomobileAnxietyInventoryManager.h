#ifndef AUTOMOBILEANXIETYINVENTORYMANAGER_H
#define AUTOMOBILEANXIETYINVENTORYMANAGER_H

#include <vector>
#include <optional>
#include <string>
#include <sqlite3.h>
#include "forms/AutomobileAnxietyInventory.h"
#include "utils/CSVUtils.h"

namespace SilverClinic {
class AutomobileAnxietyInventoryManager {
    sqlite3* m_db;
public:
    explicit AutomobileAnxietyInventoryManager(sqlite3* db): m_db(db) {}

    bool create(const Forms::AutomobileAnxietyInventory &form);
    bool update(const Forms::AutomobileAnxietyInventory &form);
    std::optional<Forms::AutomobileAnxietyInventory> getById(int id) const;
    std::vector<Forms::AutomobileAnxietyInventory> listByCase(int caseProfileId) const;
    bool deleteById(int id);
    int importFromCSV(const std::string &filePath);

private:
    Forms::AutomobileAnxietyInventory mapRow(sqlite3_stmt* stmt) const;
};
}
#endif
