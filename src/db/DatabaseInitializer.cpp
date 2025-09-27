#include "db/DatabaseInitializer.h"
#include "db/DatabaseSchema.h"
#include "core/DatabaseConfig.h"
#include "core/Utils.h"
#include "utils/StructuredLogger.h"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>
#include <sstream>

using namespace std;
using namespace SilverClinic;
using namespace utils;

namespace SilverClinic {
namespace db {

bool DatabaseInitializer::initialize(sqlite3* db) {
    utils::logStructured(utils::LogLevel::INFO, {"DB","init_start","DatabaseInitializer", "", {}}, "Starting complete database initialization");
    
    // Step 1: Validate database integrity
    if (!validateDatabaseIntegrity(db)) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","init_fail","DatabaseInitializer", "", {}}, "Database integrity validation failed");
        return false;
    }
    
    // Step 2: Apply standard PRAGMAs
    if (!DatabaseConfig::applyStandardPragmas(db)) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","init_fail","DatabaseInitializer", "", {}}, "Failed to apply standard PRAGMAs");
        return false;
    }
    
    // Step 3: Create all tables
    if (!createAllTables(db)) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","init_fail","DatabaseInitializer", "", {}}, "Failed to create database tables");
        return false;
    }
    
    // Step 4: Create all indexes
    if (!createAllIndexes(db)) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","init_fail","DatabaseInitializer", "", {}}, "Failed to create database indexes");
        return false;
    }
    
    // Step 5: Update schema version
    if (!updateSchemaVersion(db)) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","init_fail","DatabaseInitializer", "", {}}, "Failed to update schema version");
        return false;
    }
    
    // Step 6: Insert sample data (only if tables are empty)
    if (!insertSampleData(db)) {
        utils::logStructured(utils::LogLevel::WARN, {"DB","init_warn","DatabaseInitializer", "", {}}, "Sample data insertion failed - continuing");
        // Non-critical error - continue
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"DB","init_success","DatabaseInitializer", "", {}}, "Database initialization completed successfully");
    return true;
}

bool DatabaseInitializer::initializeForTesting(sqlite3* db) {
    utils::logStructured(utils::LogLevel::DEBUG, {"DB","test_init_start","DatabaseInitializer", "", {}}, "Starting test database initialization");
    
    // Apply PRAGMAs
    if (!DatabaseConfig::applyStandardPragmas(db)) {
        return false;
    }
    
    // Create tables (skip sample data for tests)
    if (!createAllTables(db)) {
        return false;
    }
    
    // Create indexes  
    if (!createAllIndexes(db)) {
        return false;
    }
    
    utils::logStructured(utils::LogLevel::DEBUG, {"DB","test_init_success","DatabaseInitializer", "", {}}, "Test database initialization completed");
    return true;
}

bool DatabaseInitializer::createAllTables(sqlite3* db) {
    utils::logStructured(utils::LogLevel::INFO, {"DB","create_tables","DatabaseInitializer", "", {}}, "Creating all database tables");
    
    auto tableDefinitions = DatabaseSchema::getAllTableDefinitions();
    
    for (const auto& [name, sql] : tableDefinitions) {
        if (!executeSQLCommand(db, sql, name + " table creation")) {
            return false;
        }
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"DB","tables_created","DatabaseInitializer", "", {}}, "All database tables created successfully");
    return true;
}

bool DatabaseInitializer::createAllIndexes(sqlite3* db) {
    utils::logStructured(utils::LogLevel::INFO, {"DB","create_indexes","DatabaseInitializer", "", {}}, "Creating all database indexes");
    
    auto indexDefinitions = DatabaseSchema::getAllIndexDefinitions();
    
    for (const auto& [name, sql] : indexDefinitions) {
        if (!executeSQLCommand(db, sql, name)) {
            return false;
        }
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"DB","indexes_created","DatabaseInitializer", "", {}}, "All database indexes created successfully");
    return true;
}

