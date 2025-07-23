#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include "managers/AssessorManager.h"
#include "core/Assessor.h"
#include "core/Address.h"
#include "core/DateTime.h"
#include "core/Utils.h"
#include "core/DatabaseConfig.h"

using namespace std;
using namespace SilverClinic;

sqlite3* db = nullptr;

void initializeDatabase() {
    cout << "🔧 Initializing database..." << endl;
    
    // Ensure directories exist
    DatabaseConfig::ensureDirectoriesExist();
    
    if (sqlite3_open(DatabaseConfig::DEMO_DATABASE_PATH.c_str(), &db) != SQLITE_OK) {
        cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
    
    // Enable foreign keys
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    
    // Create tables (same as in main.cpp)
    string createAssessorTable = R"(
        CREATE TABLE IF NOT EXISTS assessor(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    
    string createAddressTable = R"(
        CREATE TABLE IF NOT EXISTS address(
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
    
    string createCaseProfileTable = R"(
        CREATE TABLE IF NOT EXISTS case_profile(
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
    
    sqlite3_exec(db, createAssessorTable.c_str(), nullptr, nullptr, nullptr);
    sqlite3_exec(db, createAddressTable.c_str(), nullptr, nullptr, nullptr);
    sqlite3_exec(db, createCaseProfileTable.c_str(), nullptr, nullptr, nullptr);
    
    cout << "✅ Database initialized successfully" << endl;
}

void displayAssessorTable(const vector<Assessor>& assessors) {
    cout << "\n📋 Assessors List" << endl;
    cout << "=================" << endl;
    cout << setw(8) << "ID" << " | " 
         << setw(15) << "Last Name" << " | " 
         << setw(15) << "First Name" << " | " 
         << setw(20) << "Email" << " | " 
         << setw(15) << "Phone" << " | " 
         << "Actions" << endl;
    cout << string(90, '-') << endl;
    
    for (const auto& assessor : assessors) {
        cout << setw(8) << assessor.getAssessorId() << " | " 
             << setw(15) << assessor.getLastName().substr(0, 15) << " | " 
             << setw(15) << assessor.getFirstName().substr(0, 15) << " | " 
             << setw(20) << assessor.getEmail().substr(0, 20) << " | " 
             << setw(15) << assessor.getPhone() << " | " 
             << "View | Edit | Delete | Cases" << endl;
    }
    
    if (assessors.empty()) {
        cout << "No assessors found." << endl;
    }
}

void demonstrateCreateOperation(AssessorManager& manager) {
    cout << "\n🆕 CREATE Operation Demo" << endl;
    cout << "========================" << endl;
    
    // Create sample assessors
    Address address;
    DateTime now = DateTime::now();
    
    vector<Assessor> sampleAssessors = {
        Assessor(100001, "Dr. Ana", "Silva", "ana.silva@clinic.com", "11999887766", address, now, now),
        Assessor(100002, "Dr. Carlos", "Santos", "carlos.santos@clinic.com", "11888776655", address, now, now),
        Assessor(100003, "Dra. Maria", "Oliveira", "maria.oliveira@clinic.com", "11777665544", address, now, now),
        Assessor(100004, "Dr. Pedro", "Costa", "pedro.costa@clinic.com", "11666554433", address, now, now)
    };
    
    for (const auto& assessor : sampleAssessors) {
        if (manager.create(assessor)) {
            cout << "✅ Created: " << assessor.getFullName() << " (ID: " << assessor.getAssessorId() << ")" << endl;
        } else {
            cout << "❌ Failed to create: " << assessor.getFullName() << endl;
        }
    }
    
    cout << "\n📊 Total assessors in database: " << manager.getCount() << endl;
}

void demonstrateReadOperations(AssessorManager& manager) {
    cout << "\n📖 READ Operations Demo" << endl;
    cout << "=======================" << endl;
    
    // Read all assessors
    vector<Assessor> allAssessors = manager.readAll();
    displayAssessorTable(allAssessors);
    
    // Read specific assessor
    cout << "\n🔍 Reading specific assessor (ID: 100002):" << endl;
    auto assessor = manager.readById(100002);
    if (assessor.has_value()) {
        assessor->displayInfo();
    } else {
        cout << "Assessor not found." << endl;
    }
    
    // Pagination demo
    cout << "\n📄 Pagination Demo (2 per page):" << endl;
    vector<Assessor> page1 = manager.readWithPagination(2, 0);
    cout << "Page 1:" << endl;
    displayAssessorTable(page1);
    
    vector<Assessor> page2 = manager.readWithPagination(2, 2);
    cout << "\nPage 2:" << endl;
    displayAssessorTable(page2);
}

void demonstrateSearchOperations(AssessorManager& manager) {
    cout << "\n🔍 SEARCH Operations Demo" << endl;
    cout << "=========================" << endl;
    
    // Search by name
    cout << "Searching for 'Silva':" << endl;
    vector<Assessor> results = manager.searchByName("Silva");
    displayAssessorTable(results);
    
    // Find by email
    cout << "\nSearching by email 'maria.oliveira@clinic.com':" << endl;
    auto assessor = manager.findByEmail("MARIA.OLIVEIRA@CLINIC.COM");
    if (assessor.has_value()) {
        cout << "Found: " << assessor->getFullName() << " (ID: " << assessor->getAssessorId() << ")" << endl;
    } else {
        cout << "Assessor not found by email." << endl;
    }
}

void demonstrateUpdateOperation(AssessorManager& manager) {
    cout << "\n✏️ UPDATE Operation Demo" << endl;
    cout << "========================" << endl;
    
    // Get an assessor to update
    auto assessor = manager.readById(100001);
    if (assessor.has_value()) {
        cout << "Original data:" << endl;
        assessor->displayInfo();
        
        // Update the assessor
        Assessor updatedAssessor = assessor.value();
        updatedAssessor.setFirstName("Dr. Ana Carolina");
        updatedAssessor.setPhone("11999888777");
        
        if (manager.update(updatedAssessor)) {
            cout << "\n✅ Update successful! New data:" << endl;
            auto verifyAssessor = manager.readById(100001);
            if (verifyAssessor.has_value()) {
                verifyAssessor->displayInfo();
            }
        } else {
            cout << "❌ Update failed!" << endl;
        }
    }
}

void demonstrateDeleteOperation(AssessorManager& manager) {
    cout << "\n🗑️ DELETE Operation Demo" << endl;
    cout << "========================" << endl;
    
    int assessorToDelete = 100004;
    
    // Check if can delete
    if (manager.canDelete(assessorToDelete)) {
        cout << "✅ Safe to delete assessor " << assessorToDelete << " (no associated cases)" << endl;
        
        if (manager.deleteById(assessorToDelete)) {
            cout << "✅ Assessor deleted successfully!" << endl;
            cout << "📊 Remaining assessors: " << manager.getCount() << endl;
        } else {
            cout << "❌ Delete failed!" << endl;
        }
    } else {
        cout << "⚠️ Cannot delete assessor " << assessorToDelete << " (has associated cases)" << endl;
    }
}

void demonstrateErrorHandling(AssessorManager& manager) {
    cout << "\n⚠️ ERROR HANDLING Demo" << endl;
    cout << "======================" << endl;
    
    // Try to create invalid assessor
    cout << "Attempting to create assessor with invalid data:" << endl;
    Address address;
    DateTime now = DateTime::now();
    Assessor invalidAssessor(50000, "", "Invalid", "not-an-email", "invalid-phone", address, now, now);
    
    if (!manager.create(invalidAssessor)) {
        cout << "✅ Invalid assessor creation properly rejected" << endl;
    }
    
    // Try to read non-existent assessor
    cout << "\nAttempting to read non-existent assessor (ID: 999999):" << endl;
    auto nonExistent = manager.readById(999999);
    if (!nonExistent.has_value()) {
        cout << "✅ Non-existent assessor properly handled" << endl;
    }
    
    // Try to delete non-existent assessor
    cout << "\nAttempting to delete non-existent assessor (ID: 999999):" << endl;
    if (!manager.deleteById(999999)) {
        cout << "✅ Non-existent assessor deletion properly rejected" << endl;
    }
}

void cleanup() {
    if (db) {
        sqlite3_close(db);
    }
}

int main() {
    cout << "🏥 AssessorManager CRUD Operations Demo" << endl;
    cout << "=======================================" << endl;
    cout << "This demo shows how to use AssessorManager for database operations." << endl;
    
    try {
        initializeDatabase();
        AssessorManager manager(db);
        
        demonstrateCreateOperation(manager);
        demonstrateReadOperations(manager);
        demonstrateSearchOperations(manager);
        demonstrateUpdateOperation(manager);
        demonstrateDeleteOperation(manager);
        demonstrateErrorHandling(manager);
        
        cleanup();
        
        cout << "\n🎉 Demo completed successfully!" << endl;
        cout << "Database file: assessor_demo.db" << endl;
        cout << "You can inspect the database using SQLite tools." << endl;
        
    } catch (const exception& e) {
        cerr << "❌ Demo failed with exception: " << e.what() << endl;
        cleanup();
        return 1;
    }
    
    return 0;
}
