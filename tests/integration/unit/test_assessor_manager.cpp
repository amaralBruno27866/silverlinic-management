#include <iostream>
#include <cassert>
#include <vector>
#include <sqlite3.h>
#include "managers/AssessorManager.h"
#include "core/Assessor.h"
#include "core/Address.h"
#include "core/DateTime.h"
#include "core/Utils.h"
#include "core/DatabaseConfig.h"

using namespace std;
using namespace SilverClinic;

// Global test database
sqlite3* testDb = nullptr;

void setupTestDatabase() {
    cout << "🧪 Setting up test database..." << endl;
    
    // Ensure test directories exist
    DatabaseConfig::ensureDirectoriesExist();
    
    // Remove existing test database
    remove(DatabaseConfig::ASSESSOR_MANAGER_TEST_DB.c_str());
    
    // Open test database
    if (sqlite3_open(DatabaseConfig::ASSESSOR_MANAGER_TEST_DB.c_str(), &testDb) != SQLITE_OK) {
        cerr << "Failed to open test database: " << sqlite3_errmsg(testDb) << endl;
        exit(1);
    }
    
    // Enable foreign keys
    sqlite3_exec(testDb, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    
    // Create assessor table
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
    
    if (sqlite3_exec(testDb, createAssessorTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Failed to create assessor table: " << sqlite3_errmsg(testDb) << endl;
        exit(1);
    }
    
    // Create address table
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
    
    if (sqlite3_exec(testDb, createAddressTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Failed to create address table: " << sqlite3_errmsg(testDb) << endl;
        exit(1);
    }
    
    // Create case_profile table
    string createCaseProfileTable = R"(
        CREATE TABLE case_profile(
            id INTEGER PRIMARY KEY,
            client_id INTEGER NOT NULL,
            assessor_id INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT 'Pending',
            notes TEXT,
            created_at TEXT NOT NULL,
            closed_at TEXT,
            modified_at TEXT NOT NULL
        )
    )";
    
    if (sqlite3_exec(testDb, createCaseProfileTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Failed to create case_profile table: " << sqlite3_errmsg(testDb) << endl;
        exit(1);
    }
    
    cout << "✅ Test database setup completed" << endl;
}

void cleanupTestDatabase() {
    if (testDb) {
        sqlite3_close(testDb);
        remove(DatabaseConfig::ASSESSOR_MANAGER_TEST_DB.c_str());
    }
}

void test_create_assessor() {
    cout << "\n🧪 Running test_create_assessor..." << endl;
    
    AssessorManager manager(testDb);
    
    // Create test assessor
    Address address;
    DateTime now = DateTime::now();
    Assessor assessor(100001, "João", "Silva", "joao@test.com", "11999999999", address, now, now);
    
    bool result = manager.create(assessor);
    assert(result == true);
    cout << "✅ PASS: Assessor created successfully" << endl;
    
    // Verify assessor exists
    assert(manager.exists(100001) == true);
    cout << "✅ PASS: Assessor exists in database" << endl;
    
    // Try to create duplicate (should fail)
    bool duplicateResult = manager.create(assessor);
    assert(duplicateResult == false);
    cout << "✅ PASS: Duplicate creation properly rejected" << endl;
    
    cout << "✅ test_create_assessor completed successfully" << endl;
}

void test_read_operations() {
    cout << "\n🧪 Running test_read_operations..." << endl;
    
    AssessorManager manager(testDb);
    
    // Create additional test assessors
    Address address;
    DateTime now = DateTime::now();
    
    Assessor assessor2(100002, "Maria", "Santos", "maria@test.com", "11888888888", address, now, now);
    Assessor assessor3(100003, "Pedro", "Oliveira", "pedro@test.com", "11777777777", address, now, now);
    
    manager.create(assessor2);
    manager.create(assessor3);
    
    // Test readAll
    vector<Assessor> allAssessors = manager.readAll();
    assert(allAssessors.size() == 3);
    cout << "✅ PASS: ReadAll returned correct number of assessors" << endl;
    
    // Test readById
    auto assessor = manager.readById(100001);
    assert(assessor.has_value());
    assert(assessor->getFirstName() == "JOAO"); // No accents expected
    assert(assessor->getLastName() == "SILVA");
    cout << "✅ PASS: ReadById returned correct assessor" << endl;
    
    // Test readById with non-existent ID
    auto nonExistent = manager.readById(999999);
    assert(!nonExistent.has_value());
    cout << "✅ PASS: ReadById correctly returned empty for non-existent ID" << endl;
    
    // Test count
    int count = manager.getCount();
    assert(count == 3);
    cout << "✅ PASS: GetCount returned correct count" << endl;
    
    cout << "✅ test_read_operations completed successfully" << endl;
}

void test_search_operations() {
    cout << "\n🧪 Running test_search_operations..." << endl;
    
    AssessorManager manager(testDb);
    
    // Test search by name
    vector<Assessor> results = manager.searchByName("Silva");
    assert(results.size() == 1);
    assert(results[0].getLastName() == "SILVA");
    cout << "✅ PASS: SearchByName found correct assessor" << endl;
    
    // Test search by partial name
    results = manager.searchByName("Santos");
    assert(results.size() == 1);
    assert(results[0].getFirstName() == "MARIA");
    cout << "✅ PASS: SearchByName with partial match worked" << endl;
    
    // Test find by email
    auto assessor = manager.findByEmail("PEDRO@TEST.COM");
    assert(assessor.has_value());
    assert(assessor->getFirstName() == "PEDRO");
    cout << "✅ PASS: FindByEmail found correct assessor" << endl;
    
    // Test find by non-existent email
    auto nonExistent = manager.findByEmail("nonexistent@test.com");
    assert(!nonExistent.has_value());
    cout << "✅ PASS: FindByEmail correctly returned empty for non-existent email" << endl;
    
    cout << "✅ test_search_operations completed successfully" << endl;
}

void test_update_assessor() {
    cout << "\n🧪 Running test_update_assessor..." << endl;
    
    AssessorManager manager(testDb);
    
    // Get existing assessor
    auto assessor = manager.readById(100001);
    assert(assessor.has_value());
    
    // Update assessor data
    Assessor updatedAssessor = assessor.value();
    updatedAssessor.setFirstName("Joao Carlos"); // No accents in test
    updatedAssessor.setEmail("joao.carlos@test.com");
    
    bool result = manager.update(updatedAssessor);
    assert(result == true);
    cout << "✅ PASS: Assessor updated successfully" << endl;
    
    // Verify update
    auto verifyAssessor = manager.readById(100001);
    assert(verifyAssessor.has_value());
    assert(verifyAssessor->getFirstName() == "JOAO CARLOS"); // No accents expected
    assert(verifyAssessor->getEmail() == "JOAO.CARLOS@TEST.COM");
    cout << "✅ PASS: Update changes verified in database" << endl;
    
    // Try to update non-existent assessor
    Assessor nonExistent(999999, "Test", "User", "test@test.com", "11999999999", Address(), DateTime::now(), DateTime::now());
    bool updateResult = manager.update(nonExistent);
    assert(updateResult == false);
    cout << "✅ PASS: Update of non-existent assessor properly rejected" << endl;
    
    cout << "✅ test_update_assessor completed successfully" << endl;
}

void test_pagination() {
    cout << "\n🧪 Running test_pagination..." << endl;
    
    AssessorManager manager(testDb);
    
    // Test pagination
    vector<Assessor> page1 = manager.readWithPagination(2, 0);
    assert(page1.size() == 2);
    cout << "✅ PASS: First page returned correct number of assessors" << endl;
    
    vector<Assessor> page2 = manager.readWithPagination(2, 2);
    assert(page2.size() == 1);
    cout << "✅ PASS: Second page returned correct number of assessors" << endl;
    
    // Verify different assessors in different pages
    assert(page1[0].getAssessorId() != page2[0].getAssessorId());
    cout << "✅ PASS: Different pages returned different assessors" << endl;
    
    cout << "✅ test_pagination completed successfully" << endl;
}

void test_validation() {
    cout << "\n🧪 Running test_validation..." << endl;
    
    AssessorManager manager(testDb);
    
    // Test invalid assessor data
    Address address;
    DateTime now = DateTime::now();
    
    // Empty first name
    Assessor invalidAssessor1(100010, "", "Test", "test@test.com", "11999999999", address, now, now);
    bool result1 = manager.create(invalidAssessor1);
    assert(result1 == false);
    cout << "✅ PASS: Creation with empty first name properly rejected" << endl;
    
    // Invalid email
    Assessor invalidAssessor2(100011, "Test", "User", "invalid-email", "11999999999", address, now, now);
    bool result2 = manager.create(invalidAssessor2);
    assert(result2 == false);
    cout << "✅ PASS: Creation with invalid email properly rejected" << endl;
    
    // Invalid ID range
    Assessor invalidAssessor3(50000, "Test", "User", "test@test.com", "11999999999", address, now, now);
    bool result3 = manager.create(invalidAssessor3);
    assert(result3 == false);
    cout << "✅ PASS: Creation with invalid ID range properly rejected" << endl;
    
    cout << "✅ test_validation completed successfully" << endl;
}

void test_delete_operations() {
    cout << "\n🧪 Running test_delete_operations..." << endl;
    
    AssessorManager manager(testDb);
    
    // Test canDelete (should be true since no cases)
    bool canDelete = manager.canDelete(100003);
    assert(canDelete == true);
    cout << "✅ PASS: CanDelete returned true for assessor without cases" << endl;
    
    // Delete assessor
    bool deleteResult = manager.deleteById(100003);
    assert(deleteResult == true);
    cout << "✅ PASS: Assessor deleted successfully" << endl;
    
    // Verify deletion
    assert(manager.exists(100003) == false);
    cout << "✅ PASS: Assessor no longer exists after deletion" << endl;
    
    // Try to delete non-existent assessor
    bool deleteNonExistent = manager.deleteById(999999);
    assert(deleteNonExistent == false);
    cout << "✅ PASS: Deletion of non-existent assessor properly rejected" << endl;
    
    // Verify count decreased
    int newCount = manager.getCount();
    assert(newCount == 2);
    cout << "✅ PASS: Count decreased after deletion" << endl;
    
    cout << "✅ test_delete_operations completed successfully" << endl;
}

int main() {
    cout << "🚀 AssessorManager Unit Tests" << endl;
    cout << "==============================" << endl;
    
    try {
        setupTestDatabase();
        
        test_create_assessor();
        test_read_operations();
        test_search_operations();
        test_update_assessor();
        test_pagination();
        test_validation();
        test_delete_operations();
        
        cleanupTestDatabase();
        
        cout << "\n📊 Test Summary" << endl;
        cout << "===============" << endl;
        cout << "Total tests: 7" << endl;
        cout << "Passed: 7 ✅" << endl;
        cout << "Failed: 0 ❌" << endl;
        cout << "Success rate: 100%" << endl;
        cout << "\n🎉 All tests passed!" << endl;
        
    } catch (const exception& e) {
        cerr << "❌ Test failed with exception: " << e.what() << endl;
        cleanupTestDatabase();
        return 1;
    }
    
    return 0;
}
