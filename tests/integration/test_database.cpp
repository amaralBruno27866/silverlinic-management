#include "core/Address.h"
#include "core/Assessor.h"
#include "core/Client.h"
#include "core/CaseProfile.h"
#include "core/Utils.h"
#include <iostream>
#include <sqlite3.h>
#include <cassert>

using namespace std;
using namespace SilverClinic;
using namespace utils;

// Simple test framework macros
#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        cout << "❌ FAIL: " << message << endl; \
        return false; \
    } else { \
        cout << "✅ PASS: " << message << endl; \
    }

#define RUN_TEST(test_function) \
    cout << "\n🧪 Running " << #test_function << "..." << endl; \
    if (test_function()) { \
        cout << "✅ " << #test_function << " completed successfully" << endl; \
        passed_tests++; \
    } else { \
        cout << "❌ " << #test_function << " failed" << endl; \
        failed_tests++; \
    } \
    total_tests++;

// Global test counters
int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

// Global database connection
sqlite3* test_db = nullptr;

bool executeSQLCommand(const string& sql, const string& description) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(test_db, sql.c_str(), nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        cout << "Error executing " << description << ": " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    
    return true;
}

bool setup_test_database() {
    // Create a test database in memory
    int result = sqlite3_open(":memory:", &test_db);
    TEST_ASSERT(result == SQLITE_OK, "Should open test database");
    
    // Enable foreign keys
    TEST_ASSERT(executeSQLCommand("PRAGMA foreign_keys = ON;", "Foreign keys"), "Should enable foreign keys");
    
    // Create tables
    string createAssessorTable = R"(
        CREATE TABLE assessor(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    TEST_ASSERT(executeSQLCommand(createAssessorTable, "Assessor table"), "Should create assessor table");
    
    string createClientTable = R"(
        CREATE TABLE client(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            date_of_birth TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    TEST_ASSERT(executeSQLCommand(createClientTable, "Client table"), "Should create client table");
    
    string createCaseProfileTable = R"(
        CREATE TABLE case_profile(
            id INTEGER PRIMARY KEY,
            client_id INTEGER NOT NULL,
            assessor_id INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT 'Pending',
            notes TEXT CHECK(length(notes) <= 1500),
            created_at TEXT NOT NULL,
            closed_at TEXT,
            modified_at TEXT NOT NULL
        )
    )";
    TEST_ASSERT(executeSQLCommand(createCaseProfileTable, "CaseProfile table"), "Should create case_profile table");
    
    string createAddressTable = R"(
        CREATE TABLE address(
            id INTEGER PRIMARY KEY,
            user_key INTEGER NOT NULL,
            street TEXT,
            city TEXT,
            province TEXT,
            postal_code TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    TEST_ASSERT(executeSQLCommand(createAddressTable, "Address table"), "Should create address table");
    
    return true;
}

bool test_database_insertion() {
    string currentTime = getCurrentTimestamp();
    
    // Test inserting an assessor
    string insertAssessor = R"(
        INSERT INTO assessor (id, firstname, lastname, phone, email, created_at, modified_at)
        VALUES (100001, 'Dr. Test', 'Assessor', '416-555-0123', 'test@assessor.com', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    TEST_ASSERT(executeSQLCommand(insertAssessor, "Insert assessor"), "Should insert assessor");
    
    // Test inserting a client
    string insertClient = R"(
        INSERT INTO client (id, firstname, lastname, phone, email, date_of_birth, created_at, modified_at)
        VALUES (300001, 'Test', 'Client', '416-555-9876', 'test@client.com', '1990-01-01', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    TEST_ASSERT(executeSQLCommand(insertClient, "Insert client"), "Should insert client");
    
    // Test inserting addresses
    string insertAddress1 = R"(
        INSERT INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES (200001, 100001, '123 Assessor St', 'Toronto', 'ON', 'M5H 2N2', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    TEST_ASSERT(executeSQLCommand(insertAddress1, "Insert assessor address"), "Should insert assessor address");
    
    string insertAddress2 = R"(
        INSERT INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES (200002, 300001, '456 Client Ave', 'Toronto', 'ON', 'M5V 3A8', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    TEST_ASSERT(executeSQLCommand(insertAddress2, "Insert client address"), "Should insert client address");
    
    // Test inserting a case profile
    string insertCaseProfile = R"(
        INSERT INTO case_profile (id, client_id, assessor_id, status, notes, created_at, modified_at)
        VALUES (400001, 300001, 100001, 'Pending', 'Test case profile', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    TEST_ASSERT(executeSQLCommand(insertCaseProfile, "Insert case profile"), "Should insert case profile");
    
    return true;
}

bool test_database_queries() {
    sqlite3_stmt* stmt;
    
    // Test counting assessors
    if (sqlite3_prepare_v2(test_db, "SELECT COUNT(*) FROM assessor", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            TEST_ASSERT(count == 1, "Should have 1 assessor");
        }
        sqlite3_finalize(stmt);
    }
    
    // Test counting clients
    if (sqlite3_prepare_v2(test_db, "SELECT COUNT(*) FROM client", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            TEST_ASSERT(count == 1, "Should have 1 client");
        }
        sqlite3_finalize(stmt);
    }
    
    // Test counting addresses
    if (sqlite3_prepare_v2(test_db, "SELECT COUNT(*) FROM address", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            TEST_ASSERT(count == 2, "Should have 2 addresses");
        }
        sqlite3_finalize(stmt);
    }
    
    // Test counting case profiles
    if (sqlite3_prepare_v2(test_db, "SELECT COUNT(*) FROM case_profile", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            TEST_ASSERT(count == 1, "Should have 1 case profile");
        }
        sqlite3_finalize(stmt);
    }
    
    return true;
}

bool test_notes_validation() {
    // Create a long note (over 1500 characters)
    string longNote(1600, 'A');
    
    string insertLongNote = R"(
        INSERT INTO case_profile (id, client_id, assessor_id, status, notes, created_at, modified_at)
        VALUES (400002, 300001, 100001, 'Pending', ')" + longNote + "', '" + getCurrentTimestamp() + "', '" + getCurrentTimestamp() + R"(')
    )";
    
    // This should fail due to CHECK constraint
    TEST_ASSERT(!executeSQLCommand(insertLongNote, "Insert long note"), "Should reject notes over 1500 characters");
    
    return true;
}

bool cleanup_test_database() {
    if (test_db) {
        sqlite3_close(test_db);
        test_db = nullptr;
    }
    return true;
}

int main() {
    cout << "🚀 Database Integration Tests" << endl;
    cout << "==============================" << endl;
    
    // Run all tests
    RUN_TEST(setup_test_database);
    RUN_TEST(test_database_insertion);
    RUN_TEST(test_database_queries);
    RUN_TEST(test_notes_validation);
    RUN_TEST(cleanup_test_database);
    
    // Print summary
    cout << "\n📊 Test Summary" << endl;
    cout << "===============" << endl;
    cout << "Total tests: " << total_tests << endl;
    cout << "Passed: " << passed_tests << " ✅" << endl;
    cout << "Failed: " << failed_tests << " ❌" << endl;
    cout << "Success rate: " << (total_tests > 0 ? (passed_tests * 100 / total_tests) : 0) << "%" << endl;
    
    if (failed_tests == 0) {
        cout << "\n🎉 All tests passed!" << endl;
        return 0;
    } else {
        cout << "\n⚠️  Some tests failed!" << endl;
        return 1;
    }
}
