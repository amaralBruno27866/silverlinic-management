// ========================================
// Silver Clinic Management System - Main Application
// ========================================

// Core System Headers
#include "core/DatabaseConfig.h"
#include "core/Utils.h"
#include "core/DateTime.h"

// Database Management Headers
#include "db/DatabaseInitializer.h"

// Entity Headers
#include "core/Address.h"
#include "core/Assessor.h"
#include "core/Client.h"
#include "core/CaseProfile.h"

// Form Headers
#include "forms/FamilyPhysician.h"
#include "forms/AutomobileAnxietyInventory.h"
#include "forms/BeckDepressionInventory.h"
#include "forms/BeckAnxietyInventory.h"
#include "forms/PainBodyMap.h"
#include "forms/ActivitiesOfDailyLiving.h"

// Standard Library Headers
#include <iostream>
#include <string>
#include <filesystem>

// Third-party Headers
#include <sqlite3.h>

// Platform-specific Headers
#ifndef _WIN32
#include <unistd.h>
#endif

using namespace std;
using namespace SilverClinic;
using namespace utils;

// Function to ensure we're running from the correct directory
bool ensureCorrectWorkingDirectory() {
    string currentDir = filesystem::current_path().string();
    
    #ifdef _WIN32
        // On Windows, get executable path differently
        string executablePath = currentDir; // Use current directory as fallback
    #else
        string executablePath = filesystem::canonical("/proc/self/exe").parent_path().string();
    #endif
    
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


// ========================================

// ========================================
// Main Application Entry Point
// ========================================
int main(int argc, char* argv[]) {
    // ========================================
    // Command Line Arguments Processing
    // ========================================
    
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            cout << "🏥 Silver Clinic Management System - Help" << endl;
            cout << "=========================================" << endl;
            cout << "Usage: " << argv[0] << " [options]" << endl;
            cout << "" << endl;
            cout << "Options:" << endl;
            cout << "  --help, -h     Show this help message" << endl;
            cout << "  --version, -v  Show version information" << endl;
            cout << "" << endl;
            cout << "Description:" << endl;
            cout << "  This system helps manage clinical assessments and client data." << endl;
            cout << "  It initializes the SQLite database and creates sample data if needed." << endl;
            cout << "" << endl;
            cout << "Files:" << endl;
            cout << "  Database: data/clinic.db" << endl;
            cout << "  Logs: Console output with structured logging" << endl;
            return 0;
        }
        if (arg == "--version" || arg == "-v") {
            cout << "Silver Clinic Management System v1.0.0" << endl;
            cout << "Built with C++17, SQLite3, and libharu" << endl;
            return 0;
        }
    }
    
    // ========================================
    // Application Banner
    // ========================================
    
    cout << "🏥 Silver Clinic Management System" << endl;
    cout << "===================================" << endl;
    cout << "Welcome to the Silver Clinic Management System!" << endl;
    cout << "This system helps manage clinical assessments and client data." << endl;
    
    // ========================================
    // Environment Validation
    // ========================================
    
    // CRITICAL: Ensure we're running from the correct directory
    if (!ensureCorrectWorkingDirectory()) {
        cerr << "❌ FATAL ERROR: Application must be run from project root directory!" << endl;
        cerr << "   Please navigate to the project root and try again." << endl;
        utils::logStructured(utils::LogLevel::ERROR, {"APP","init_fail","Environment", "", {}}, "Failed to locate project root directory");
        return 1;
    }
    
    try {
        // ========================================
        // Database Initialization
        // ========================================
        
        // Ensure data directories exist
        DatabaseConfig::ensureDirectoriesExist();
        
        // Setup and open SQLite database (compute path at runtime)
        sqlite3* db;
        std::string dbPath = DatabaseConfig::getMainDatabasePath();
        int result = sqlite3_open(dbPath.c_str(), &db);
        
        if (result != SQLITE_OK) {
            cerr << "❌ Error opening database: " << sqlite3_errmsg(db) << endl;
            return 1;
        }
        
    utils::logStructured(utils::LogLevel::INFO, {"APP","db_open","Database", dbPath, {}}, "Database opened successfully");

        // Initialize complete database using centralized architecture
        if (!SilverClinic::db::DatabaseInitializer::initialize(db)) {
            cerr << "❌ Database initialization failed!" << endl;
            sqlite3_close(db);
            return 1;
        }
        
        // Display database information
        cout << "\n📊 Database Status" << endl;
        cout << "==================" << endl;
        
        // Display database statistics using centralized reporting
        SilverClinic::db::DatabaseInitializer::getDatabaseStatistics(db);
        
        cout << "\n✅ System initialized successfully!" << endl;
        cout << "Database file: clinic.db" << endl;
        cout << "To run tests, use: ./run_tests.sh" << endl;
        cout << "===================================" << endl;
        
        // Close database
        sqlite3_close(db);
    utils::logStructured(utils::LogLevel::INFO, {"APP","db_close","Database", DatabaseConfig::MAIN_DATABASE_PATH, {}}, "Database connection closed");
        
    } catch (const exception& e) {
        cerr << "Error occurred: " << e.what() << endl;
    utils::logStructured(utils::LogLevel::ERROR, {"APP","init_fail","App", "", {}}, string("Application failed to initialize: ")+e.what());
        return 1;
    }
    
    return 0;
}
