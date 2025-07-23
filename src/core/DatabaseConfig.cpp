#include "core/DatabaseConfig.h"
#include <filesystem>
#include <iostream>

namespace SilverClinic {
    
    // Define the static constants
    const std::string DatabaseConfig::MAIN_DATABASE_PATH = "data/clinic.db";
    const std::string DatabaseConfig::TEST_DATABASE_DIR = "data/test/";
    const std::string DatabaseConfig::ASSESSOR_MANAGER_TEST_DB = "data/test/test_assessor_manager.db";
    const std::string DatabaseConfig::CLIENT_MANAGER_TEST_DB = "data/test/test_client_manager.db";
    const std::string DatabaseConfig::INTEGRATION_TEST_DB = "data/test/test_integration.db";
    const std::string DatabaseConfig::DEMO_DATABASE_PATH = "data/demo.db";
    
    std::string DatabaseConfig::getTestDatabasePath(const std::string& testName) {
        return TEST_DATABASE_DIR + testName + ".db";
    }
    
    bool DatabaseConfig::ensureDirectoriesExist() {
        try {
            // Create data directory if it doesn't exist
            if (!std::filesystem::exists("data")) {
                std::filesystem::create_directory("data");
            }
            
            // Create test directory if it doesn't exist
            if (!std::filesystem::exists(TEST_DATABASE_DIR)) {
                std::filesystem::create_directory(TEST_DATABASE_DIR);
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error creating directories: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool DatabaseConfig::cleanupTestDatabases() {
        try {
            if (std::filesystem::exists(TEST_DATABASE_DIR)) {
                // Remove all .db files in test directory
                for (const auto& entry : std::filesystem::directory_iterator(TEST_DATABASE_DIR)) {
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
    
} // namespace SilverClinic
