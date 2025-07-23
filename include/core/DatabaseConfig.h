#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H

#include <string>

namespace SilverClinic {
    
    /**
     * @brief Configuration class for database paths and settings
     * 
     * This class centralizes all database-related configuration to ensure
     * consistent database file locations across the application.
     */
    class DatabaseConfig {
    public:
        // Main application database
        static const std::string MAIN_DATABASE_PATH;
        
        // Test databases directory
        static const std::string TEST_DATABASE_DIR;
        
        // Specific test database paths
        static const std::string ASSESSOR_MANAGER_TEST_DB;
        static const std::string CLIENT_MANAGER_TEST_DB;
        static const std::string INTEGRATION_TEST_DB;
        
        // Demo database path
        static const std::string DEMO_DATABASE_PATH;
        
        /**
         * @brief Get the full path for a test database
         * @param testName The name of the test (without .db extension)
         * @return Full path to the test database
         */
        static std::string getTestDatabasePath(const std::string& testName);
        
        /**
         * @brief Ensure data directories exist
         * @return true if directories were created successfully, false otherwise
         */
        static bool ensureDirectoriesExist();
        
        /**
         * @brief Clean up all test databases
         * @return true if cleanup was successful, false otherwise
         */
        static bool cleanupTestDatabases();
        
    private:
        DatabaseConfig() = delete; // Static-only class
    };
    
} // namespace SilverClinic

#endif // DATABASE_CONFIG_H
