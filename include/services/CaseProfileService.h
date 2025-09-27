// ========================================
// Silver Clinic Management System
// Case Profile Service Layer
// ========================================

#ifndef SILVERCLINIC_CASE_PROFILE_SERVICE_H
#define SILVERCLINIC_CASE_PROFILE_SERVICE_H

#include <vector>
#include <optional>
#include "core/CaseProfile.h"
#include "managers/CaseProfileManager.h"
#include "utils/Result.h"
#include "utils/IdAllocator.h"
#include "utils/ValidationMessages.h"
#include "utils/StructuredLogger.h"

namespace SilverClinic {

/**
 * @brief Service layer for Case Profile operations
 * 
 * This service provides a high-level interface for case profile management,
 * including validation, business logic enforcement, and error handling.
 * 
 * Features:
 * - Case profile creation with validation
 * - List operations with filtering
 * - Business rule enforcement
 * - Structured logging
 * - Result pattern for error handling
 */
class CaseProfileService {
public:
    explicit CaseProfileService(CaseProfileManager &manager) : m_manager(manager) {}

    // ========================================
    // Core Operations
    // ========================================
    
    /**
     * @brief Create a new case profile
     * @param clientId Client ID (must be >= 300001)
     * @param assessorId Assessor ID (must be >= 100001)  
     * @param status Initial status ("Pending", "Active", "Closed")
     * @param notes Optional notes (max length enforced)
     * @return Result indicating success or failure with error details
     */
    Result create(int clientId, int assessorId, const std::string &status, const std::string &notes) {
        // Input validation
        if (clientId < 300001) {
            utils::logStructured(utils::LogLevel::WARN, {"SERVICE","validation","CaseProfile", "", {}}, "Invalid client ID: " + std::to_string(clientId));
            return Result::Error("validation.invalid_client_id", ValidationMessages::messageFor("validation.invalid_client_id"));
        }
        if (assessorId < 100001) {
            utils::logStructured(utils::LogLevel::WARN, {"SERVICE","validation","CaseProfile", "", {}}, "Invalid assessor ID: " + std::to_string(assessorId));
            return Result::Error("validation.invalid_assessor_id", ValidationMessages::messageFor("validation.invalid_assessor_id"));
        }
        if (notes.size() > CaseProfile::MAX_NOTES_LENGTH) {
            utils::logStructured(utils::LogLevel::WARN, {"SERVICE","validation","CaseProfile", "", {}}, "Notes too long: " + std::to_string(notes.size()) + " chars");
            return Result::Error("validation.invalid_notes_length", ValidationMessages::messageFor("validation.invalid_notes_length"));
        }

        // ID allocation
        auto nextIdOpt = IdAllocator::next(m_manager.getDb(), "case_profile", 400001);
        if (!nextIdOpt) {
            utils::logStructured(utils::LogLevel::ERROR, {"SERVICE","id_allocation","CaseProfile", "", {}}, "Failed to allocate new case profile ID");
            return Result::Error("id.allocation_failed", ValidationMessages::messageFor("id.allocation_failed"));
        }
        int id = *nextIdOpt;

        // Create case profile
        DateTime now = DateTime::now();
        CaseProfile cp(id, clientId, assessorId, status, notes, now, DateTime(), now);
        if (!m_manager.create(cp)) {
            utils::logStructured(utils::LogLevel::ERROR, {"SERVICE","create","CaseProfile", std::to_string(id), {}}, "Database creation failed");
            return Result::Error("db.step_failed", ValidationMessages::messageFor("db.step_failed"));
        }
        
        utils::logStructured(utils::LogLevel::INFO, {"SERVICE","create","CaseProfile", std::to_string(id), {}}, "Case profile created successfully");
        return Result::Ok("Case profile criado com ID: " + std::to_string(id));
    }

    /**
     * @brief List all case profiles
     * @return Vector of all case profiles in the system
     */
    std::vector<CaseProfile> listAll() { 
        utils::logStructured(utils::LogLevel::DEBUG, {"SERVICE","list","CaseProfile", "", {}}, "Listing all case profiles");
        return m_manager.readAll(); 
    }
    
    /**
     * @brief Get case profiles by client ID
     * @param clientId The client ID to filter by
     * @return Vector of case profiles for the specified client
     */
    std::vector<CaseProfile> getByClientId(int clientId) {
        utils::logStructured(utils::LogLevel::DEBUG, {"SERVICE","filter","CaseProfile", std::to_string(clientId), {}}, "Getting cases by client ID");
        return m_manager.getCasesByClientId(clientId);
    }
    
    /**
     * @brief Get case profiles by assessor ID
     * @param assessorId The assessor ID to filter by
     * @return Vector of case profiles for the specified assessor
     */
    std::vector<CaseProfile> getByAssessorId(int assessorId) {
        utils::logStructured(utils::LogLevel::DEBUG, {"SERVICE","filter","CaseProfile", std::to_string(assessorId), {}}, "Getting cases by assessor ID");
        return m_manager.getCasesByAssessorId(assessorId);
    }
    
    /**
     * @brief Get case profiles by status
     * @param status The status to filter by ("Pending", "Active", "Closed")
     * @return Vector of case profiles with the specified status
     */
    std::vector<CaseProfile> getByStatus(const std::string& status) {
        utils::logStructured(utils::LogLevel::DEBUG, {"SERVICE","filter","CaseProfile", status, {}}, "Getting cases by status");
        return m_manager.getCasesByStatus(status);
    }
    
    /**
     * @brief Get total count of case profiles
     * @return Number of case profiles in the system
     */
    int getCount() const {
        return m_manager.getCount();
    }

private:
    CaseProfileManager &m_manager;
};

}

#endif
