#include "managers/CaseProfileManager.h"
#include "core/Utils.h"
#include "core/DateTime.h"
#include "utils/PDFConfig.h"
#include "utils/CSVUtils.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <hpdf.h>
#include "managers/AutomobileAnxietyInventoryManager.h"
#include "utils/StructuredLogger.h"

using namespace std;
using namespace SilverClinic;
using namespace utils;

namespace SilverClinic {

// Constructor
CaseProfileManager::CaseProfileManager(sqlite3* database) : m_db(database) {
    if (!m_db) {
        utils::LogEventContext ctx{"MANAGER","init","CaseProfile", std::nullopt, std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "CaseProfileManager: Invalid database connection");
    }
}

// ========================================
// CRUD Operations Implementation
// ========================================

bool CaseProfileManager::create(const CaseProfile& caseProfile) {
    if (!validateCaseProfile(caseProfile)) {
        utils::LogEventContext ctx{"MANAGER","create","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Invalid case profile data");
        return false;
    }
    
    // Additional validation: check if client and assessor exist
    if (!validateRelationship(caseProfile.getClientId(), caseProfile.getAssessorId())) {
        utils::LogEventContext ctx{"MANAGER","create","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Invalid client or assessor relationship");
        return false;
    }
    // Defensive double-check using EXISTS to ensure referential integrity prior to insert (even if FK pragma off)
    auto existsQuick=[&](const char* table,int id){
        const char* sql="SELECT 1 FROM %s WHERE id=? LIMIT 1"; // can't param table directly; build string
        std::string q = std::string("SELECT 1 FROM ")+table+" WHERE id=? LIMIT 1";
        sqlite3_stmt* st=nullptr; if(sqlite3_prepare_v2(m_db,q.c_str(),-1,&st,nullptr)!=SQLITE_OK) return false; sqlite3_bind_int(st,1,id); bool ok = sqlite3_step(st)==SQLITE_ROW; sqlite3_finalize(st); return ok; };
    if(!existsQuick("client", caseProfile.getClientId()) || !existsQuick("assessor", caseProfile.getAssessorId())){
        utils::LogEventContext ctx{"MANAGER","create","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Referenced client or assessor not found (pre-insert abort)");
        return false;
    }
    
    const string sql = R"(
        INSERT INTO case_profile (id, client_id, assessor_id, status, notes, created_at, modified_at)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare create statement");
        return false;
    }
    
    // Bind parameters
    sqlite3_bind_int(stmt, 1, caseProfile.getCaseProfileId());
    sqlite3_bind_int(stmt, 2, caseProfile.getClientId());
    sqlite3_bind_int(stmt, 3, caseProfile.getAssessorId());
    sqlite3_bind_text(stmt, 4, caseProfile.getStatus().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, caseProfile.getNotes().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, caseProfile.getCreatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, caseProfile.getUpdatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute create statement");
        return false;
    }
    
