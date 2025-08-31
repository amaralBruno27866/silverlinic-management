#include "managers/AssessorManager.h"
#include "core/Utils.h"
#include "utils/CSVUtils.h"
#include "managers/AddressManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace SilverClinic;

AssessorManager::AssessorManager(sqlite3* database) : m_db(database) {
    if (!m_db) {
        throw invalid_argument("Database connection cannot be null");
    }
}

// Helper: find existing assessor by email or by firstname+lastname+phone
optional<int> AssessorManager::findExistingAssessorId(const Assessor& assessor) const {
    // Prefer email match if provided
    if (!assessor.getEmail().empty()) {
        const string sqlEmail = "SELECT id FROM assessor WHERE email = ? LIMIT 1";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sqlEmail.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, assessor.getEmail().c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int foundId = sqlite3_column_int(stmt, 0);
                sqlite3_finalize(stmt);
                return optional<int>(foundId);
            }
            sqlite3_finalize(stmt);
        } else {
            logDatabaseError("prepare find by email");
        }
    }

    // Fallback: match by firstname + lastname + phone (normalized)
    const string sqlNamePhone = R"(
        SELECT id FROM assessor WHERE firstname = ? AND lastname = ? AND phone = ? LIMIT 1
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sqlNamePhone.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, assessor.getFirstName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, assessor.getLastName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, assessor.getPhone().c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int foundId = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return optional<int>(foundId);
        }
        sqlite3_finalize(stmt);
    } else {
        logDatabaseError("prepare find by name+phone");
    }

    return nullopt;
}

// CRUD Operations Implementation

