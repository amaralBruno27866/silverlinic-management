#pragma once
#include <sqlite3.h>
#include <string>
#include "core/Utils.h"

namespace utils {
inline void logDbPrepareError(const std::string& scope, sqlite3* db, const std::string& sql){
    if(db){
        logMessage("ERROR", scope + " prepare failed: " + std::string(sqlite3_errmsg(db)) + " | SQL= " + sql);
    } else {
        logMessage("ERROR", scope + " prepare failed: null db | SQL= " + sql);
    }
}
inline void logDbStepError(const std::string& scope, sqlite3* db){
    if(db){
        logMessage("ERROR", scope + " step failed: " + std::string(sqlite3_errmsg(db)));
    } else {
        logMessage("ERROR", scope + " step failed: null db");
    }
}
}
