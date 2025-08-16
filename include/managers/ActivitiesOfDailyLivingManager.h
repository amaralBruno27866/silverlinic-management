#ifndef ACTIVITIES_OF_DAILY_LIVING_MANAGER_H
#define ACTIVITIES_OF_DAILY_LIVING_MANAGER_H

#include <vector>
#include <optional>
#include <sqlite3.h>
#include "forms/ActivitiesOfDailyLiving.h"
#include "utils/CSVUtils.h"

namespace SilverClinic {
    class ActivitiesOfDailyLivingManager {
        sqlite3* m_db;
    public:
        explicit ActivitiesOfDailyLivingManager(sqlite3* db) : m_db(db) {}
        bool create(const Forms::ActivitiesOfDailyLiving &form);
        bool update(const Forms::ActivitiesOfDailyLiving &form);
        std::optional<Forms::ActivitiesOfDailyLiving> getById(int id) const;
        std::vector<Forms::ActivitiesOfDailyLiving> listByCase(int caseProfileId) const;
        bool deleteById(int id);
        int importFromCSV(const std::string &filePath);
    private:
        Forms::ActivitiesOfDailyLiving mapRow(sqlite3_stmt* stmt) const;
    };
}

#endif