bool AssessorManager::create(const Assessor& assessor) {
    if (!validateAssessor(assessor)) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","validate_fail","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Invalid assessor data");
        return false;
    }

    // Duplicate check
    optional<int> existing = findExistingAssessorId(assessor);
    if (existing.has_value()) {
        // Log and return error - duplicate found
        string msg = "Duplicate assessor detected. Existing id=" + to_string(existing.value());
        utils::LogEventContext ctx;
        ctx.category = "MANAGER";
        ctx.action = "duplicate_detected";
        ctx.entityType = std::optional<std::string>("Assessor");
        ctx.entityId = std::optional<std::string>(std::to_string(assessor.getAssessorId()));
        ctx.correlationId = std::optional<std::string>(std::to_string(existing.value()));
        utils::logStructured(utils::LogLevel::ERROR, ctx, msg);
        return false;
    }

    const string sql = R"(
        INSERT INTO assessor (id, firstname, lastname, phone, email, created_at, modified_at)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare create statement");
        return false;
    }
    
    // Bind parameters
    sqlite3_bind_int(stmt, 1, assessor.getAssessorId());
    sqlite3_bind_text(stmt, 2, assessor.getFirstName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, assessor.getLastName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, assessor.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, assessor.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, assessor.getCreatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, assessor.getUpdatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute create statement");
        return false;
    }
    // If assessor has address data, persist it
    try {
        const Address &addr = assessor.getAddress();
        // If street (or other field) not empty, save address
        if (!addr.getStreet().empty()) {
            // Use AddressManager to insert address
            AddressManager addrMgr(m_db);
            Address toInsert = addr;
            toInsert.setUserKey(assessor.getAssessorId());
            if (!addrMgr.create(toInsert)) {
                utils::logStructured(utils::LogLevel::WARN, {"MANAGER","address_create_fail","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Failed to persist associated address");
            }
        }
    } catch (...) {
        // Non-fatal: address persistence should not break assessor creation
        utils::logStructured(utils::LogLevel::WARN, {"MANAGER","address_persist_exception","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Exception while persisting address");
    }

    utils::logStructured(utils::LogLevel::INFO, {"MANAGER","create","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Assessor created successfully");
    return true;
}

vector<Assessor> AssessorManager::readAll() const {
    vector<Assessor> assessors;
    
    const string sql = R"(
        SELECT a.id, a.firstname, a.lastname, a.phone, a.email, a.created_at, a.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM assessor a
        LEFT JOIN address addr ON a.id = addr.user_key
    ORDER BY a.id ASC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readAll statement");
        return assessors;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        assessors.push_back(createAssessorFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return assessors;
}

optional<Assessor> AssessorManager::readById(int assessorId) const {
    const string sql = R"(
        SELECT a.id, a.firstname, a.lastname, a.phone, a.email, a.created_at, a.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM assessor a
        LEFT JOIN address addr ON a.id = addr.user_key
        WHERE a.id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readById statement");
        return nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, assessorId);
    
    optional<Assessor> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = createAssessorFromRow(stmt);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

bool AssessorManager::update(const Assessor& assessor) {
    if (!validateAssessor(assessor)) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","validate_fail","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Invalid assessor data");
        return false;
    }
    if (!exists(assessor.getAssessorId())) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","not_found","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Assessor not found");
        return false;
    }
    
    const string sql = R"(
        UPDATE assessor 
        SET firstname = ?, lastname = ?, phone = ?, email = ?, modified_at = ?
        WHERE id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare update statement");
        return false;
    }
    
    // Bind parameters
    sqlite3_bind_text(stmt, 1, assessor.getFirstName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, assessor.getLastName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, assessor.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, assessor.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, DateTime::now().toString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, assessor.getAssessorId());
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute update statement");
        return false;
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"MANAGER","update","Assessor", to_string(assessor.getAssessorId()), {}}, "Assessor updated successfully");
    // Persist address changes if present
    try {
        const Address &addr = assessor.getAddress();
        if (!addr.getStreet().empty()) {
            AddressManager addrMgr(m_db);
            // If address has an id and exists, attempt update; otherwise create
            if (addr.getAddressId() >= Address::ID_PREFIX) {
                // try update, if fails, attempt create
                if (!addrMgr.update(addr)) {
                    Address toInsert = addr;
                    toInsert.setUserKey(assessor.getAssessorId());
                    if (!addrMgr.create(toInsert)) {
                        utils::logStructured(utils::LogLevel::WARN, {"MANAGER","address_update_fail","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Failed to update/create associated address");
                    }
                }
            } else {
                Address toInsert = addr;
                toInsert.setUserKey(assessor.getAssessorId());
                if (!addrMgr.create(toInsert)) {
                    utils::logStructured(utils::LogLevel::WARN, {"MANAGER","address_create_fail","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Failed to persist associated address");
                }
            }
        }
    } catch (...) {
        utils::logStructured(utils::LogLevel::WARN, {"MANAGER","address_persist_exception","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Exception while persisting address on update");
    }

    return true;
}

bool AssessorManager::deleteById(int assessorId) {
    if (!exists(assessorId)) {
    utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","delete_not_found","Assessor", to_string(assessorId), {}}, "Assessor not found");
        return false;
    }
    
    if (!canDelete(assessorId)) {
    utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","delete_conflict_cases","Assessor", to_string(assessorId), {}}, "Cannot delete assessor with associated cases");
        return false;
    }
    
    const string sql = "DELETE FROM assessor WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare delete statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, assessorId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute delete statement");
        return false;
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"MANAGER","delete","Assessor", to_string(assessorId), {}}, "Assessor deleted successfully");
    return true;
}

// Utility Methods Implementation

bool AssessorManager::exists(int assessorId) const {
    const string sql = "SELECT COUNT(*) FROM assessor WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare exists statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, assessorId);
    
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    
    sqlite3_finalize(stmt);
    return exists;
}

