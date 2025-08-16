#ifndef SILVERCLINIC_CASE_PROFILE_SERVICE_H
#define SILVERCLINIC_CASE_PROFILE_SERVICE_H

#include <vector>
#include "core/CaseProfile.h"
#include "managers/CaseProfileManager.h"
#include "utils/Result.h"
#include "utils/IdAllocator.h"
#include "utils/ValidationMessages.h"

namespace SilverClinic {

class CaseProfileService {
public:
    explicit CaseProfileService(CaseProfileManager &manager) : m_manager(manager) {}

    Result create(int clientId, int assessorId, const std::string &status, const std::string &notes) {
        // Validation
        if (clientId < 300001) return Result::Error("validation.invalid_client_id", ValidationMessages::messageFor("validation.invalid_client_id"));
        if (assessorId < 100001) return Result::Error("validation.invalid_assessor_id", ValidationMessages::messageFor("validation.invalid_assessor_id"));
        if (notes.size() > CaseProfile::MAX_NOTES_LENGTH) return Result::Error("validation.invalid_notes_length", ValidationMessages::messageFor("validation.invalid_notes_length"));

        auto nextIdOpt = IdAllocator::next(m_manager.getDb(), "case_profile", 400001);
        if (!nextIdOpt) return Result::Error("id.allocation_failed", ValidationMessages::messageFor("id.allocation_failed"));
        int id = *nextIdOpt;

        DateTime now = DateTime::now();
        CaseProfile cp(id, clientId, assessorId, status, notes, now, DateTime(), now);
        if (!m_manager.create(cp)) {
            return Result::Error("db.step_failed", ValidationMessages::messageFor("db.step_failed"));
        }
        utils::logStructured(utils::LogLevel::INFO, {"SERVICE","create","CaseProfile", std::to_string(id), {}}, "CaseProfileService created case profile");
        return Result::Ok("Case profile criado");
    }

    std::vector<CaseProfile> listAll() { return m_manager.listAll(); }

private:
    CaseProfileManager &m_manager;
};

}

#endif
