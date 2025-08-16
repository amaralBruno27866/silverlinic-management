#pragma once
#include <sqlite3.h>
#include <string>
#include "core/Utils.h"
#include "utils/StructuredLogger.h"

namespace utils {
inline void logDbPrepareError(const std::string& scope, sqlite3* db, const std::string& sql){
    LogEventContext ctx{ "DB", "prepare", scope, std::nullopt, std::nullopt };
    if(db){
        logStructured(LogLevel::ERROR, ctx, std::string(sqlite3_errmsg(db)) + " | SQL=" + sql);
    } else {
        logStructured(LogLevel::ERROR, ctx, "null db | SQL=" + sql);
    }
}
inline void logDbStepError(const std::string& scope, sqlite3* db){
    LogEventContext ctx{ "DB", "step", scope, std::nullopt, std::nullopt };
    if(db){
        logStructured(LogLevel::ERROR, ctx, sqlite3_errmsg(db));
    } else {
        logStructured(LogLevel::ERROR, ctx, "null db");
    }
}
}
