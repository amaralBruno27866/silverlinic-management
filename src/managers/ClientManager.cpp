#include "managers/ClientManager.h"
#include "core/Utils.h"
#include "utils/CSVUtils.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace SilverClinic;

ClientManager::ClientManager(sqlite3* database) : m_db(database) {
    if (!m_db) {
        throw invalid_argument("Database connection cannot be null");
    }
}

// CRUD Operations Implementation

bool ClientManager::create(const Client& client) {
    if (!validateClient(client)) {
        utils::logMessage("ERROR", "ClientManager::create - Invalid client data");
        return false;
    }
    
    const string sql = R"(
        INSERT INTO client (id, firstname, lastname, phone, email, date_of_birth, created_at, modified_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare create statement");
        return false;
    }
    
    // Bind parameters
    sqlite3_bind_int(stmt, 1, client.getClientId());
    sqlite3_bind_text(stmt, 2, client.getFirstName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, client.getLastName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, client.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, client.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, client.getDateOfBirth().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, client.getCreatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, client.getUpdatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute create statement");
        return false;
    }
    
    utils::logMessage("INFO", "Client created successfully with ID: " + utils::toString(client.getClientId()));
    return true;
}

vector<Client> ClientManager::readAll() const {
    vector<Client> clients;
    
    const string sql = R"(
        SELECT c.id, c.firstname, c.lastname, c.phone, c.email, c.date_of_birth, c.created_at, c.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM client c
        LEFT JOIN address addr ON c.id = addr.user_key
        ORDER BY c.lastname, c.firstname
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readAll statement");
        return clients;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        clients.push_back(createClientFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return clients;
}

optional<Client> ClientManager::readById(int clientId) const {
    const string sql = R"(
        SELECT c.id, c.firstname, c.lastname, c.phone, c.email, c.date_of_birth, c.created_at, c.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM client c
        LEFT JOIN address addr ON c.id = addr.user_key
        WHERE c.id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readById statement");
        return nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    
    optional<Client> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = createClientFromRow(stmt);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

bool ClientManager::update(const Client& client) {
    if (!validateClient(client)) {
        utils::logMessage("ERROR", "ClientManager::update - Invalid client data");
        return false;
    }
    
    if (!exists(client.getClientId())) {
        utils::logMessage("ERROR", "ClientManager::update - Client not found with ID: " + utils::toString(client.getClientId()));
        return false;
    }
    
    const string sql = R"(
        UPDATE client 
        SET firstname = ?, lastname = ?, phone = ?, email = ?, date_of_birth = ?, modified_at = ?
        WHERE id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare update statement");
        return false;
    }
    
    // Bind parameters
    sqlite3_bind_text(stmt, 1, client.getFirstName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, client.getLastName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, client.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, client.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, client.getDateOfBirth().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, DateTime::now().toString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, client.getClientId());
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute update statement");
        return false;
    }
    
    utils::logMessage("INFO", "Client updated successfully with ID: " + utils::toString(client.getClientId()));
    return true;
}

bool ClientManager::deleteById(int clientId) {
    if (!exists(clientId)) {
        utils::logMessage("ERROR", "ClientManager::deleteById - Client not found with ID: " + utils::toString(clientId));
        return false;
    }
    
    if (!canDelete(clientId)) {
        utils::logMessage("ERROR", "ClientManager::deleteById - Cannot delete client with associated cases. ID: " + utils::toString(clientId));
        return false;
    }
    
    const string sql = "DELETE FROM client WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare delete statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute delete statement");
        return false;
    }
    
    utils::logMessage("INFO", "Client deleted successfully with ID: " + utils::toString(clientId));
    return true;
}

// Utility Methods Implementation

bool ClientManager::exists(int clientId) const {
    const string sql = "SELECT COUNT(*) FROM client WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare exists statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    
    sqlite3_finalize(stmt);
    return exists;
}

