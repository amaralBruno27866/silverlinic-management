#include <sqlite3.h>
#include <iostream>
#include "core/DatabaseConfig.h"

#define TEST_ASSERT(cond, msg) \
    if(!(cond)){ std::cout << "❌ FAIL: " << msg << std::endl; return false; } else { std::cout << "✅ PASS: " << msg << std::endl; }
#define RUN_TEST(fn) \
    std::cout << "\n🧪 Running " #fn "..." << std::endl; \
    if(fn()){ std::cout << "✅ " #fn " completed" << std::endl; passed++; } else { std::cout << "❌ " #fn " failed" << std::endl; failed++; } total++;

static int total=0, passed=0, failed=0;
static sqlite3* db = nullptr;

bool test_apply_pragmas() {
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) return false;
    TEST_ASSERT(SilverClinic::DatabaseConfig::applyStandardPragmas(db), "applyStandardPragmas should return true");
    sqlite3_stmt* stmt=nullptr;
    TEST_ASSERT(sqlite3_prepare_v2(db, "PRAGMA foreign_keys;", -1, &stmt, nullptr)==SQLITE_OK, "Prepare foreign_keys query");
    int v=-1; if(sqlite3_step(stmt)==SQLITE_ROW) v = sqlite3_column_int(stmt,0); sqlite3_finalize(stmt);
    TEST_ASSERT(v==1, "foreign_keys pragma should be ON");
    return true;
}

bool cleanup() { if(db){ sqlite3_close(db); db=nullptr;} return true; }

int main(){
    RUN_TEST(test_apply_pragmas);
    RUN_TEST(cleanup);
    std::cout << "\n📊 Summary: " << passed << "/" << total << " passed, failed=" << failed << std::endl;
    return failed==0?0:1;
}
