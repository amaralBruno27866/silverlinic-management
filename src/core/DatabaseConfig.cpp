#include "core/DatabaseConfig.h"
#include <filesystem>
#include <iostream>
#include <sqlite3.h>

#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>  // For SHGetFolderPath
#endif

namespace SilverClinic {
    
    // Helper function to get the project root directory
    std::string getProjectRootPath() {
        std::string currentPath = std::filesystem::current_path();
        
        // Convert backslashes to forward slashes on Windows
        #ifdef _WIN32
        std::replace(currentPath.begin(), currentPath.end(), '\\', '/');
        #endif
        
        // If we're in a build directory, go up one level
        if (currentPath.find("/build") != std::string::npos || 
            currentPath.find("/build_test") != std::string::npos ||
            currentPath.find("/Debug") != std::string::npos ||
            currentPath.find("/Release") != std::string::npos) {
            currentPath = std::filesystem::current_path().parent_path();
            #ifdef _WIN32
            std::replace(currentPath.begin(), currentPath.end(), '\\', '/');
            #endif
        }
        
        // Ensure we're in the project root by looking for CMakeLists.txt
        while (!std::filesystem::exists(currentPath + "/CMakeLists.txt") && 
               !std::filesystem::exists(currentPath + "/PROJECT_CONVENTIONS.md")) {
            std::filesystem::path parent = std::filesystem::path(currentPath).parent_path();
            if (parent == currentPath) break; // Reached filesystem root
            currentPath = parent;
            #ifdef _WIN32
            std::replace(currentPath.begin(), currentPath.end(), '\\', '/');
            #endif
        }
        
        return currentPath;
    }
    
    // Define the static constants with absolute paths
    const std::string DatabaseConfig::MAIN_DATABASE_PATH = getProjectRootPath() + "/data/clinic.db";
    const std::string DatabaseConfig::TEST_DATABASE_DIR = getProjectRootPath() + "/data/test/";
    const std::string DatabaseConfig::ASSESSOR_MANAGER_TEST_DB = getProjectRootPath() + "/data/test/test_assessor_manager.db";
    const std::string DatabaseConfig::CLIENT_MANAGER_TEST_DB = getProjectRootPath() + "/data/test/test_client_manager.db";
    const std::string DatabaseConfig::INTEGRATION_TEST_DB = getProjectRootPath() + "/data/test/test_integration.db";
    const std::string DatabaseConfig::DEMO_DATABASE_PATH = getProjectRootPath() + "/data/demo.db";

    std::string DatabaseConfig::getMainDatabasePath() {
        return getProjectRootPath() + "/data/clinic.db";
    }
    
    std::string DatabaseConfig::getTestDatabasePath(const std::string& testName) {
        return getProjectRootPath() + "/data/test/" + testName + ".db";
    }
    
    bool DatabaseConfig::ensureDirectoriesExist() {
        try {
            std::string projectRoot = getProjectRootPath();
            std::string dataDir = projectRoot + "/data";
            std::string testDir = projectRoot + "/data/test";
            
            // Create data directory if it doesn't exist
            if (!std::filesystem::exists(dataDir)) {
                std::filesystem::create_directories(dataDir);
                std::cout << "Created directory: " << dataDir << std::endl;
            }
            
            // Create test directory if it doesn't exist
            if (!std::filesystem::exists(testDir)) {
                std::filesystem::create_directories(testDir);
                std::cout << "Created directory: " << testDir << std::endl;
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error creating directories: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool DatabaseConfig::cleanupTestDatabases() {
        try {
            std::string testDir = getProjectRootPath() + "/data/test";
            if (std::filesystem::exists(testDir)) {
                // Remove all .db files in test directory
                for (const auto& entry : std::filesystem::directory_iterator(testDir)) {
                    if (entry.path().extension() == ".db") {
                        std::filesystem::remove(entry.path());
                    }
                }
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error cleaning up test databases: " << e.what() << std::endl;
            return false;
        }
    }

    bool DatabaseConfig::applyStandardPragmas(sqlite3* db) {
        if (!db) return false;
        // Order: foreign_keys first (no dependency), then WAL, then synchronous.
        char* errMsg = nullptr;
        auto exec = [&](const char* sql){
            if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
                std::cerr << "PRAGMA error (" << sql << "): " << (errMsg?errMsg:"?") << std::endl;
                if (errMsg) sqlite3_free(errMsg);
                return false;
            }
            return true;
        };
        if (!exec("PRAGMA foreign_keys=ON;")) return false;
        if (!exec("PRAGMA journal_mode=WAL;")) return false;
        if (!exec("PRAGMA synchronous=NORMAL;")) return false;
        // Verify foreign_keys actually ON (defensive)
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;
        if (sqlite3_prepare_v2(db, "PRAGMA foreign_keys;", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                ok = sqlite3_column_int(stmt, 0) == 1;
            }
        }
        if (stmt) sqlite3_finalize(stmt);
        return ok;
    }
    
} // namespace SilverClinic
