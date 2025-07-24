#include "core/Address.h"
#include "core/DateTime.h"
#include "core/Assessor.h"
#include "core/Client.h"
#include "core/CaseProfile.h"
#include "core/Utils.h"
#include "core/DatabaseConfig.h"
#include "forms/FamilyPhysician.h"
#include "forms/AutomobileAnxietyInventory.h"
#include "forms/BeckDepressionInventory.h"
#include <iostream>
#include <sqlite3.h>
#include <string>

using namespace std;
using namespace SilverClinic;
using namespace utils;

// Function to execute SQL and check errors
bool executeSQLCommand(sqlite3* db, const string& sql, const string& description) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        cerr << "Error executing " << description << ": " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    
    logMessage("INFO", description + " executed successfully");
    return true;
}

// Function to create database tables
bool createDatabaseTables(sqlite3* db) {
    logMessage("INFO", "Creating database tables...");
    
    // Enable foreign keys in SQLite
    if (!executeSQLCommand(db, "PRAGMA foreign_keys = ON;", "Foreign keys activation")) {
        return false;
    }
    
    // Assessor table
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
    
    if (!executeSQLCommand(db, createAssessorTable, "Assessor table creation")) {
        return false;
    }
    
    // Client table
    string createClientTable = R"(
        CREATE TABLE IF NOT EXISTS client(
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
    
    if (!executeSQLCommand(db, createClientTable, "Client table creation")) {
        return false;
    }
    
    // Case profile table
    string createCaseProfileTable = R"(
        CREATE TABLE IF NOT EXISTS case_profile(
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
    
    if (!executeSQLCommand(db, createCaseProfileTable, "CaseProfile table creation")) {
        return false;
    }
    
    // Address table
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
    
    if (!executeSQLCommand(db, createAddressTable, "Address table creation")) {
        return false;
    }
    
    // Family Physician form table
    string createFamilyPhysicianTable = R"(
        CREATE TABLE IF NOT EXISTS family_physician(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'FPH',
            client_id INTEGER NOT NULL,
            full_name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            address_id INTEGER,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id),
            FOREIGN KEY (address_id) REFERENCES address(id)
        )
    )";
    
    if (!executeSQLCommand(db, createFamilyPhysicianTable, "FamilyPhysician table creation")) {
        return false;
    }
    
    // Emergency Contact form table
    string createEmergencyContactTable = R"(
        CREATE TABLE IF NOT EXISTS emergency_contact(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'ECT',
            client_id INTEGER NOT NULL,
            full_name TEXT NOT NULL,
            relationship TEXT,
            phone TEXT,
            alt_phone TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id)
        )
    )";
    
    if (!executeSQLCommand(db, createEmergencyContactTable, "EmergencyContact table creation")) {
        return false;
    }
    
    // Legal Representative form table
    string createLegalRepresentativeTable = R"(
        CREATE TABLE IF NOT EXISTS legal_representative(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'LRT',
            client_id INTEGER NOT NULL,
            company_name TEXT NOT NULL,
            full_name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id)
        )
    )";
    
    if (!executeSQLCommand(db, createLegalRepresentativeTable, "LegalRepresentative table creation")) {
        return false;
    }
    
    // Insurance Company form table
    string createInsuranceCompanyTable = R"(
        CREATE TABLE IF NOT EXISTS insurance_company(
            id INTEGER PRIMARY KEY,
            type TEXT NOT NULL DEFAULT 'ICO',
            client_id INTEGER NOT NULL,
            company_name TEXT NOT NULL,
            adjuster_name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            claim_number TEXT,
            policy_number TEXT,
            date_accident TEXT NOT NULL,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (client_id) REFERENCES client(id)
        )
    )";
    
    if (!executeSQLCommand(db, createInsuranceCompanyTable, "InsuranceCompany table creation")) {
        return false;
    }
    
    // Automobile Anxiety Inventory form table
    string createAutomobileAnxietyInventoryTable = R"(
        CREATE TABLE IF NOT EXISTS automobile_anxiety_inventory(
            id INTEGER PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'AAI',
            question_1 BOOLEAN DEFAULT 0,
            question_2 BOOLEAN DEFAULT 0,
            question_3 BOOLEAN DEFAULT 0,
            question_4 BOOLEAN DEFAULT 0,
            question_5 BOOLEAN DEFAULT 0,
            question_6 BOOLEAN DEFAULT 0,
            question_7 BOOLEAN DEFAULT 0,
            question_8 BOOLEAN DEFAULT 0,
            question_9 BOOLEAN DEFAULT 0,
            question_10 BOOLEAN DEFAULT 0,
            question_11 BOOLEAN DEFAULT 0,
            question_12 BOOLEAN DEFAULT 0,
            question_13 BOOLEAN DEFAULT 0,
            question_14_driver BOOLEAN DEFAULT 0,
            question_14_passenger BOOLEAN DEFAULT 0,
            question_14_no_difference BOOLEAN DEFAULT 0,
            question_15_a BOOLEAN DEFAULT 0,
            question_15_b TEXT,
            question_16 BOOLEAN DEFAULT 0,
            question_17 BOOLEAN DEFAULT 0,
            question_18 BOOLEAN DEFAULT 0,
            question_19 BOOLEAN DEFAULT 0,
            question_19_sidewalks BOOLEAN DEFAULT 0,
            question_19_crossing BOOLEAN DEFAULT 0,
            question_19_both BOOLEAN DEFAULT 0,
            question_20 BOOLEAN DEFAULT 0,
            question_21 BOOLEAN DEFAULT 0,
            question_22 BOOLEAN DEFAULT 0,
            question_23 BOOLEAN DEFAULT 0,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
    
    if (!executeSQLCommand(db, createAutomobileAnxietyInventoryTable, "AutomobileAnxietyInventory table creation")) {
        return false;
    }
    
    // Beck Depression Inventory form table
    string createBeckDepressionInventoryTable = R"(
        CREATE TABLE IF NOT EXISTS beck_depression_inventory(
            id INTEGER PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'BDI',
            question_1 INTEGER DEFAULT 0 CHECK(question_1 >= 0 AND question_1 <= 3),
            question_2 INTEGER DEFAULT 0 CHECK(question_2 >= 0 AND question_2 <= 3),
            question_3 INTEGER DEFAULT 0 CHECK(question_3 >= 0 AND question_3 <= 3),
            question_4 INTEGER DEFAULT 0 CHECK(question_4 >= 0 AND question_4 <= 3),
            question_5 INTEGER DEFAULT 0 CHECK(question_5 >= 0 AND question_5 <= 3),
            question_6 INTEGER DEFAULT 0 CHECK(question_6 >= 0 AND question_6 <= 3),
            question_7 INTEGER DEFAULT 0 CHECK(question_7 >= 0 AND question_7 <= 3),
            question_8 INTEGER DEFAULT 0 CHECK(question_8 >= 0 AND question_8 <= 3),
            question_9 INTEGER DEFAULT 0 CHECK(question_9 >= 0 AND question_9 <= 3),
            question_10 INTEGER DEFAULT 0 CHECK(question_10 >= 0 AND question_10 <= 3),
            question_11 INTEGER DEFAULT 0 CHECK(question_11 >= 0 AND question_11 <= 3),
            question_12 INTEGER DEFAULT 0 CHECK(question_12 >= 0 AND question_12 <= 3),
            question_13 INTEGER DEFAULT 0 CHECK(question_13 >= 0 AND question_13 <= 3),
            question_14 INTEGER DEFAULT 0 CHECK(question_14 >= 0 AND question_14 <= 3),
            question_15 INTEGER DEFAULT 0 CHECK(question_15 >= 0 AND question_15 <= 3),
            question_16 INTEGER DEFAULT 0 CHECK(question_16 >= 0 AND question_16 <= 3),
            question_17 INTEGER DEFAULT 0 CHECK(question_17 >= 0 AND question_17 <= 3),
            question_18 INTEGER DEFAULT 0 CHECK(question_18 >= 0 AND question_18 <= 3),
            question_19 INTEGER DEFAULT 0 CHECK(question_19 >= 0 AND question_19 <= 3),
            question_20 INTEGER DEFAULT 0 CHECK(question_20 >= 0 AND question_20 <= 3),
            question_21 INTEGER DEFAULT 0 CHECK(question_21 >= 0 AND question_21 <= 3),
            total_score INTEGER DEFAULT 0,
            severity_level TEXT DEFAULT 'Minimal',
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
    
    if (!executeSQLCommand(db, createBeckDepressionInventoryTable, "BeckDepressionInventory table creation")) {
        return false;
    }
    
    logMessage("INFO", "All database tables created successfully");
    return true;
}

// Function to insert sample data
bool insertSampleData(sqlite3* db) {
    logMessage("INFO", "Inserting sample data...");
    
    string currentTime = getCurrentTimestamp();
    
    // Generate IDs with correct prefixes
    int assessorId = Assessor::ID_PREFIX + 1; // 100001
    int clientId = Client::ID_PREFIX + 1;     // 300001
    int addressId1 = Address::ID_PREFIX + 1;  // 200001
    int addressId2 = Address::ID_PREFIX + 2;  // 200002
    
    // Insert sample assessor
    string insertAssessor = R"(
        INSERT OR IGNORE INTO assessor (id, firstname, lastname, phone, email, created_at, modified_at)
        VALUES ()" + to_string(assessorId) + R"(, 'Dr. Maria', 'Silva', '416-555-0123', 'maria.silva@silverclinic.ca', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAssessor, "Sample assessor insertion")) {
        return false;
    }
    
    // Insert sample client
    string insertClient = R"(
        INSERT OR IGNORE INTO client (id, firstname, lastname, phone, email, date_of_birth, created_at, modified_at)
        VALUES ()" + to_string(clientId) + R"(, 'João', 'Santos', '416-555-9876', 'joao.santos@email.com', '1985-03-15', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertClient, "Sample client insertion")) {
        return false;
    }
    
    // Insert sample case profile
    int caseProfileId = CaseProfile::ID_PREFIX + 1;
    string insertCaseProfile = R"(
        INSERT OR IGNORE INTO case_profile (id, client_id, assessor_id, status, notes, created_at, modified_at)
        VALUES ()" + to_string(caseProfileId) + ", " + to_string(clientId) + ", " + to_string(assessorId) + R"(, 'Pending', 'Initial case profile for mental health assessment', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertCaseProfile, "Sample case profile insertion")) {
        return false;
    }
    
    // Insert assessor address (user_key = assessorId)
    string insertAddressAssessor = R"(
        INSERT OR IGNORE INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES ()" + to_string(addressId1) + ", " + to_string(assessorId) + R"(, '456 Healthcare St', 'Toronto', 'ON', 'M5H 2N2', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAddressAssessor, "Sample assessor address insertion")) {
        return false;
    }
    
    // Insert client address (user_key = clientId)
    string insertAddressClient = R"(
        INSERT OR IGNORE INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES ()" + to_string(addressId2) + ", " + to_string(clientId) + R"(, '123 Main Street', 'Toronto', 'ON', 'M5V 3A8', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAddressClient, "Sample client address insertion")) {
        return false;
    }
    
    logMessage("INFO", "Sample data inserted successfully");
    return true;
}

