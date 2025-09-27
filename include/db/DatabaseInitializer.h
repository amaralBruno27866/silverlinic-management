#ifndef DB_DATABASE_INITIALIZER_H
#define DB_DATABASE_INITIALIZER_H

#include <sqlite3.h>
#include <string>

namespace SilverClinic {
namespace db {

/**
 * @brief Centralized database initialization and management
 * 
 * This class orchestrates the complete database setup process,
 * including table creation, indexing, and sample data insertion.
 */
class DatabaseInitializer {
public:
    /**
     * @brief Initialize the database with complete schema
     * 
     * This method:
     * 1. Validates database integrity
     * 2. Applies standard PRAGMAs
     * 3. Creates all tables in correct order
     * 4. Creates indexes
     * 5. Inserts sample data (if needed)
     * 
     * @param db Open SQLite database connection
     * @return true if initialization succeeded, false otherwise
     */
    static bool initialize(sqlite3* db);
    
    /**
     * @brief Initialize database for testing
     * 
     * Similar to initialize() but:
     * - Uses in-memory database or test paths
     * - Skips sample data insertion
     * - Optimized for test performance
     * 
     * @param db Open SQLite database connection
     * @return true if test initialization succeeded, false otherwise
     */
    static bool initializeForTesting(sqlite3* db);
    
    /**
     * @brief Create all database tables
     * 
     * Creates all tables defined in DatabaseSchema in the correct order
     * to respect foreign key constraints.
     * 
     * @param db Open SQLite database connection
     * @return true if all tables created successfully, false otherwise
     */
    static bool createAllTables(sqlite3* db);
    
    /**
     * @brief Create all database indexes
     * 
     * Creates all indexes defined in DatabaseSchema for optimal performance.
     * 
     * @param db Open SQLite database connection
     * @return true if all indexes created successfully, false otherwise
     */
    static bool createAllIndexes(sqlite3* db);
    
    /**
     * @brief Insert sample data for development/demo
     * 
     * Inserts sample assessor, client, addresses, and case profile
     * for development and demonstration purposes.
     * 
     * @param db Open SQLite database connection
     * @return true if sample data inserted successfully, false otherwise
     */
    static bool insertSampleData(sqlite3* db);
    
    /**
     * @brief Validate database integrity
     * 
     * Performs basic connectivity and schema validation checks.
     * 
     * @param db Open SQLite database connection
     * @return true if database is valid, false otherwise
     */
    static bool validateDatabaseIntegrity(sqlite3* db);
    
    /**
     * @brief Update schema version tracking
     * 
     * Records the current schema version in the database for
     * future migration support.
     * 
     * @param db Open SQLite database connection
     * @return true if version updated successfully, false otherwise
     */
    static bool updateSchemaVersion(sqlite3* db);
    
    /**
     * @brief Get database statistics
     * 
     * Retrieves count of records in all major tables for
     * debugging and monitoring purposes.
     * 
     * @param db Open SQLite database connection
     * @return String containing formatted database statistics
     */
    static std::string getDatabaseStatistics(sqlite3* db);

private:
    /**
     * @brief Execute SQL command with error handling
     * 
     * Helper method to execute SQL commands with consistent
     * error handling and logging.
     * 
     * @param db Open SQLite database connection
     * @param sql SQL command to execute
     * @param description Human-readable description for logging
     * @return true if command executed successfully, false otherwise
     */
    static bool executeSQLCommand(sqlite3* db, const std::string& sql, const std::string& description);
    
    DatabaseInitializer() = delete; // Static-only class
};

} // namespace db
} // namespace SilverClinic

#endif // DB_DATABASE_INITIALIZER_H