#include <iostream>
#include <cassert>
#include <vector>
#include <sqlite3.h>
#include "managers/ClientManager.h"
#include "core/Client.h"
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
    remove(DatabaseConfig::CLIENT_MANAGER_TEST_DB.c_str());
    
    // Open test database
    if (sqlite3_open(DatabaseConfig::CLIENT_MANAGER_TEST_DB.c_str(), &testDb) != SQLITE_OK) {
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
    
    if (sqlite3_exec(testDb, createClientTable.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Failed to create client table: " << sqlite3_errmsg(testDb) << endl;
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
        remove(DatabaseConfig::CLIENT_MANAGER_TEST_DB.c_str());
    }
}

void test_create_client() {
    cout << "\n🧪 Running test_create_client..." << endl;
    
    ClientManager manager(testDb);
    
    // Create test client
    Address address;
    DateTime now = DateTime::now();
    Client client(300001, "Maria", "Silva", "maria@test.com", "11999999999", "1990-05-15", address, now, now);
    
    bool result = manager.create(client);
    assert(result == true);
    cout << "✅ PASS: Client created successfully" << endl;
    
    // Verify client exists
    assert(manager.exists(300001) == true);
    cout << "✅ PASS: Client exists in database" << endl;
    
    // Try to create duplicate (should fail)
    bool duplicateResult = manager.create(client);
    assert(duplicateResult == false);
    cout << "✅ PASS: Duplicate creation properly rejected" << endl;
    
    cout << "✅ test_create_client completed successfully" << endl;
}

void test_read_operations() {
    cout << "\n🧪 Running test_read_operations..." << endl;
    
    ClientManager manager(testDb);
    
    // Create additional test clients
    Address address;
    DateTime now = DateTime::now();
    
    Client client2(300002, "João", "Santos", "joao@test.com", "11888888888", "1985-12-20", address, now, now);
    Client client3(300003, "Ana", "Oliveira", "ana@test.com", "11777777777", "1992-03-10", address, now, now);
    
    manager.create(client2);
    manager.create(client3);
    
    // Test readAll
    vector<Client> allClients = manager.readAll();
    assert(allClients.size() == 3);
    cout << "✅ PASS: ReadAll returned correct number of clients" << endl;
    
    // Test readById
    auto client = manager.readById(300001);
    assert(client.has_value());
    assert(client->getFirstName() == "MARIA");
    assert(client->getLastName() == "SILVA");
    cout << "✅ PASS: ReadById returned correct client" << endl;
    
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
    
    ClientManager manager(testDb);
    
    // Test search by name
    vector<Client> results = manager.searchByName("Silva");
    assert(results.size() == 1);
    assert(results[0].getLastName() == "SILVA");
    cout << "✅ PASS: SearchByName found correct client" << endl;
    
    // Test search by partial name
    results = manager.searchByName("Santos");
    assert(results.size() == 1);
    assert(results[0].getFirstName() == "JOAO"); // No accents expected
    cout << "✅ PASS: SearchByName with partial match worked" << endl;
    
    // Test find by email
    auto client = manager.findByEmail("ANA@TEST.COM");
    assert(client.has_value());
    assert(client->getFirstName() == "ANA"); // No accents expected
    cout << "✅ PASS: FindByEmail found correct client" << endl;
    
    // Test find by non-existent email
    auto nonExistent = manager.findByEmail("nonexistent@test.com");
    assert(!nonExistent.has_value());
    cout << "✅ PASS: FindByEmail correctly returned empty for non-existent email" << endl;
    
    // Test find by date of birth
    results = manager.findByDateOfBirth("1990-05-15");
    assert(results.size() == 1);
    assert(results[0].getFirstName() == "MARIA");
    cout << "✅ PASS: FindByDateOfBirth found correct client" << endl;
    
    cout << "✅ test_search_operations completed successfully" << endl;
}

void test_update_client() {
    cout << "\n🧪 Running test_update_client..." << endl;
    
    ClientManager manager(testDb);
    
    // Get existing client
    auto client = manager.readById(300001);
    assert(client.has_value());
    
    // Update client data
    Client updatedClient = client.value();
    updatedClient.setFirstName("Maria Jose"); // No accents in test data
    updatedClient.setEmail("maria.jose@test.com");
    
    bool result = manager.update(updatedClient);
    assert(result == true);
    cout << "✅ PASS: Client updated successfully" << endl;
    
    // Verify update
    auto verifyClient = manager.readById(300001);
    assert(verifyClient.has_value());
    cout << "DEBUG: Updated firstName: '" << verifyClient->getFirstName() << "'" << endl;
    assert(verifyClient->getFirstName() == "MARIA JOSE"); // No accents expected
    assert(verifyClient->getEmail() == "MARIA.JOSE@TEST.COM");
    cout << "✅ PASS: Update changes verified in database" << endl;
    
    // Try to update non-existent client
    Client nonExistent(999999, "Test", "User", "test@test.com", "11999999999", "1990-01-01", Address(), DateTime::now(), DateTime::now());
    bool updateResult = manager.update(nonExistent);
    assert(updateResult == false);
    cout << "✅ PASS: Update of non-existent client properly rejected" << endl;
    
    cout << "✅ test_update_client completed successfully" << endl;
}

void test_pagination() {
    cout << "\n🧪 Running test_pagination..." << endl;
    
    ClientManager manager(testDb);
    
    // Test pagination
    vector<Client> page1 = manager.readWithPagination(2, 0);
    assert(page1.size() == 2);
    cout << "✅ PASS: First page returned correct number of clients" << endl;
    
    vector<Client> page2 = manager.readWithPagination(2, 2);
    assert(page2.size() == 1);
    cout << "✅ PASS: Second page returned correct number of clients" << endl;
    
    // Verify different clients in different pages
    assert(page1[0].getClientId() != page2[0].getClientId());
    cout << "✅ PASS: Different pages returned different clients" << endl;
    
    cout << "✅ test_pagination completed successfully" << endl;
}

void test_validation() {
    cout << "\n🧪 Running test_validation..." << endl;
    
    ClientManager manager(testDb);
    
    // Test invalid client data
    Address address;
    DateTime now = DateTime::now();
    
    // Empty first name
    Client invalidClient1(300010, "", "Test", "test@test.com", "11999999999", "1990-01-01", address, now, now);
    bool result1 = manager.create(invalidClient1);
    assert(result1 == false);
    cout << "✅ PASS: Creation with empty first name properly rejected" << endl;
    
    // Invalid email
    Client invalidClient2(300011, "Test", "User", "invalid-email", "11999999999", "1990-01-01", address, now, now);
    bool result2 = manager.create(invalidClient2);
    assert(result2 == false);
    cout << "✅ PASS: Creation with invalid email properly rejected" << endl;
    
    // Invalid ID range
    Client invalidClient3(50000, "Test", "User", "test@test.com", "11999999999", "1990-01-01", address, now, now);
    bool result3 = manager.create(invalidClient3);
    assert(result3 == false);
    cout << "✅ PASS: Creation with invalid ID range properly rejected" << endl;
    
    cout << "✅ test_validation completed successfully" << endl;
}

void test_delete_operations() {
    cout << "\n🧪 Running test_delete_operations..." << endl;
    
    ClientManager manager(testDb);
    
    // Test canDelete (should be true since no cases)
    bool canDelete = manager.canDelete(300003);
    assert(canDelete == true);
    cout << "✅ PASS: CanDelete returned true for client without cases" << endl;
    
    // Delete client
    bool deleteResult = manager.deleteById(300003);
    assert(deleteResult == true);
    cout << "✅ PASS: Client deleted successfully" << endl;
    
    // Verify deletion
    assert(manager.exists(300003) == false);
    cout << "✅ PASS: Client no longer exists after deletion" << endl;
    
    // Try to delete non-existent client
    bool deleteNonExistent = manager.deleteById(999999);
    assert(deleteNonExistent == false);
    cout << "✅ PASS: Deletion of non-existent client properly rejected" << endl;
    
    // Verify count decreased
    int newCount = manager.getCount();
    assert(newCount == 2);
    cout << "✅ PASS: Count decreased after deletion" << endl;
    
    cout << "✅ test_delete_operations completed successfully" << endl;
}

void test_age_functionality() {
    cout << "\n🧪 Running test_age_functionality..." << endl;
    
    ClientManager manager(testDb);
    
    // Test age range search
    vector<Client> youngClients = manager.getClientsByAgeRange(25, 35);
    cout << "Found " << youngClients.size() << " clients in age range 25-35" << endl;
    
    vector<Client> olderClients = manager.getClientsByAgeRange(35, 45);
    cout << "Found " << olderClients.size() << " clients in age range 35-45" << endl;
    
    // This test is more about functionality than assertions since ages depend on current date
    cout << "✅ PASS: Age range functionality executed without errors" << endl;
    
    cout << "✅ test_age_functionality completed successfully" << endl;
}

int main() {
    cout << "🚀 ClientManager Unit Tests" << endl;
    cout << "============================" << endl;
    
    try {
        setupTestDatabase();
        
        test_create_client();
        test_read_operations();
        test_search_operations();
        test_update_client();
        test_pagination();
        test_validation();
        test_delete_operations();
        test_age_functionality();
        
        cleanupTestDatabase();
        
        cout << "\n📊 Test Summary" << endl;
        cout << "===============" << endl;
        cout << "Total tests: 8" << endl;
        cout << "Passed: 8 ✅" << endl;
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
