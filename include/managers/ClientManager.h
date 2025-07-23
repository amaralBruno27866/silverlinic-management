#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <vector>
#include <optional>
#include <memory>
#include <sqlite3.h>
#include "core/Client.h"
#include "core/CaseProfile.h"

using namespace std;
using namespace SilverClinic;

namespace SilverClinic {
    
    /**
     * @brief Manager class for Client CRUD operations
     * 
     * This class implements the Repository pattern for managing Client data
     * in the SQLite database. It provides comprehensive CRUD operations with
     * proper error handling and transaction management.
     */
    class ClientManager {
    private:
        sqlite3* m_db;
        
        // Helper methods for database operations
        bool executeStatement(const string& sql, const string& operation) const;
        Client createClientFromRow(sqlite3_stmt* stmt) const;
        Address createAddressFromRow(sqlite3_stmt* stmt, int startColumn = 0) const;
        
    public:
        // Constructor and Destructor
        explicit ClientManager(sqlite3* database);
        ~ClientManager() = default;
        
        // Disable copy constructor and assignment operator
        ClientManager(const ClientManager&) = delete;
        ClientManager& operator=(const ClientManager&) = delete;
        
        // CRUD Operations
        
        /**
         * @brief Create a new client in the database
         * @param client The client object to create
         * @return true if creation was successful, false otherwise
         */
        bool create(const Client& client);
        
        /**
         * @brief Retrieve all clients from the database
         * @return Vector of all clients
         */
        vector<Client> readAll() const;
        
        /**
         * @brief Retrieve a specific client by ID
         * @param clientId The ID of the client to retrieve
         * @return Optional containing the client if found, empty otherwise
         */
        optional<Client> readById(int clientId) const;
        
        /**
         * @brief Update an existing client in the database
         * @param client The client object with updated data
         * @return true if update was successful, false otherwise
         */
        bool update(const Client& client);
        
        /**
         * @brief Delete a client from the database
         * @param clientId The ID of the client to delete
         * @return true if deletion was successful, false otherwise
         */
        bool deleteById(int clientId);
        
        // Additional utility methods
        
        /**
         * @brief Check if a client exists in the database
         * @param clientId The ID to check
         * @return true if client exists, false otherwise
         */
        bool exists(int clientId) const;
        
        /**
         * @brief Get the total count of clients
         * @return Number of clients in the database
         */
        int getCount() const;
        
        /**
         * @brief Get all cases associated with a specific client
         * @param clientId The ID of the client
         * @return Vector of case profiles associated with the client
         */
        vector<CaseProfile> getCasesByClientId(int clientId) const;
        
        /**
         * @brief Search clients by name (first or last name)
         * @param searchTerm The term to search for
         * @return Vector of matching clients
         */
        vector<Client> searchByName(const string& searchTerm) const;
        
        /**
         * @brief Search clients by email
         * @param email The email to search for
         * @return Optional containing the client if found, empty otherwise
         */
        optional<Client> findByEmail(const string& email) const;
        
        /**
         * @brief Search clients by date of birth
         * @param dateOfBirth The date of birth to search for
         * @return Vector of clients with matching date of birth
         */
        vector<Client> findByDateOfBirth(const string& dateOfBirth) const;
        
        /**
         * @brief Get clients within a specific age range
         * @param minAge Minimum age
         * @param maxAge Maximum age
         * @return Vector of clients within the age range
         */
        vector<Client> getClientsByAgeRange(int minAge, int maxAge) const;
        
        /**
         * @brief Get clients with pagination support
         * @param limit Maximum number of clients to return
         * @param offset Number of clients to skip
         * @return Vector of clients for the requested page
         */
        vector<Client> readWithPagination(int limit, int offset = 0) const;
        
        // Validation methods
        
        /**
         * @brief Validate client data before database operations
         * @param client The client to validate
         * @return true if valid, false otherwise
         */
        bool validateClient(const Client& client) const;
        
        /**
         * @brief Check if deletion is safe (no associated cases)
         * @param clientId The ID of the client to check
         * @return true if safe to delete, false if has associated cases
         */
        bool canDelete(int clientId) const;
        
    private:
        // Internal helper methods
        bool bindClientToStatement(sqlite3_stmt* stmt, const Client& client, bool includeId = false) const;
        void logDatabaseError(const string& operation) const;
    };
    
} // namespace SilverClinic

#endif // CLIENT_MANAGER_H