int ClientManager::getCount() const {
    const string sql = "SELECT COUNT(*) FROM client";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare count statement");
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

vector<CaseProfile> ClientManager::getCasesByClientId(int clientId) const {
    vector<CaseProfile> cases;
    
    const string sql = R"(
        SELECT id, client_id, assessor_id, status, notes, created_at, closed_at, modified_at
        FROM case_profile
        WHERE client_id = ?
        ORDER BY created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare getCases statement");
        return cases;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CaseProfile caseProfile;
        caseProfile.setCaseProfileId(sqlite3_column_int(stmt, 0));
        caseProfile.setClientId(sqlite3_column_int(stmt, 1));
        caseProfile.setAssessorId(sqlite3_column_int(stmt, 2));
        
        const char* status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        if (status) caseProfile.setStatus(status);
        
        const char* notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        if (notes) caseProfile.setNotes(notes);
        
        const char* createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        if (createdAt) caseProfile.setCreatedAt(DateTime::fromString(createdAt));
        
        const char* closedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        if (closedAt) caseProfile.setClosedAt(DateTime::fromString(closedAt));
        
        const char* modifiedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        if (modifiedAt) caseProfile.setUpdatedAt(DateTime::fromString(modifiedAt));
        
        cases.push_back(caseProfile);
    }
    
    sqlite3_finalize(stmt);
    return cases;
}

vector<Client> ClientManager::searchByName(const string& searchTerm) const {
    vector<Client> clients;
    
    const string sql = R"(
        SELECT c.id, c.firstname, c.lastname, c.phone, c.email, c.date_of_birth, c.created_at, c.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM client c
        LEFT JOIN address addr ON c.id = addr.user_key
        WHERE c.firstname LIKE ? OR c.lastname LIKE ?
        ORDER BY c.lastname, c.firstname
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare searchByName statement");
        return clients;
    }
    
    string searchPattern = "%" + searchTerm + "%";
    sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, searchPattern.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        clients.push_back(createClientFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return clients;
}

