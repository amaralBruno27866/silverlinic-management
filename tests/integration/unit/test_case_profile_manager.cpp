#include <iostream>
#include <cassert>
#include <vector>
#include <sqlite3.h>
#include "managers/CaseProfileManager.h"
#include "managers/ClientManager.h"
#include "managers/AssessorManager.h"
#include "core/CaseProfile.h"
#include "core/Client.h"
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
    string testDbPath = DatabaseConfig::getTestDatabasePath("test_case_profile_manager");
    remove(testDbPath.c_str());
    
    // Open test database
    if (sqlite3_open(testDbPath.c_str(), &testDb) != SQLITE_OK) {
        cerr << "Failed to open test database: " << sqlite3_errmsg(testDb) << endl;
        exit(1);
    }
    
    // Enable foreign keys
    sqlite3_exec(testDb, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    
    // Create client table
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
    
    // Create case_profile table
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
    
    // Execute table creation
    if (sqlite3_exec(testDb, createClientTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK ||
        sqlite3_exec(testDb, createAssessorTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK ||
        sqlite3_exec(testDb, createCaseProfileTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Failed to create tables: " << sqlite3_errmsg(testDb) << endl;
        exit(1);
    }
    
    cout << "✅ Test database setup completed" << endl;
}

void cleanupTestDatabase() {
    if (testDb) {
        sqlite3_close(testDb);
        string testDbPath = DatabaseConfig::getTestDatabasePath("test_case_profile_manager");
        remove(testDbPath.c_str());
    }
}

void setupTestData() {
    cout << "🔧 Setting up test data..." << endl;
    
    // Create test clients and assessors
    ClientManager clientManager(testDb);
    AssessorManager assessorManager(testDb);
    
    Address address;
    DateTime now = DateTime::now();
    
    // Create test clients
    Client client1(300001, "John", "Doe", "john@test.com", "11999999999", "1990-01-15", address, now, now);
    Client client2(300002, "Jane", "Smith", "jane@test.com", "11888888888", "1985-06-20", address, now, now);
    
    // Create test assessors
    Assessor assessor1(100001, "Dr. Alice", "Johnson", "alice@clinic.com", "11777777777", address, now, now);
    Assessor assessor2(100002, "Dr. Bob", "Wilson", "bob@clinic.com", "11666666666", address, now, now);
    
    // Insert test data
    assert(clientManager.create(client1));
    assert(clientManager.create(client2));
    assert(assessorManager.create(assessor1));
    assert(assessorManager.create(assessor2));
    
    cout << "✅ Test data setup completed" << endl;
}

void test_create_case_profile() {
    cout << "\n🧪 Running test_create_case_profile..." << endl;
    
    CaseProfileManager manager(testDb);
    
    // Create test case profile
    DateTime now = DateTime::now();
    CaseProfile caseProfile(400001, 300001, 100001, "Pending", "Initial assessment needed", now, DateTime(), now);
    // Ensure status matches validation expectations (capitalization already correct). If validation rules change, adjust here.
    
    bool result = manager.create(caseProfile);
    assert(result == true);
    cout << "✅ PASS: Case profile created successfully" << endl;
    
    // Verify case profile exists
    assert(manager.exists(400001) == true);
    cout << "✅ PASS: Case profile exists in database" << endl;
    
    // Try to create duplicate (should fail)
    bool duplicateResult = manager.create(caseProfile);
    assert(duplicateResult == false);
    cout << "✅ PASS: Duplicate creation properly rejected" << endl;
    
    cout << "✅ test_create_case_profile completed successfully" << endl;
}

void test_read_operations() {
    cout << "\n🧪 Running test_read_operations..." << endl;
    
    CaseProfileManager manager(testDb);
    
    // Create additional test case profiles
    DateTime now = DateTime::now();
    CaseProfile caseProfile2(400002, 300002, 100001, "Active", "Second case", now, DateTime(), now);
    CaseProfile caseProfile3(400003, 300001, 100002, "Pending", "Third case", now, DateTime(), now);
    
    manager.create(caseProfile2);
    manager.create(caseProfile3);
    
    // Test readAll
    vector<CaseProfile> allCases = manager.readAll();
    assert(allCases.size() == 3);
    cout << "✅ PASS: ReadAll returned correct number of case profiles" << endl;
    
    // Test readById
    auto caseProfile = manager.readById(400001);
    assert(caseProfile.has_value());
    assert(caseProfile->getClientId() == 300001);
    assert(caseProfile->getAssessorId() == 100001);
    assert(caseProfile->getStatus() == CaseProfile::toString(CaseProfile::Status::Pending));
    cout << "✅ PASS: ReadById returned correct case profile" << endl;
    
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

void test_relationship_operations() {
    cout << "\n🧪 Running test_relationship_operations..." << endl;
    
    CaseProfileManager manager(testDb);
    
    // Test getCasesByClientId
    vector<CaseProfile> clientCases = manager.getCasesByClientId(300001);
    assert(clientCases.size() == 2); // Cases 400001 and 400003
    cout << "✅ PASS: GetCasesByClientId returned correct cases" << endl;
    
    // Test getCasesByAssessorId
    vector<CaseProfile> assessorCases = manager.getCasesByAssessorId(100001);
    assert(assessorCases.size() == 2); // Cases 400001 and 400002
    cout << "✅ PASS: GetCasesByAssessorId returned correct cases" << endl;
    
    // Test getCasesByClientAndAssessor
    vector<CaseProfile> specificCases = manager.getCasesByClientAndAssessor(300001, 100001);
    assert(specificCases.size() == 1); // Only case 400001
    assert(specificCases[0].getCaseProfileId() == 400001);
    cout << "✅ PASS: GetCasesByClientAndAssessor returned correct case" << endl;
    
    cout << "✅ test_relationship_operations completed successfully" << endl;
}

void test_workflow_operations() {
    cout << "\n🧪 Running test_workflow_operations..." << endl;
    
    CaseProfileManager manager(testDb);
    
    // Test activateCase
    bool activateResult = manager.activateCase(400001, 100001);
    assert(activateResult == true);
    cout << "✅ PASS: Case activated successfully" << endl;
    
    // Verify case is now active
    auto activatedCase = manager.readById(400001);
    assert(activatedCase.has_value());
    assert(activatedCase->getStatus() == CaseProfile::toString(CaseProfile::Status::Active));
    cout << "✅ PASS: Case status updated to Active" << endl;
    
    // Test closeCase
    bool closeResult = manager.closeCase(400001, 100001, "Assessment completed");
    assert(closeResult == true);
    cout << "✅ PASS: Case closed successfully" << endl;
    
    // Verify case is now closed
    auto closedCase = manager.readById(400001);
    assert(closedCase.has_value());
    assert(closedCase->getStatus() == CaseProfile::toString(CaseProfile::Status::Closed));
    cout << "✅ PASS: Case status updated to Closed" << endl;
    
    // Test transferCase
    bool transferResult = manager.transferCase(400002, 100002, 100001);
    assert(transferResult == true);
    cout << "✅ PASS: Case transferred successfully" << endl;
    
    // Verify case was transferred
    auto transferredCase = manager.readById(400002);
    assert(transferredCase.has_value());
    assert(transferredCase->getAssessorId() == 100002);
    cout << "✅ PASS: Case assessor updated after transfer" << endl;
    
    cout << "✅ test_workflow_operations completed successfully" << endl;
}

void test_search_operations() {
    cout << "\n🧪 Running test_search_operations..." << endl;
    
    CaseProfileManager manager(testDb);
    
    // Test getCasesByStatus
    vector<CaseProfile> pendingCases = manager.getCasesByStatus("Pending");
    if(pendingCases.empty()) pendingCases = manager.getCasesByStatus("PENDING");
    // After activation/closure steps, only case 400003 should remain pending
    assert(pendingCases.size() == 1);
    cout << "✅ PASS: GetCasesByStatus found correct pending cases" << endl;
    
    vector<CaseProfile> closedCases = manager.getCasesByStatus("Closed");
    if(closedCases.empty()) closedCases = manager.getCasesByStatus("CLOSED");
    assert(closedCases.size() == 1);
    cout << "✅ PASS: GetCasesByStatus found correct closed cases" << endl;
    
    cout << "✅ test_search_operations completed successfully" << endl;
}

void test_validation() {
    cout << "\n🧪 Running test_validation..." << endl;
    
    CaseProfileManager manager(testDb);
    
    DateTime now = DateTime::now();
    
    // Test invalid case profile ID range
    CaseProfile invalidCase1(200000, 300001, 100001, "Pending", "Invalid ID", now, DateTime(), now);
    bool result1 = manager.create(invalidCase1);
    assert(result1 == false);
    cout << "✅ PASS: Creation with invalid case profile ID properly rejected" << endl;
    
    // Test invalid client ID range
    CaseProfile invalidCase2(400010, 200000, 100001, "Pending", "Invalid client", now, DateTime(), now);
    bool result2 = manager.create(invalidCase2);
    assert(result2 == false);
    cout << "✅ PASS: Creation with invalid client ID properly rejected" << endl;
    
    // Test invalid assessor ID range
    CaseProfile invalidCase3(400011, 300001, 50000, "Pending", "Invalid assessor", now, DateTime(), now);
    bool result3 = manager.create(invalidCase3);
    assert(result3 == false);
    cout << "✅ PASS: Creation with invalid assessor ID properly rejected" << endl;
    
    // Test non-existent client relationship
    CaseProfile invalidCase4(400012, 399999, 100001, "Pending", "Non-existent client", now, DateTime(), now);
    bool result4 = manager.create(invalidCase4);
    assert(result4 == false);
    cout << "✅ PASS: Creation with non-existent client properly rejected" << endl;
    
    cout << "✅ test_validation completed successfully" << endl;
}

void test_delete_operations() {
    cout << "\n🧪 Running test_delete_operations..." << endl;
    
    CaseProfileManager manager(testDb);
    
    // Test canDelete (should be true for pending case)
    bool canDelete = manager.canDelete(400003);
    assert(canDelete == true);
    cout << "✅ PASS: CanDelete returned true for pending case" << endl;
    
    // Test canDelete (should be false for closed case)
    bool cannotDelete = manager.canDelete(400001);
    assert(cannotDelete == false);
    cout << "✅ PASS: CanDelete returned false for closed case" << endl;
    
    // Delete pending case (soft delete to Cancelled)
    bool deleteResult = manager.deleteById(400003);
    assert(deleteResult == true);
    cout << "✅ PASS: Case soft deleted successfully" << endl;
    
    // Verify case is marked as cancelled
    auto deletedCase = manager.readById(400003);
    assert(deletedCase.has_value());
    assert(deletedCase->getStatus() == CaseProfile::toString(CaseProfile::Status::Cancelled));
    cout << "✅ PASS: Case marked as Cancelled after deletion" << endl;
    
    cout << "✅ test_delete_operations completed successfully" << endl;
}

int main() {
    cout << "🚀 CaseProfileManager Unit Tests" << endl;
    cout << "=================================" << endl;
    
    try {
        setupTestDatabase();
        setupTestData();
        
        test_create_case_profile();
        test_read_operations();
        test_relationship_operations();
        test_workflow_operations();
        test_search_operations();
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
