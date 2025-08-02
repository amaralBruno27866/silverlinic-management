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
#include "forms/BeckAnxietyInventory.h"
#include "forms/PainBodyMap.h"
#include "forms/ActivitiesOfDailyLiving.h"
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <filesystem>
#include <unistd.h>

using namespace std;
using namespace SilverClinic;
using namespace utils;

// Function to ensure we're running from the correct directory
bool ensureCorrectWorkingDirectory() {
    string currentDir = filesystem::current_path().string();
    string executablePath = filesystem::canonical("/proc/self/exe").parent_path().string();
    
    // Check if we're in a build directory and need to go up one level
    if (currentDir.find("build") != string::npos || 
        currentDir.find("Debug") != string::npos || 
        currentDir.find("Release") != string::npos) {
        
        // Try to find the project root by looking for CMakeLists.txt or data/ directory
        filesystem::path testPath = filesystem::current_path();
        bool foundProjectRoot = false;
        
        // Go up directories until we find project root indicators
        for (int i = 0; i < 3; ++i) {
            if (filesystem::exists(testPath / "CMakeLists.txt") && 
                filesystem::exists(testPath / "data") && 
                filesystem::exists(testPath / "src")) {
                
                filesystem::current_path(testPath);
                cout << "📂 Auto-corrected working directory to: " << testPath << endl;
                foundProjectRoot = true;
                break;
            }
            testPath = testPath.parent_path();
        }
        
        if (!foundProjectRoot) {
            cerr << "❌ ERROR: Could not locate project root directory!" << endl;
            cerr << "   Current directory: " << currentDir << endl;
            cerr << "   Please run from project root directory containing:" << endl;
            cerr << "   - CMakeLists.txt" << endl;
            cerr << "   - data/ directory" << endl;
            cerr << "   - src/ directory" << endl;
            return false;
        }
    }
    
    // Final validation - ensure required directories exist
    if (!filesystem::exists("data") || !filesystem::exists("src")) {
        cerr << "❌ ERROR: Required directories not found!" << endl;
        cerr << "   Current directory: " << filesystem::current_path() << endl;
        cerr << "   Missing: " << (!filesystem::exists("data") ? "data/ " : "") 
             << (!filesystem::exists("src") ? "src/ " : "") << endl;
        return false;
    }
    
    return true;
}