optional<Client> ClientManager::findByEmail(const string& email) const {
    const string sql = R"(
        SELECT c.id, c.firstname, c.lastname, c.phone, c.email, c.date_of_birth, c.created_at, c.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM client c
        LEFT JOIN address addr ON c.id = addr.user_key
        WHERE c.email = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare findByEmail statement");
        return nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    
    optional<Client> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = createClientFromRow(stmt);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

vector<Client> ClientManager::findByDateOfBirth(const string& dateOfBirth) const {
    vector<Client> clients;
    
    const string sql = R"(
        SELECT c.id, c.firstname, c.lastname, c.phone, c.email, c.date_of_birth, c.created_at, c.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM client c
        LEFT JOIN address addr ON c.id = addr.user_key
        WHERE c.date_of_birth = ?
        ORDER BY c.lastname, c.firstname
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare findByDateOfBirth statement");
        return clients;
    }
    
    sqlite3_bind_text(stmt, 1, dateOfBirth.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        clients.push_back(createClientFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return clients;
}

vector<Client> ClientManager::getClientsByAgeRange(int minAge, int maxAge) const {
    vector<Client> clients;
    vector<Client> allClients = readAll();
    
    for (const auto& client : allClients) {
        int age = client.getAge();
        if (age >= minAge && age <= maxAge) {
            clients.push_back(client);
        }
    }
    
    return clients;
}

vector<Client> ClientManager::readWithPagination(int limit, int offset) const {
    vector<Client> clients;
    
    const string sql = R"(
        SELECT c.id, c.firstname, c.lastname, c.phone, c.email, c.date_of_birth, c.created_at, c.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM client c
        LEFT JOIN address addr ON c.id = addr.user_key
        ORDER BY c.lastname, c.firstname
        LIMIT ? OFFSET ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readWithPagination statement");
        return clients;
    }
    
    sqlite3_bind_int(stmt, 1, limit);
    sqlite3_bind_int(stmt, 2, offset);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        clients.push_back(createClientFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return clients;
}

// Validation Methods

bool ClientManager::validateClient(const Client& client) const {
    // Check required fields
    if (client.getFirstName().empty()) {
        utils::logMessage("ERROR", "Validation failed: First name is required");
        return false;
    }
    
    if (client.getLastName().empty()) {
        utils::logMessage("ERROR", "Validation failed: Last name is required");
        return false;
    }
    
    // Use Utils functions for validation
    if (!utils::isValidEmail(client.getEmail())) {
        utils::logMessage("ERROR", "Validation failed: Invalid email format: " + client.getEmail());
        return false;
    }
    
    if (!utils::isValidCanadianPhoneNumber(client.getPhone())) {
        utils::logMessage("ERROR", "Validation failed: Invalid Canadian phone format: " + client.getPhone());
        return false;
    }
    
    // Check ID range
    if (client.getClientId() < Client::ID_PREFIX) {
        utils::logMessage("ERROR", "Validation failed: Invalid client ID range: " + utils::toString(client.getClientId()));
        return false;
    }
    
    // Check names are not just numeric or contain invalid characters
    if (utils::isNumeric(client.getFirstName()) || utils::isNumeric(client.getLastName())) {
        utils::logMessage("ERROR", "Validation failed: Names cannot be purely numeric");
        return false;
    }
    
    // Validate date of birth if provided
    if (!client.getDateOfBirth().empty()) {
        // Basic date format validation (you might want to enhance this)
        if (client.getDateOfBirth().length() < 8) {
            utils::logMessage("ERROR", "Validation failed: Invalid date of birth format");
            return false;
        }
    }
    
    return true;
}

bool ClientManager::canDelete(int clientId) const {
    const string sql = "SELECT COUNT(*) FROM case_profile WHERE client_id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare canDelete statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    
    bool canDelete = true;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        canDelete = (count == 0);
    }
    
    sqlite3_finalize(stmt);
    return canDelete;
}

// Helper Methods

Client ClientManager::createClientFromRow(sqlite3_stmt* stmt) const {
    // Extract client data with NULL safety
    int id = sqlite3_column_int(stmt, 0);
    
    const char* firstNamePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    string firstName = firstNamePtr ? utils::normalizeName(firstNamePtr) : "";
    
    const char* lastNamePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    string lastName = lastNamePtr ? utils::normalizeName(lastNamePtr) : "";
    
    const char* phonePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    string phone = phonePtr ? utils::normalizePhoneNumber(phonePtr) : "";
    
    const char* emailPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    string email = emailPtr ? utils::normalizeForDatabase(emailPtr) : "";
    
    const char* dateOfBirthPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    string dateOfBirth = dateOfBirthPtr ? dateOfBirthPtr : "";
    
    const char* createdAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    DateTime createdAt = createdAtPtr ? DateTime::fromString(createdAtPtr) : DateTime();
    
    const char* modifiedAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    DateTime modifiedAt = modifiedAtPtr ? DateTime::fromString(modifiedAtPtr) : DateTime();
    
    // Extract address data (if exists)
    Address address;
    if (sqlite3_column_type(stmt, 8) != SQLITE_NULL) {
        address = createAddressFromRow(stmt, 8);
    }
    
    return Client(id, firstName, lastName, email, phone, dateOfBirth, address, createdAt, modifiedAt);
}

Address ClientManager::createAddressFromRow(sqlite3_stmt* stmt, int startColumn) const {
    Address address;
    
    if (sqlite3_column_type(stmt, startColumn) != SQLITE_NULL) {
        int id = sqlite3_column_int(stmt, startColumn);
        
        const char* streetPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, startColumn + 1));
        string street = streetPtr ? streetPtr : "";
        
        const char* cityPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, startColumn + 2));
        string city = cityPtr ? cityPtr : "";
        
        const char* provincePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, startColumn + 3));
        string province = provincePtr ? provincePtr : "";
        
        const char* postalCodePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, startColumn + 4));
        string postalCode = postalCodePtr ? postalCodePtr : "";
        
        const char* createdAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, startColumn + 5));
        DateTime createdAt = createdAtPtr ? DateTime::fromString(createdAtPtr) : DateTime();
        
        const char* modifiedAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, startColumn + 6));
        DateTime modifiedAt = modifiedAtPtr ? DateTime::fromString(modifiedAtPtr) : DateTime();
        
        address = Address(id, 0, street, city, province, postalCode, createdAt, modifiedAt);
    }
    
    return address;
}

