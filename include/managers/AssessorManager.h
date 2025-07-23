#ifndef ASSESSOR_MANAGER_H
#define ASSESSOR_MANAGER_H

#include <vector>
#include <optional>
#include <memory>
#include <sqlite3.h>
#include "core/Assessor.h"
#include "core/CaseProfile.h"

using namespace std;
using namespace SilverClinic;

namespace SilverClinic {
    
    /**
     * @brief Manager class for Assessor CRUD operations
     * 
     * This class implements the Repository pattern for managing Assessor data
     * in the SQLite database. It provides comprehensive CRUD operations with
     * proper error handling and transaction management.
     */
    class AssessorManager {
    private:
        sqlite3* m_db;
        
        // Helper methods for database operations
        bool executeStatement(const string& sql, const string& operation) const;
        Assessor createAssessorFromRow(sqlite3_stmt* stmt) const;
        Address createAddressFromRow(sqlite3_stmt* stmt, int startColumn = 0) const;
        
    public:
        // Constructor and Destructor
        explicit AssessorManager(sqlite3* database);
        ~AssessorManager() = default;
        
        // Disable copy constructor and assignment operator
        AssessorManager(const AssessorManager&) = delete;
        AssessorManager& operator=(const AssessorManager&) = delete;
        
        // CRUD Operations
        
        /**
         * @brief Create a new assessor in the database
         * @param assessor The assessor object to create
         * @return true if creation was successful, false otherwise
         */
        bool create(const Assessor& assessor);
        
        /**
         * @brief Retrieve all assessors from the database
         * @return Vector of all assessors
         */
        vector<Assessor> readAll() const;
        
        /**
         * @brief Retrieve a specific assessor by ID
         * @param assessorId The ID of the assessor to retrieve
         * @return Optional containing the assessor if found, empty otherwise
         */
        optional<Assessor> readById(int assessorId) const;
        
        /**
         * @brief Update an existing assessor in the database
         * @param assessor The assessor object with updated data
         * @return true if update was successful, false otherwise
         */
        bool update(const Assessor& assessor);
        
        /**
         * @brief Delete an assessor from the database
         * @param assessorId The ID of the assessor to delete
         * @return true if deletion was successful, false otherwise
         */
        bool deleteById(int assessorId);
        
        // Additional utility methods
        
        /**
         * @brief Check if an assessor exists in the database
         * @param assessorId The ID to check
         * @return true if assessor exists, false otherwise
         */
        bool exists(int assessorId) const;
        
        /**
         * @brief Get the total count of assessors
         * @return Number of assessors in the database
         */
        int getCount() const;
        
        /**
         * @brief Get all cases associated with a specific assessor
         * @param assessorId The ID of the assessor
         * @return Vector of case profiles associated with the assessor
         */
        vector<CaseProfile> getCasesByAssessorId(int assessorId) const;
        
        /**
         * @brief Search assessors by name (first or last name)
         * @param searchTerm The term to search for
         * @return Vector of matching assessors
         */
        vector<Assessor> searchByName(const string& searchTerm) const;
        
        /**
         * @brief Search assessors by email
         * @param email The email to search for
         * @return Optional containing the assessor if found, empty otherwise
         */
        optional<Assessor> findByEmail(const string& email) const;
        
        /**
         * @brief Get assessors with pagination support
         * @param limit Maximum number of assessors to return
         * @param offset Number of assessors to skip
         * @return Vector of assessors for the requested page
         */
        vector<Assessor> readWithPagination(int limit, int offset = 0) const;
        
        // Validation methods
        
        /**
         * @brief Validate assessor data before database operations
         * @param assessor The assessor to validate
         * @return true if valid, false otherwise
         */
        bool validateAssessor(const Assessor& assessor) const;
        
        /**
         * @brief Check if deletion is safe (no associated cases)
         * @param assessorId The ID of the assessor to check
         * @return true if safe to delete, false if has associated cases
         */
        bool canDelete(int assessorId) const;
        
    private:
        // Internal helper methods
        bool bindAssessorToStatement(sqlite3_stmt* stmt, const Assessor& assessor, bool includeId = false) const;
        void logDatabaseError(const string& operation) const;
    };
    
} // namespace SilverClinic

#endif // ASSESSOR_MANAGER_H