// Function to validate database integrity
bool validateDatabaseIntegrity(sqlite3* db) {
    // Check if database file is not empty (size > 0)
    string dbPath = DatabaseConfig::MAIN_DATABASE_PATH;
    if (filesystem::file_size(dbPath) == 0) {
        logMessage("WARNING", "Database file is empty, will initialize with new tables");
        return true; // Empty DB is OK, we'll create tables
    }
    
    // Test basic database connectivity
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, "SELECT name FROM sqlite_master WHERE type='table' LIMIT 1;", 
                             nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        cerr << "❌ Database integrity check failed: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    
    logMessage("INFO", "Database integrity validated successfully");
    return true;
}

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
    
    // Beck Anxiety Inventory form table
    string createBeckAnxietyInventoryTable = R"(
        CREATE TABLE IF NOT EXISTS beck_anxiety_inventory(
            id INTEGER PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'BAI',
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
    
    if (!executeSQLCommand(db, createBeckAnxietyInventoryTable, "BeckAnxietyInventory table creation")) {
        return false;
    }
    
    // Pain Body Map form table
    string createPainBodyMapTable = R"(
        CREATE TABLE IF NOT EXISTS pain_body_map(
            id INTEGER PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'PBM',
            pain_data_json TEXT NOT NULL DEFAULT '{}',
            additional_comments TEXT CHECK(length(additional_comments) <= 1000),
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
    
    if (!executeSQLCommand(db, createPainBodyMapTable, "PainBodyMap table creation")) {
        return false;
    }
    
    // Activities of Daily Living form table
    string createActivitiesOfDailyLivingTable = R"(
        CREATE TABLE IF NOT EXISTS activities_of_daily_living(
            id INTEGER PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'ADL',
            activities_data_json TEXT NOT NULL DEFAULT '{}',
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
    
    if (!executeSQLCommand(db, createActivitiesOfDailyLivingTable, "ActivitiesOfDailyLiving table creation")) {
        return false;
    }
    
    // SCL-90-R form table
    string createSCL90RTable = R"(
        CREATE TABLE IF NOT EXISTS scl90r(
            scl_id INTEGER PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            type TEXT NOT NULL DEFAULT 'SCL90R',
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
            question_22 INTEGER DEFAULT 0 CHECK(question_22 >= 0 AND question_22 <= 3),
            question_23 INTEGER DEFAULT 0 CHECK(question_23 >= 0 AND question_23 <= 3),
            question_24 INTEGER DEFAULT 0 CHECK(question_24 >= 0 AND question_24 <= 3),
            question_25 INTEGER DEFAULT 0 CHECK(question_25 >= 0 AND question_25 <= 3),
            question_26 INTEGER DEFAULT 0 CHECK(question_26 >= 0 AND question_26 <= 3),
            question_27 INTEGER DEFAULT 0 CHECK(question_27 >= 0 AND question_27 <= 3),
            question_28 INTEGER DEFAULT 0 CHECK(question_28 >= 0 AND question_28 <= 3),
            question_29 INTEGER DEFAULT 0 CHECK(question_29 >= 0 AND question_29 <= 3),
            question_30 INTEGER DEFAULT 0 CHECK(question_30 >= 0 AND question_30 <= 3),
            question_31 INTEGER DEFAULT 0 CHECK(question_31 >= 0 AND question_31 <= 3),
            question_32 INTEGER DEFAULT 0 CHECK(question_32 >= 0 AND question_32 <= 3),
            question_33 INTEGER DEFAULT 0 CHECK(question_33 >= 0 AND question_33 <= 3),
            question_34 INTEGER DEFAULT 0 CHECK(question_34 >= 0 AND question_34 <= 3),
            question_35 INTEGER DEFAULT 0 CHECK(question_35 >= 0 AND question_35 <= 3),
            question_36 INTEGER DEFAULT 0 CHECK(question_36 >= 0 AND question_36 <= 3),
            question_37 INTEGER DEFAULT 0 CHECK(question_37 >= 0 AND question_37 <= 3),
            question_38 INTEGER DEFAULT 0 CHECK(question_38 >= 0 AND question_38 <= 3),
            question_39 INTEGER DEFAULT 0 CHECK(question_39 >= 0 AND question_39 <= 3),
            question_40 INTEGER DEFAULT 0 CHECK(question_40 >= 0 AND question_40 <= 3),
            question_41 INTEGER DEFAULT 0 CHECK(question_41 >= 0 AND question_41 <= 3),
            question_42 INTEGER DEFAULT 0 CHECK(question_42 >= 0 AND question_42 <= 3),
            question_43 INTEGER DEFAULT 0 CHECK(question_43 >= 0 AND question_43 <= 3),
            question_44 INTEGER DEFAULT 0 CHECK(question_44 >= 0 AND question_44 <= 3),
            question_45 INTEGER DEFAULT 0 CHECK(question_45 >= 0 AND question_45 <= 3),
            question_46 INTEGER DEFAULT 0 CHECK(question_46 >= 0 AND question_46 <= 3),
            question_47 INTEGER DEFAULT 0 CHECK(question_47 >= 0 AND question_47 <= 3),
            question_48 INTEGER DEFAULT 0 CHECK(question_48 >= 0 AND question_48 <= 3),
            question_49 INTEGER DEFAULT 0 CHECK(question_49 >= 0 AND question_49 <= 3),
            question_50 INTEGER DEFAULT 0 CHECK(question_50 >= 0 AND question_50 <= 3),
            question_51 INTEGER DEFAULT 0 CHECK(question_51 >= 0 AND question_51 <= 3),
            question_52 INTEGER DEFAULT 0 CHECK(question_52 >= 0 AND question_52 <= 3),
            question_53 INTEGER DEFAULT 0 CHECK(question_53 >= 0 AND question_53 <= 3),
            question_54 INTEGER DEFAULT 0 CHECK(question_54 >= 0 AND question_54 <= 3),
            question_55 INTEGER DEFAULT 0 CHECK(question_55 >= 0 AND question_55 <= 3),
            question_56 INTEGER DEFAULT 0 CHECK(question_56 >= 0 AND question_56 <= 3),
            question_57 INTEGER DEFAULT 0 CHECK(question_57 >= 0 AND question_57 <= 3),
            question_58 INTEGER DEFAULT 0 CHECK(question_58 >= 0 AND question_58 <= 3),
            question_59 INTEGER DEFAULT 0 CHECK(question_59 >= 0 AND question_59 <= 3),
            question_60 INTEGER DEFAULT 0 CHECK(question_60 >= 0 AND question_60 <= 3),
            question_61 INTEGER DEFAULT 0 CHECK(question_61 >= 0 AND question_61 <= 3),
            question_62 INTEGER DEFAULT 0 CHECK(question_62 >= 0 AND question_62 <= 3),
            question_63 INTEGER DEFAULT 0 CHECK(question_63 >= 0 AND question_63 <= 3),
            question_64 INTEGER DEFAULT 0 CHECK(question_64 >= 0 AND question_64 <= 3),
            question_65 INTEGER DEFAULT 0 CHECK(question_65 >= 0 AND question_65 <= 3),
            question_66 INTEGER DEFAULT 0 CHECK(question_66 >= 0 AND question_66 <= 3),
            question_67 INTEGER DEFAULT 0 CHECK(question_67 >= 0 AND question_67 <= 3),
            question_68 INTEGER DEFAULT 0 CHECK(question_68 >= 0 AND question_68 <= 3),
            question_69 INTEGER DEFAULT 0 CHECK(question_69 >= 0 AND question_69 <= 3),
            question_70 INTEGER DEFAULT 0 CHECK(question_70 >= 0 AND question_70 <= 3),
            question_71 INTEGER DEFAULT 0 CHECK(question_71 >= 0 AND question_71 <= 3),
            question_72 INTEGER DEFAULT 0 CHECK(question_72 >= 0 AND question_72 <= 3),
            question_73 INTEGER DEFAULT 0 CHECK(question_73 >= 0 AND question_73 <= 3),
            question_74 INTEGER DEFAULT 0 CHECK(question_74 >= 0 AND question_74 <= 3),
            question_75 INTEGER DEFAULT 0 CHECK(question_75 >= 0 AND question_75 <= 3),
            question_76 INTEGER DEFAULT 0 CHECK(question_76 >= 0 AND question_76 <= 3),
            question_77 INTEGER DEFAULT 0 CHECK(question_77 >= 0 AND question_77 <= 3),
            question_78 INTEGER DEFAULT 0 CHECK(question_78 >= 0 AND question_78 <= 3),
            question_79 INTEGER DEFAULT 0 CHECK(question_79 >= 0 AND question_79 <= 3),
            question_80 INTEGER DEFAULT 0 CHECK(question_80 >= 0 AND question_80 <= 3),
            question_81 INTEGER DEFAULT 0 CHECK(question_81 >= 0 AND question_81 <= 3),
            question_82 INTEGER DEFAULT 0 CHECK(question_82 >= 0 AND question_82 <= 3),
            question_83 INTEGER DEFAULT 0 CHECK(question_83 >= 0 AND question_83 <= 3),
            question_84 INTEGER DEFAULT 0 CHECK(question_84 >= 0 AND question_84 <= 3),
            question_85 INTEGER DEFAULT 0 CHECK(question_85 >= 0 AND question_85 <= 3),
            question_86 INTEGER DEFAULT 0 CHECK(question_86 >= 0 AND question_86 <= 3),
            question_87 INTEGER DEFAULT 0 CHECK(question_87 >= 0 AND question_87 <= 3),
            question_88 INTEGER DEFAULT 0 CHECK(question_88 >= 0 AND question_88 <= 3),
            question_89 INTEGER DEFAULT 0 CHECK(question_89 >= 0 AND question_89 <= 3),
            question_90 INTEGER DEFAULT 0 CHECK(question_90 >= 0 AND question_90 <= 3),
            scl_createdAt TEXT NOT NULL,
            scl_updatedAt TEXT NOT NULL,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
    
    if (!executeSQLCommand(db, createSCL90RTable, "SCL90R table creation")) {
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
    
    // CRITICAL: Ensure we're running from the correct directory
    if (!ensureCorrectWorkingDirectory()) {
        cerr << "❌ FATAL ERROR: Application must be run from project root directory!" << endl;
        cerr << "   Please navigate to the project root and try again." << endl;
        return 1;
    }
    
    try {
        // Ensure data directories exist
        DatabaseConfig::ensureDirectoriesExist();
        
        // Setup and open SQLite database
        sqlite3* db;
        int result = sqlite3_open(DatabaseConfig::MAIN_DATABASE_PATH.c_str(), &db);
        
        if (result != SQLITE_OK) {
            cerr << "❌ Error opening database: " << sqlite3_errmsg(db) << endl;
            return 1;
        }
        
        logMessage("INFO", "Database opened successfully: " + DatabaseConfig::MAIN_DATABASE_PATH);
        
        // Validate database integrity
        if (!validateDatabaseIntegrity(db)) {
            cerr << "❌ Database integrity validation failed!" << endl;
            sqlite3_close(db);
            return 1;
        }
        
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
        
        // Count beck anxiety inventories
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM beck_anxiety_inventory", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "BAI Forms in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count pain body maps
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM pain_body_map", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "PBM Forms in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Count activities of daily living forms
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM activities_of_daily_living", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "ADL Forms in database: " << count << endl;
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