    {
        utils::LogEventContext ctx{"MANAGER","create","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::INFO, ctx, "Created successfully");
    }
    return true;
}

vector<CaseProfile> CaseProfileManager::readAll() const {
    vector<CaseProfile> caseProfiles;
    
    const string sql = R"(
        SELECT cp.id, cp.client_id, cp.assessor_id, cp.status, cp.notes, 
               cp.created_at, cp.closed_at, cp.modified_at
        FROM case_profile cp
        ORDER BY cp.created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readAll statement");
        return caseProfiles;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        caseProfiles.push_back(createCaseProfileFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return caseProfiles;
}

optional<CaseProfile> CaseProfileManager::readById(int caseProfileId) const {
    const string sql = R"(
        SELECT cp.id, cp.client_id, cp.assessor_id, cp.status, cp.notes, 
               cp.created_at, cp.closed_at, cp.modified_at
        FROM case_profile cp
        WHERE cp.id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare readById statement");
        return nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, caseProfileId);
    
    optional<CaseProfile> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = createCaseProfileFromRow(stmt);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

bool CaseProfileManager::update(const CaseProfile& caseProfile) {
    if (!validateCaseProfile(caseProfile)) {
        utils::LogEventContext ctx{"MANAGER","update","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Invalid case profile data");
        return false;
    }

    if (!exists(caseProfile.getCaseProfileId())) {
        utils::LogEventContext ctx{"MANAGER","update","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Case profile not found");
        return false;
    }
    
    const string sql = R"(
        UPDATE case_profile 
        SET client_id = ?, assessor_id = ?, status = ?, notes = ?, 
            closed_at = ?, modified_at = ?
        WHERE id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare update statement");
        return false;
    }
    
    // Bind parameters
    sqlite3_bind_int(stmt, 1, caseProfile.getClientId());
    sqlite3_bind_int(stmt, 2, caseProfile.getAssessorId());
    sqlite3_bind_text(stmt, 3, caseProfile.getStatus().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, caseProfile.getNotes().c_str(), -1, SQLITE_TRANSIENT);
    
    // Handle closed_at - can be NULL
    if (caseProfile.isClosed() && !caseProfile.getClosedAt().toString().empty()) {
        sqlite3_bind_text(stmt, 5, caseProfile.getClosedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_null(stmt, 5);
    }
    
    sqlite3_bind_text(stmt, 6, DateTime::now().toString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, caseProfile.getCaseProfileId());
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute update statement");
        return false;
    }
    
    {
        utils::LogEventContext ctx{"MANAGER","update","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::INFO, ctx, "Updated successfully");
    }
    return true;
}

bool CaseProfileManager::deleteById(int caseProfileId) {
    if (!canDelete(caseProfileId)) {
        utils::LogEventContext ctx{"MANAGER","delete","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Cannot delete case profile");
        return false;
    }
    
    // Soft delete - mark as cancelled instead of actual deletion
    return updateCaseStatus(caseProfileId, "Cancelled", "Deleted by user");
}

// ========================================
// Relationship-based Operations Implementation
// ========================================

vector<CaseProfile> CaseProfileManager::getCasesByClientId(int clientId) const {
    vector<CaseProfile> caseProfiles;
    
    const string sql = R"(
        SELECT cp.id, cp.client_id, cp.assessor_id, cp.status, cp.notes, 
               cp.created_at, cp.closed_at, cp.modified_at
        FROM case_profile cp
        WHERE cp.client_id = ?
        ORDER BY cp.created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare getCasesByClientId statement");
        return caseProfiles;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        caseProfiles.push_back(createCaseProfileFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return caseProfiles;
}

vector<CaseProfile> CaseProfileManager::getCasesByAssessorId(int assessorId) const {
    vector<CaseProfile> caseProfiles;
    
    const string sql = R"(
        SELECT cp.id, cp.client_id, cp.assessor_id, cp.status, cp.notes, 
               cp.created_at, cp.closed_at, cp.modified_at
        FROM case_profile cp
        WHERE cp.assessor_id = ?
        ORDER BY cp.created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare getCasesByAssessorId statement");
        return caseProfiles;
    }
    
    sqlite3_bind_int(stmt, 1, assessorId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        caseProfiles.push_back(createCaseProfileFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return caseProfiles;
}

vector<CaseProfile> CaseProfileManager::getCasesByClientAndAssessor(int clientId, int assessorId) const {
    vector<CaseProfile> caseProfiles;
    
    const string sql = R"(
        SELECT cp.id, cp.client_id, cp.assessor_id, cp.status, cp.notes, 
               cp.created_at, cp.closed_at, cp.modified_at
        FROM case_profile cp
        WHERE cp.client_id = ? AND cp.assessor_id = ?
        ORDER BY cp.created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare getCasesByClientAndAssessor statement");
        return caseProfiles;
    }
    
    sqlite3_bind_int(stmt, 1, clientId);
    sqlite3_bind_int(stmt, 2, assessorId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        caseProfiles.push_back(createCaseProfileFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return caseProfiles;
}

// ========================================
// Workflow Management Implementation
// ========================================

bool CaseProfileManager::activateCase(int caseProfileId, int assessorId) {
    if (!validateAssessorPermission(caseProfileId, assessorId)) {
        utils::LogEventContext ctx{"MANAGER","activate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Assessor not authorized");
        return false;
    }

    auto caseProfile = readById(caseProfileId);
    if (!caseProfile.has_value()) {
        utils::LogEventContext ctx{"MANAGER","activate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Case not found");
        return false;
    }

    if (!caseProfile->isPending()) {
        utils::LogEventContext ctx{"MANAGER","activate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Case not pending");
        return false;
    }
    
    return updateCaseStatus(caseProfileId, "Active", "Activated by assessor " + toString(assessorId));
}

bool CaseProfileManager::closeCase(int caseProfileId, int assessorId, const string& reason) {
    if (!validateAssessorPermission(caseProfileId, assessorId)) {
        utils::LogEventContext ctx{"MANAGER","close","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Assessor not authorized");
        return false;
    }

    auto caseProfile = readById(caseProfileId);
    if (!caseProfile.has_value()) {
        utils::LogEventContext ctx{"MANAGER","close","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Case not found");
        return false;
    }

    if (!caseProfile->isActive()) {
        utils::LogEventContext ctx{"MANAGER","close","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Case not active");
        return false;
    }
    
    // Update status and set closed_at timestamp
    const string sql = R"(
        UPDATE case_profile 
        SET status = 'Closed', notes = ?, closed_at = ?, modified_at = ?
        WHERE id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare closeCase statement");
        return false;
    }
    
    string updatedNotes = caseProfile->getNotes();
    if (!reason.empty()) {
        updatedNotes += "\n[CLOSED] " + reason + " - " + getCurrentTimestamp();
    }
    
    string currentTime = getCurrentTimestamp();
    sqlite3_bind_text(stmt, 1, updatedNotes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, currentTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, currentTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, caseProfileId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute closeCase statement");
        return false;
    }
    
    { utils::LogEventContext ctx{"MANAGER","close","CaseProfile", std::to_string(caseProfileId), std::nullopt}; logStructured(utils::LogLevel::INFO, ctx, "Case closed successfully"); }
    return true;
}

bool CaseProfileManager::transferCase(int caseProfileId, int newAssessorId, int currentAssessorId) {
    if (!validateAssessorPermission(caseProfileId, currentAssessorId)) {
        utils::LogEventContext ctx{"MANAGER","transfer","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Current assessor not authorized");
        return false;
    }

    // Validate new assessor exists
    if (!validateRelationship(0, newAssessorId)) { // We only care about assessor existing
        utils::LogEventContext ctx{"MANAGER","transfer","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "New assessor not found");
        return false;
    }
    
    const string sql = R"(
        UPDATE case_profile 
        SET assessor_id = ?, notes = ?, modified_at = ?
        WHERE id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare transferCase statement");
        return false;
    }
    
    // Get current notes and add transfer information
    auto caseProfile = readById(caseProfileId);
    string updatedNotes = caseProfile.has_value() ? caseProfile->getNotes() : "";
    updatedNotes += "\n[TRANSFER] From assessor " + toString(currentAssessorId) + 
                   " to " + toString(newAssessorId) + " - " + getCurrentTimestamp();
    
    sqlite3_bind_int(stmt, 1, newAssessorId);
    sqlite3_bind_text(stmt, 2, updatedNotes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, getCurrentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, caseProfileId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        logDatabaseError("execute transferCase statement");
        return false;
    }
    
    { utils::LogEventContext ctx{"MANAGER","transfer","CaseProfile", std::to_string(caseProfileId), std::nullopt}; logStructured(utils::LogLevel::INFO, ctx, "Case transferred successfully"); }
    return true;
}

// ========================================
// Helper Methods Implementation
// ========================================

CaseProfile CaseProfileManager::createCaseProfileFromRow(sqlite3_stmt* stmt) const {
    // Extract case profile data with NULL safety
    int id = sqlite3_column_int(stmt, 0);
    int clientId = sqlite3_column_int(stmt, 1);
    int assessorId = sqlite3_column_int(stmt, 2);
    
    const char* statusPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    string status = statusPtr ? statusPtr : "Pending";
    
    const char* notesPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    string notes = notesPtr ? notesPtr : "";
    
    const char* createdAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    DateTime createdAt = createdAtPtr ? DateTime::fromString(createdAtPtr) : DateTime();
    
    const char* closedAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    DateTime closedAt = closedAtPtr ? DateTime::fromString(closedAtPtr) : DateTime();
    
    const char* modifiedAtPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    DateTime modifiedAt = modifiedAtPtr ? DateTime::fromString(modifiedAtPtr) : DateTime();
    
    return CaseProfile(id, clientId, assessorId, status, notes, createdAt, closedAt, modifiedAt);
}

bool CaseProfileManager::validateCaseProfile(const CaseProfile& caseProfile) const {
    // Validate ID range (400001-499999)
    if (caseProfile.getCaseProfileId() < 400001 || caseProfile.getCaseProfileId() > 499999) {
        utils::LogEventContext ctx{"MANAGER","validate","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Invalid case profile ID range");
        return false;
    }
    
    // Validate client ID range (300001-399999)
    if (caseProfile.getClientId() < 300001 || caseProfile.getClientId() > 399999) {
        utils::LogEventContext ctx{"MANAGER","validate","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Invalid client ID range");
        return false;
    }
    
    // Validate assessor ID range (100001-199999)
    if (caseProfile.getAssessorId() < 100001 || caseProfile.getAssessorId() > 199999) {
        utils::LogEventContext ctx{"MANAGER","validate","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Invalid assessor ID range");
        return false;
    }
    
    // Validate status
    string status = caseProfile.getStatus();
    // Accept both human-readable (capitalized) and normalized uppercase variants
    if (status != "Pending" && status != "Active" && status != "Closed" && status != "Cancelled" &&
        status != "PENDING" && status != "ACTIVE" && status != "CLOSED" && status != "CANCELLED") {
        utils::LogEventContext ctx{"MANAGER","validate","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, std::string("Invalid status: ")+status);
        return false;
    }
    
    // Validate notes length
    if (caseProfile.getNotes().length() > 1500) {
        utils::LogEventContext ctx{"MANAGER","validate","CaseProfile", std::to_string(caseProfile.getCaseProfileId()), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Notes too long");
        return false;
    }
    
    return true;
}

bool CaseProfileManager::exists(int caseProfileId) const {
    const string sql = "SELECT 1 FROM case_profile WHERE id = ? LIMIT 1";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare exists statement");
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, caseProfileId);
    
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    
    return exists;
}

bool CaseProfileManager::validateRelationship(int clientId, int assessorId) const {
    // Check if client exists (if clientId is provided)
    if (clientId > 0) {
        const string clientSql = "SELECT 1 FROM client WHERE id = ? LIMIT 1";
        sqlite3_stmt* clientStmt;
        if (sqlite3_prepare_v2(m_db, clientSql.c_str(), -1, &clientStmt, nullptr) != SQLITE_OK) {
            return false;
        }
        sqlite3_bind_int(clientStmt, 1, clientId);
        bool clientExists = (sqlite3_step(clientStmt) == SQLITE_ROW);
        sqlite3_finalize(clientStmt);
        
        if (!clientExists) {
            return false;
        }
    }
    
    // Check if assessor exists
    const string assessorSql = "SELECT 1 FROM assessor WHERE id = ? LIMIT 1";
    sqlite3_stmt* assessorStmt;
    if (sqlite3_prepare_v2(m_db, assessorSql.c_str(), -1, &assessorStmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(assessorStmt, 1, assessorId);
    bool assessorExists = (sqlite3_step(assessorStmt) == SQLITE_ROW);
    sqlite3_finalize(assessorStmt);
    
    return assessorExists;
}

bool CaseProfileManager::validateAssessorPermission(int caseProfileId, int assessorId) const {
    const string sql = "SELECT assessor_id FROM case_profile WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, caseProfileId);
    
    bool authorized = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int currentAssessorId = sqlite3_column_int(stmt, 0);
        authorized = (currentAssessorId == assessorId);
    }
    
    sqlite3_finalize(stmt);
    return authorized;
}

bool CaseProfileManager::updateCaseStatus(int caseProfileId, const string& newStatus, const string& reason) {
    const string sql = R"(
        UPDATE case_profile 
        SET status = ?, notes = ?, modified_at = ?
        WHERE id = ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare updateCaseStatus statement");
        return false;
    }
    
    // Get current notes and add status change information
    auto caseProfile = readById(caseProfileId);
    string updatedNotes = caseProfile.has_value() ? caseProfile->getNotes() : "";
    if (!reason.empty()) {
        updatedNotes += "\n[STATUS CHANGE] " + newStatus + ": " + reason + " - " + getCurrentTimestamp();
    }
    
    sqlite3_bind_text(stmt, 1, newStatus.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, updatedNotes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, getCurrentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, caseProfileId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return (result == SQLITE_DONE);
}

string CaseProfileManager::getCurrentTimestamp() const {
    return DateTime::now().toString();
}

void CaseProfileManager::logDatabaseError(const string& operation) const {
    string errorMsg = "Database error in " + operation + ": " + sqlite3_errmsg(m_db);
    utils::LogEventContext ctx{"DB","error","CaseProfile", std::nullopt, std::nullopt};
    logStructured(utils::LogLevel::ERROR, ctx, errorMsg);
}

// ========================================
// Additional utility methods (simplified for now)
// ========================================

int CaseProfileManager::getCount() const {
    const string sql = "SELECT COUNT(*) FROM case_profile";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

bool CaseProfileManager::canDelete(int caseProfileId) const {
    auto caseProfile = readById(caseProfileId);
    if (!caseProfile.has_value()) {
        return false;
    }
    
    // Business rule: Can only delete pending cases
    return caseProfile->isPending();
}

vector<CaseProfile> CaseProfileManager::getCasesByStatus(const string& status) const {
    vector<CaseProfile> caseProfiles;
    
    const string sql = R"(
        SELECT cp.id, cp.client_id, cp.assessor_id, cp.status, cp.notes, 
               cp.created_at, cp.closed_at, cp.modified_at
        FROM case_profile cp
        WHERE cp.status = ?
        ORDER BY cp.created_at DESC
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        logDatabaseError("prepare getCasesByStatus statement");
        return caseProfiles;
    }
    
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        caseProfiles.push_back(createCaseProfileFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return caseProfiles;
}

// ========================================
// PDF Export and Reporting Implementation
// ========================================

bool CaseProfileManager::generatePDFReport(int caseProfileId, const string& outputPath, const string& reportType) const {
    // Get case profile data
    auto caseProfile = readById(caseProfileId);
    if (!caseProfile.has_value()) {
        utils::LogEventContext ctx{"PDF","generate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Case profile not found");
        return false;
    }
    
    // Initialize PDF document
    HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
    if (!pdf) {
        utils::LogEventContext ctx{"PDF","generate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, "Failed to create PDF document");
        return false;
    }
    
    try {
        // Set compression mode
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
        
        // Add a page
        HPDF_Page page = HPDF_AddPage(pdf);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
        
        // Get template configuration
        PDFConfig::ReportTemplate template_config = PDFConfig::getTemplate(reportType);
        
        // Generate PDF content
        float currentY = PDFConfig::PAGE_HEIGHT - PDFConfig::MARGIN_TOP;
        
        if (template_config.includeHeader) {
            // Generate header inline
            HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
            HPDF_Page_SetFontAndSize(page, font, PDFConfig::TITLE_FONT_SIZE);
            HPDF_Page_SetRGBFill(page, PDFConfig::HEADER_BLUE.r, PDFConfig::HEADER_BLUE.g, PDFConfig::HEADER_BLUE.b);
            
            float textWidth = HPDF_Page_TextWidth(page, PDFConfig::CLINIC_NAME.c_str());
            float x = (PDFConfig::PAGE_WIDTH - textWidth) / 2;
            float y = currentY - 20;
            
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, x, y, PDFConfig::CLINIC_NAME.c_str());
            HPDF_Page_EndText(page);
            
            currentY -= 80; // Header height
        }
        
        if (template_config.includeClientInfo) {
            // Generate case info inline
            HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", nullptr);
            HPDF_Page_SetFontAndSize(page, font, PDFConfig::TEXT_FONT_SIZE);
            HPDF_Page_SetRGBFill(page, PDFConfig::TEXT_BLACK.r, PDFConfig::TEXT_BLACK.g, PDFConfig::TEXT_BLACK.b);
            
            float x = PDFConfig::MARGIN_LEFT;
            float y = currentY - 20;
            
            string caseInfo = "Case ID: " + to_string(caseProfile->getCaseProfileId());
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, x, y, caseInfo.c_str());
            HPDF_Page_EndText(page);
            
            currentY -= 60; // Case info section height
        }
        
        // Inject Automobile Anxiety Inventory summary (latest record)
        try {
            AutomobileAnxietyInventoryManager aaiMgr(m_db);
            auto list = aaiMgr.listByCase(caseProfileId);
            if(!list.empty()){
                auto latest = list.back();
                HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", nullptr);
                HPDF_Page_SetFontAndSize(page, font, PDFConfig::TEXT_FONT_SIZE);
                float x = PDFConfig::MARGIN_LEFT;
                float y = currentY - 20;
                std::string heading = "Automobile Anxiety Inventory (latest)";
                HPDF_Page_BeginText(page); HPDF_Page_TextOut(page, x, y, heading.c_str()); HPDF_Page_EndText(page);
                y -= 14;
                auto boolToStr=[](bool b){ return b?"Yes":"No"; };
                std::string line1 = std::string("Q1:")+boolToStr(latest.getQuestion1())+" Q2:"+boolToStr(latest.getQuestion2())+" Q3:"+boolToStr(latest.getQuestion3());
                HPDF_Page_BeginText(page); HPDF_Page_TextOut(page, x, y, line1.c_str()); HPDF_Page_EndText(page); y -= 12;
                std::string line2 = std::string("Q14 Driver:")+boolToStr(latest.getQuestion14Driver())+" Passenger:"+boolToStr(latest.getQuestion14Passenger())+" NoDiff:"+boolToStr(latest.getQuestion14NoDifference());
                HPDF_Page_BeginText(page); HPDF_Page_TextOut(page, x, y, line2.c_str()); HPDF_Page_EndText(page); y -= 12;
                std::string line3 = std::string("Q19:")+boolToStr(latest.getQuestion19())+" Sidewalks:"+boolToStr(latest.getQuestion19Sidewalks())+" Crossing:"+boolToStr(latest.getQuestion19Crossing())+" Both:"+boolToStr(latest.getQuestion19Both());
                HPDF_Page_BeginText(page); HPDF_Page_TextOut(page, x, y, line3.c_str()); HPDF_Page_EndText(page); y -= 14;
                currentY = y;
                utils::LogEventContext ctx{"PDF","embed_form","AAI", std::to_string(latest.getAAIId()), std::nullopt};
                logStructured(utils::LogLevel::INFO, ctx, "Embedded latest AAI summary into report");
            }
        } catch(const std::exception &e){
            utils::LogEventContext ctx{"PDF","embed_form","AAI", std::nullopt, std::nullopt};
            logStructured(utils::LogLevel::ERROR, ctx, std::string("Failed to embed AAI: ")+e.what());
        }

        if (template_config.includeFooter) {
            // Generate footer inline
            HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", nullptr);
            HPDF_Page_SetFontAndSize(page, font, PDFConfig::FOOTER_FONT_SIZE);
            HPDF_Page_SetRGBFill(page, PDFConfig::TEXT_SECONDARY.r, PDFConfig::TEXT_SECONDARY.g, PDFConfig::TEXT_SECONDARY.b);
            
            float x = PDFConfig::MARGIN_LEFT;
            float y = PDFConfig::MARGIN_BOTTOM + 20;
            
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, x, y, PDFConfig::CONFIDENTIALITY_NOTICE.c_str());
            HPDF_Page_EndText(page);
        }
        
        // Save PDF to file
        HPDF_SaveToFile(pdf, outputPath.c_str());
        
        {
            utils::LogEventContext ctx{"PDF","generate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
            logStructured(utils::LogLevel::INFO, ctx, std::string("PDF generated ")+outputPath);
        }
        
    } catch (const exception& e) {
        utils::LogEventContext ctx{"PDF","generate","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::ERROR, ctx, std::string("Exception: ")+e.what());
        HPDF_Free(pdf);
        return false;
    }
    
    // Clean up
    HPDF_Free(pdf);
    return true;
}

int CaseProfileManager::generateBulkPDFReports(const vector<int>& caseProfileIds, const string& outputDirectory, const string& reportType) const {
    int successCount = 0;
    
    for (int caseId : caseProfileIds) {
        string filename = "case_profile_" + to_string(caseId) + "_" + reportType + ".pdf";
        string fullPath = outputDirectory + "/" + filename;
        
        if (generatePDFReport(caseId, fullPath, reportType)) {
            successCount++;
        }
    }
    
    {
        utils::LogEventContext ctx{"PDF","bulk_generate","CaseProfile", std::nullopt, std::nullopt};
        logStructured(utils::LogLevel::INFO, ctx, "Completed: "+ std::to_string(successCount) + "/" + std::to_string(caseProfileIds.size()));
    }
    return successCount;
}

bool CaseProfileManager::generateCustomPDFReport(int caseProfileId, const string& outputPath, 
                                                bool /*includeTimeline*/, bool /*includeFullNotes*/, 
                                                const string& /*watermarkText*/) const {
    // Custom implementation similar to generatePDFReport but with custom options
    {
        utils::LogEventContext ctx{"PDF","custom_request","CaseProfile", std::to_string(caseProfileId), std::nullopt};
        logStructured(utils::LogLevel::INFO, ctx, "Custom PDF generation requested");
    }
    
    // For now, use the detailed template and modify behavior
    return generatePDFReport(caseProfileId, outputPath, "detailed");
}

// ========================================
// PDF Generation Helper Methods
// ========================================

bool CaseProfileManager::generatePDFHeader(void* /*pdfPage*/, const string& /*reportType*/) const {
    return true; // deprecated stub
}

bool CaseProfileManager::generatePDFCaseInfo(void* /*pdfPage*/, const CaseProfile& /*caseProfile*/) const {
    return true; // deprecated stub
}

bool CaseProfileManager::generatePDFClientInfo(void* /*pdfPage*/, int /*clientId*/) const {
    return true; // deprecated stub
}

bool CaseProfileManager::generatePDFAssessorInfo(void* /*pdfPage*/, int /*assessorId*/) const {
    return true; // deprecated stub
}

bool CaseProfileManager::generatePDFTimeline(void* /*pdfPage*/, int /*caseProfileId*/) const {
    return true; // deprecated stub
}

bool CaseProfileManager::generatePDFNotes(void* /*pdfPage*/, const string& /*notes*/) const {
    return true; // deprecated stub
}

bool CaseProfileManager::generatePDFFooter(void* /*pdfPage*/) const {
    return true; // deprecated stub
}

// Helper methods for PDF formatting
string CaseProfileManager::formatDateForPDF(const string& isoDate) const {
    return PDFConfig::formatDate(isoDate, true);
}

string CaseProfileManager::getStatusIcon(const string& status) const {
    auto icons = PDFConfig::getStatusIcons();
    auto it = icons.find(status);
    return (it != icons.end()) ? it->second : "●";
}

int CaseProfileManager::importFromCSV(const string& filePath) {
    int success = 0;
    int failed  = 0;
    bool inTransaction = false;
    try {
        auto table = csv::CSVReader::readFile(filePath);
        const vector<string> required = {"client_id","assessor_id","status","notes","created_at"};
        for (const auto &h : required) {
            if (find(table.headers.begin(), table.headers.end(), h) == table.headers.end()) {
                logMessage("ERROR", "CaseProfileManager::importFromCSV - Missing required header: " + h);
                return 0; // structural error
            }
        }
        if (sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) == SQLITE_OK) {
            inTransaction = true;
        } else {
            logMessage("ERROR", "CaseProfileManager::importFromCSV - Failed to BEGIN TRANSACTION (continuing non-atomic)");
        }
        for (const auto &row : table.rows) {
            try {
                int clientId = stoi(csv::safeGet(row, "client_id"));
                int assessorId = stoi(csv::safeGet(row, "assessor_id"));
                string status = csv::safeGet(row, "status");
                string notes = csv::safeGet(row, "notes");
                string createdAtRaw = csv::safeGet(row, "created_at");
                string createdAtNorm = csv::normalizeTimestampForDateTime(createdAtRaw);
                DateTime createdAt = createdAtRaw.empty() ? DateTime::now() : DateTime::fromString(createdAtNorm);
                string closedAtRaw = csv::safeGet(row, "closed_at");
                DateTime closedAt; if (!closedAtRaw.empty()) closedAt = DateTime::fromString(csv::normalizeTimestampForDateTime(closedAtRaw));
                DateTime modifiedAt = createdAt;

                int id = CaseProfile::getNextCaseProfileId();
                CaseProfile cp(id, clientId, assessorId, status, notes, createdAt, closedAt, modifiedAt);
                if (!create(cp)) {
                    failed++;
                    logMessage("ERROR", "Failed to insert case profile from CSV row (id attempt: " + toString(id) + ")");
                    continue;
                }
                if (!closedAtRaw.empty()) {
                    const string upd = "UPDATE case_profile SET closed_at = ? WHERE id = ?";
                    sqlite3_stmt* stmt;
                    if (sqlite3_prepare_v2(m_db, upd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_text(stmt, 1, closedAt.toString().c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_int(stmt, 2, id);
                        if (sqlite3_step(stmt) != SQLITE_DONE) {
                            logMessage("ERROR", "Failed to update closed_at for imported case profile id: " + toString(id));
                        }
                        sqlite3_finalize(stmt);
                    }
                }
                success++;
            } catch (const exception &e) {
                failed++;
                logMessage("ERROR", string("CaseProfileManager::importFromCSV row error: ")+ e.what());
            }
        }
        if (inTransaction) {
            if (sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
                logMessage("ERROR", "CaseProfileManager::importFromCSV - COMMIT failed, attempting ROLLBACK");
                sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
            }
        }
    } catch (const exception &e) {
        if (inTransaction) {
            sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        }
        logMessage("ERROR", string("CaseProfileManager::importFromCSV file error: ") + e.what());
    }
    logMessage("INFO", "CaseProfileManager::importFromCSV imported success=" + toString(success) + ", failed=" + toString(failed));
    return success;
}

} // namespace SilverClinic