void ClientManager::logDatabaseError(const string& operation) const {
    string errorMsg = "Database error in " + operation + ": " + sqlite3_errmsg(m_db);
    utils::logMessage("ERROR", errorMsg);
}

int ClientManager::importFromCSV(const string& filePath) {
    int success = 0;
    int failed  = 0;
    bool inTransaction = false;
    try {
        auto table = csv::CSVReader::readFile(filePath);
        const vector<string> required = {"firstname","lastname","phone","email","date_of_birth","created_at"};
        for (const auto &h : required) {
            if (find(table.headers.begin(), table.headers.end(), h) == table.headers.end()) {
                utils::logMessage("ERROR", "ClientManager::importFromCSV - Missing required header: " + h);
                return 0; // structural error
            }
        }
        if (sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) == SQLITE_OK) {
            inTransaction = true;
        } else {
            utils::logMessage("ERROR", "ClientManager::importFromCSV - Failed to BEGIN TRANSACTION (continuing non-atomic)");
        }
        for (const auto &row : table.rows) {
            try {
                string firstName = utils::normalizeName(csv::safeGet(row, "firstname"));
                string lastName  = utils::normalizeName(csv::safeGet(row, "lastname"));
                string phone     = utils::normalizePhoneNumber(csv::safeGet(row, "phone"));
                string email     = utils::normalizeForDatabase(csv::safeGet(row, "email"));
                string dob       = csv::safeGet(row, "date_of_birth");
                string createdAtRaw = csv::safeGet(row, "created_at");
                string createdAtNorm = csv::normalizeTimestampForDateTime(createdAtRaw);
                DateTime createdAt = createdAtRaw.empty() ? DateTime::now() : DateTime::fromString(createdAtNorm);
                DateTime modifiedAt = createdAt;

                Address address;
                string street = csv::safeGet(row, "street");
                if (!street.empty()) {
                    address.setStreet(street);
                    address.setCity(csv::safeGet(row, "city"));
                    address.setProvince(csv::safeGet(row, "province"));
                    address.setPostalCode(csv::safeGet(row, "postal_code"));
                    string addrCreatedRaw = csv::safeGet(row, "address_created_at");
                    string addrCreatedNorm = csv::normalizeTimestampForDateTime(addrCreatedRaw);
                    DateTime addrCreated = addrCreatedRaw.empty() ? createdAt : DateTime::fromString(addrCreatedNorm);
                    address.setCreatedAt(addrCreated);
                    address.setUpdatedAt(addrCreated);
                }

                int id = Client::getNextClientId();
                Client client(id, firstName, lastName, email, phone, dob, address, createdAt, modifiedAt);
                if (!create(client)) {
                    failed++;
                    utils::logMessage("ERROR", "Failed to insert client from CSV row (email: " + email + ")");
                    continue;
                }
                success++;
            } catch (const exception &e) {
                failed++;
                utils::logMessage("ERROR", string("ClientManager::importFromCSV row error: ")+ e.what());
            }
        }
        if (inTransaction) {
            if (sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
                utils::logMessage("ERROR", "ClientManager::importFromCSV - COMMIT failed, attempting ROLLBACK");
                sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
            }
        }
    } catch (const exception &e) {
        if (inTransaction) {
            sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        }
        utils::logMessage("ERROR", string("ClientManager::importFromCSV file error: ")+ e.what());
    }
    utils::logMessage("INFO", "ClientManager::importFromCSV imported success=" + to_string(success) + ", failed=" + to_string(failed));
    return success;
}