bool DatabaseInitializer::insertSampleData(sqlite3* db) {
    utils::logStructured(utils::LogLevel::INFO, {"DB","sample_data","DatabaseInitializer", "", {}}, "Inserting sample data");
    
    string currentTime = getCurrentTimestamp();
    
    // Generate sequential IDs starting from 1
    int assessorId = 1;
    int clientId = 1;    
    int addressId1 = 1;
    int addressId2 = 2;
    int caseProfileId = 1;
    
    // Insert sample assessor
    string insertAssessor = R"(
        INSERT OR IGNORE INTO assessor (id, firstname, lastname, phone, email, normalized_email, created_at, modified_at)
        VALUES ()" + to_string(assessorId) + R"(, 'Dr. Maria', 'Silva', '416-555-0123', 'maria.silva@silverclinic.ca', 'maria.silva@silverclinic.ca', ')" + currentTime + "', '" + currentTime + R"(')
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
    string insertCaseProfile = R"(
        INSERT OR IGNORE INTO case_profile (id, client_id, assessor_id, status, notes, created_at, modified_at)
        VALUES ()" + to_string(caseProfileId) + ", " + to_string(clientId) + ", " + to_string(assessorId) + R"(, 'Pending', 'Initial case profile for mental health assessment', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertCaseProfile, "Sample case profile insertion")) {
        return false;
    }
    
    // Insert assessor address
    string insertAddressAssessor = R"(
        INSERT OR IGNORE INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES ()" + to_string(addressId1) + ", " + to_string(assessorId) + R"(, '456 Healthcare St', 'Toronto', 'ON', 'M5H 2N2', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAddressAssessor, "Sample assessor address insertion")) {
        return false;
    }
    
    // Insert client address
    string insertAddressClient = R"(
        INSERT OR IGNORE INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES ()" + to_string(addressId2) + ", " + to_string(clientId) + R"(, '123 Main Street', 'Toronto', 'ON', 'M5V 3A8', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAddressClient, "Sample client address insertion")) {
        return false;
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"DB","sample_data_success","DatabaseInitializer", "", {}}, "Sample data inserted successfully");
    return true;
}

bool DatabaseInitializer::validateDatabaseIntegrity(sqlite3* db) {
    string dbPath = DatabaseConfig::getMainDatabasePath();
    
    // Check if database file exists and is not empty
    if (filesystem::exists(dbPath) && filesystem::file_size(dbPath) == 0) {
        utils::logStructured(utils::LogLevel::WARN, {"DB","empty_db","DatabaseInitializer", dbPath, {}}, "Database file empty - will initialize tables");
        return true; // Empty DB is OK, we'll create tables
    }
    
    // Test basic database connectivity
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, "SELECT name FROM sqlite_master WHERE type='table' LIMIT 1;", 
                             nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","integrity_fail","DatabaseInitializer", "", {}}, string("Database integrity check failed: ") + errorMessage);
        sqlite3_free(errorMessage);
        return false;
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"DB","integrity_ok","DatabaseInitializer", dbPath, {}}, "Database integrity validated successfully");
    return true;
}

bool DatabaseInitializer::updateSchemaVersion(sqlite3* db) {
    int currentVersion = DatabaseSchema::getCurrentSchemaVersion();
    
    string insertVersion = R"(
        INSERT OR REPLACE INTO schema_version (id, version) VALUES (1, )" + to_string(currentVersion) + R"()
    )";
    
    if (!executeSQLCommand(db, insertVersion, "Schema version update")) {
        return false;
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"DB","version_updated","DatabaseInitializer", to_string(currentVersion), {}}, "Schema version updated");
    return true;
}

string DatabaseInitializer::getDatabaseStatistics(sqlite3* db) {
    ostringstream stats;
    sqlite3_stmt* stmt;
    
    struct TableCount {
        string name;
        string table;
    };
    
    vector<TableCount> tables = {
        {"Assessors", "assessor"},
        {"Clients", "client"}, 
        {"Addresses", "address"},
        {"Case Profiles", "case_profile"},
        {"AAI Forms", "automobile_anxiety_inventory"},
        {"BDI Forms", "beck_depression_inventory"},
        {"BAI Forms", "beck_anxiety_inventory"},
        {"PBM Forms", "pain_body_map"},
        {"ADL Forms", "activities_of_daily_living"},
        {"SCL90R Forms", "scl90r"},
        {"Form GUIDs", "form_guids"}
    };
    
    stats << "\n📊 Database Statistics\n";
    stats << "==================\n";
    
    for (const auto& table : tables) {
        string sql = "SELECT COUNT(*) FROM " + table.table;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                stats << table.name << " in database: " << count << "\n";
            }
            sqlite3_finalize(stmt);
        }
    }
    
    return stats.str();
}

bool DatabaseInitializer::executeSQLCommand(sqlite3* db, const string& sql, const string& description) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        utils::logStructured(utils::LogLevel::ERROR, {"DB","sql_error","DatabaseInitializer", "", {}}, description + " failed: " + errorMessage);
        sqlite3_free(errorMessage);
        return false;
    }
    
    utils::logStructured(utils::LogLevel::DEBUG, {"DB","sql_success","DatabaseInitializer", "", {}}, description + " executed successfully");
    return true;
}

} // namespace db
} // namespace SilverClinic