int main() {
    cout << "🏥 Silver Clinic Management System" << endl;
    cout << "===================================" << endl;
    cout << "Welcome to the Silver Clinic Management System!" << endl;
    cout << "This system helps manage clinical assessments and client data." << endl;
    
    try {
        // Ensure data directories exist
        DatabaseConfig::ensureDirectoriesExist();
        
        // Setup and open SQLite database
        sqlite3* db;
        int result = sqlite3_open(DatabaseConfig::MAIN_DATABASE_PATH.c_str(), &db);
        
        if (result != SQLITE_OK) {
            cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
            return 1;
        }
        
        logMessage("INFO", "Database opened successfully: clinic.db");
        
        // Create database tables
        if (!createDatabaseTables(db)) {
            cerr << "Failed to create database tables" << endl;
            sqlite3_close(db);
            return 1;
        }
        
        // Insert sample data (only if tables are empty)
        if (!insertSampleData(db)) {
            cerr << "Failed to insert sample data" << endl;
            sqlite3_close(db);
            return 1;
        }
        
        // Display database information
        cout << "\n📊 Database Status" << endl;
        cout << "==================" << endl;
        
        // Query database information
        sqlite3_stmt* stmt;
        
        // Count assessors
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM assessor", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Assessors in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count clients
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM client", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Clients in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count addresses
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM address", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Addresses in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count case profiles
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM case_profile", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Case Profiles in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count automobile anxiety inventories
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM automobile_anxiety_inventory", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "AAI Forms in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count beck depression inventories
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM beck_depression_inventory", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "BDI Forms in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        cout << "\n✅ System initialized successfully!" << endl;
        cout << "Database file: clinic.db" << endl;
        cout << "To run tests, use: ./run_tests.sh" << endl;
        cout << "===================================" << endl;
        
        // Close database
        sqlite3_close(db);
        logMessage("INFO", "Database connection closed");
        
    } catch (const exception& e) {
        cerr << "Error occurred: " << e.what() << endl;
        logMessage("ERROR", "Application failed to initialize: " + string(e.what()));
        return 1;
    }
    
    return 0;
}