int AssessorManager::getCount() const {
    const string sql = "SELECT COUNT(*) FROM assessor";
    
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

vector<CaseProfile> AssessorManager::getCasesByAssessorId(int assessorId) const {
    vector<CaseProfile> cases;
    
    const string sql = R"(
        SELECT id, client_id, assessor_id, status, notes, created_at, closed_at, modified_at
        FROM case_profile
        WHERE assessor_id = ?
        ORDER BY created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare getCases statement");
        return cases;
    }
    
    sqlite3_bind_int(stmt, 1, assessorId);
    
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

vector<Assessor> AssessorManager::searchByName(const string& searchTerm) const {
    vector<Assessor> assessors;
    
    const string sql = R"(
        SELECT a.id, a.firstname, a.lastname, a.phone, a.email, a.created_at, a.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM assessor a
        LEFT JOIN address addr ON a.id = addr.user_key
        WHERE a.firstname LIKE ? OR a.lastname LIKE ?
        ORDER BY a.lastname, a.firstname
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare searchByName statement");
        return assessors;
    }
    
    string searchPattern = "%" + searchTerm + "%";
    sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, searchPattern.c_str(), -1, SQLITE_TRANSIENT);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        assessors.push_back(createAssessorFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return assessors;
}

optional<Assessor> AssessorManager::findByEmail(const string& email) const {
    const string sql = R"(
        SELECT a.id, a.firstname, a.lastname, a.phone, a.email, a.created_at, a.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM assessor a
        LEFT JOIN address addr ON a.id = addr.user_key
        WHERE a.email = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare findByEmail statement");
        return nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    
    optional<Assessor> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = createAssessorFromRow(stmt);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

vector<Assessor> AssessorManager::readWithPagination(int limit, int offset) const {
    vector<Assessor> assessors;
    
    const string sql = R"(
        SELECT a.id, a.firstname, a.lastname, a.phone, a.email, a.created_at, a.modified_at,
               addr.id as addr_id, addr.street, addr.city, addr.province, addr.postal_code,
               addr.created_at as addr_created, addr.modified_at as addr_modified
        FROM assessor a
        LEFT JOIN address addr ON a.id = addr.user_key
        ORDER BY a.lastname, a.firstname
        LIMIT ? OFFSET ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readWithPagination statement");
        return assessors;
    }
    
    sqlite3_bind_int(stmt, 1, limit);
    sqlite3_bind_int(stmt, 2, offset);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        assessors.push_back(createAssessorFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return assessors;
}

// Validation Methods

bool AssessorManager::validateAssessor(const Assessor& assessor) const {
    // Check required fields
    if (assessor.getFirstName().empty()) {
    utils::logStructured(utils::LogLevel::ERROR, {"VALIDATION","missing_firstname","Assessor", std::to_string(assessor.getAssessorId()), {}}, "First name is required");
        return false;
    }
    
    if (assessor.getLastName().empty()) {
    utils::logStructured(utils::LogLevel::ERROR, {"VALIDATION","missing_lastname","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Last name is required");
        return false;
    }
    
    // Use Utils functions for validation
    if (!utils::isValidEmail(assessor.getEmail())) {
    utils::logStructured(utils::LogLevel::ERROR, {"VALIDATION","invalid_email","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Invalid email: "+assessor.getEmail());
        return false;
    }
    
    if (!utils::isValidCanadianPhoneNumber(assessor.getPhone())) {
    utils::logStructured(utils::LogLevel::ERROR, {"VALIDATION","invalid_phone","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Invalid phone: "+assessor.getPhone());
        return false;
    }
    
    // Check ID range
    if (assessor.getAssessorId() < Assessor::ID_PREFIX) {
    utils::logStructured(utils::LogLevel::ERROR, {"VALIDATION","invalid_id_range","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Invalid assessor ID range");
        return false;
    }
    
    // Check names are not just numeric or contain invalid characters
    if (utils::isNumeric(assessor.getFirstName()) || utils::isNumeric(assessor.getLastName())) {
    utils::logStructured(utils::LogLevel::ERROR, {"VALIDATION","numeric_name","Assessor", std::to_string(assessor.getAssessorId()), {}}, "Names cannot be purely numeric");
        return false;
    }
    
    return true;
}

bool AssessorManager::canDelete(int assessorId) const {
    const string sql = "SELECT COUNT(*) FROM case_profile WHERE assessor_id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare canDelete statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, assessorId);
    
    bool canDelete = true;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        canDelete = (count == 0);
    }
    
    sqlite3_finalize(stmt);
    return canDelete;
}

// Helper Methods

Assessor AssessorManager::createAssessorFromRow(sqlite3_stmt* stmt) const {
    // Extract assessor data with NULL safety
    int id = sqlite3_column_int(stmt, 0);
    
    const char* firstNamePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    string firstName = firstNamePtr ? firstNamePtr : "";
    
    const char* lastNamePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    string lastName = lastNamePtr ? lastNamePtr : "";
    
    const char* phonePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    string phone = phonePtr ? phonePtr : "";
    
    const char* emailPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    string email = emailPtr ? emailPtr : "";
    
    const char* createdAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    DateTime createdAt = createdAtPtr ? DateTime::fromString(createdAtPtr) : DateTime();
    
    const char* modifiedAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    DateTime modifiedAt = modifiedAtPtr ? DateTime::fromString(modifiedAtPtr) : DateTime();
    
    // Extract address data (if exists)
    Address address;
    if (sqlite3_column_type(stmt, 7) != SQLITE_NULL) {
        address = createAddressFromRow(stmt, 7);
    }
    
    return Assessor(id, firstName, lastName, email, phone, address, createdAt, modifiedAt);
}

Address AssessorManager::createAddressFromRow(sqlite3_stmt* stmt, int startColumn) const {
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

void AssessorManager::logDatabaseError(const string& operation) const {
    string errorMsg = "Database error in " + operation + ": " + sqlite3_errmsg(m_db);
    utils::logStructured(utils::LogLevel::ERROR, {"DB","exec_error","Assessor","",""}, errorMsg);
}

int AssessorManager::importFromCSV(const string& filePath) {
    // Delegate to detailed report method for easier testing and richer information
    ImportResult res = importFromCSVReport(filePath);
    return res.success;
}

AssessorManager::ImportResult AssessorManager::importFromCSVReport(const string& filePath) {
    ImportResult result;
    bool inTransaction = false;
    try {
        csv::CSVTable table = csv::CSVReader::readFile(filePath);

        // Begin transaction for performance
        char* errMsg = nullptr;
        if (sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg) == SQLITE_OK) {
            inTransaction = true;
        }

        for (const auto &row : table.rows) {
            Assessor a;
            a.setFirstName(csv::safeGet(row, "firstname"));
            a.setLastName(csv::safeGet(row, "lastname"));
            a.setPhone(csv::safeGet(row, "phone"));
            a.setEmail(csv::safeGet(row, "email"));
            string createdStr = csv::normalizeTimestampForDateTime(csv::safeGet(row, "created_at"));
            if (!createdStr.empty()) {
                a.setCreatedAt(DateTime::fromString(createdStr));
                a.setUpdatedAt(DateTime::fromString(createdStr));
            }

            // Validate
            if (!validateAssessor(a)) {
                result.failed++;
                result.errors.push_back("Validation failed for row with email=" + a.getEmail());
                continue;
            }

            // Duplicate detection
            optional<int> existing = findExistingAssessorId(a);
            if (existing.has_value()) {
                // Log duplicate and record
                result.failed++;
                result.duplicates.emplace_back(existing.value(), "duplicate by email/name+phone");
                continue;
            }

            // Create in DB
            if (create(a)) {
                result.success++;
            } else {
                result.failed++;
                result.errors.push_back("DB insert failed for email=" + a.getEmail());
            }
        }

        if (inTransaction) {
            sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, &errMsg);
        }
    } catch (const exception &e) {
        if (inTransaction) {
            char* errMsg = nullptr;
            sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, &errMsg);
        }
        result.errors.push_back(string("CSV import exception: ") + e.what());
    }

    return result;
}
