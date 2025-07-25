#include "core/CaseProfile.h"
#include "core/Utils.h"
#include <iostream>
#include <iomanip>

using namespace utils;

namespace SilverClinic {

    // Static variable for ID management
    static int case_profile_id_counter = 1;

    // Constructors
    CaseProfile::CaseProfile() {
        m_case_profile_id = getNextCaseProfileId();
        m_client_id = 0;
        m_assessor_id = 0;
        m_status = STATUS_PENDING;
        m_notes = "";
        m_closed_at = DateTime(); // Empty DateTime
        setTimestamps();
    }

    CaseProfile::CaseProfile(int client_id, int assessor_id, const string& notes) {
        m_case_profile_id = getNextCaseProfileId();
        m_client_id = client_id;
        m_assessor_id = assessor_id;
        m_status = STATUS_PENDING; // Always starts as Pending
        m_notes = utils::normalizeForDatabase(notes);
        m_closed_at = DateTime(); // Empty DateTime
        setTimestamps();
        utils::logMessage("INFO", "New Case Profile created with ID " + to_string(m_case_profile_id) + " (Status: Pending)");
    }

    CaseProfile::CaseProfile(int case_profile_id, int client_id, int assessor_id, const string& status,
                             const string& notes, const DateTime& created_at, const DateTime& closed_at,
                             const DateTime& updated_at) {
        m_case_profile_id = case_profile_id;
        m_client_id = client_id;
        m_assessor_id = assessor_id;
        m_status = getStatusFromString(status);
        m_notes = utils::normalizeForDatabase(notes);
        m_created_at = created_at;
        m_closed_at = closed_at;
        m_updated_at = updated_at;
    }

    // Copy constructor
    CaseProfile::CaseProfile(const CaseProfile& other) {
        m_case_profile_id = other.m_case_profile_id;
        m_client_id = other.m_client_id;
        m_assessor_id = other.m_assessor_id;
        m_status = other.m_status;
        m_notes = other.m_notes;
        m_created_at = other.m_created_at;
        m_closed_at = other.m_closed_at;
        m_updated_at = other.m_updated_at;
    }

    CaseProfile& CaseProfile::operator=(const CaseProfile& other) {
        if (this != &other) {
            m_case_profile_id = other.m_case_profile_id;
            m_client_id = other.m_client_id;
            m_assessor_id = other.m_assessor_id;
            m_status = other.m_status;
            m_notes = other.m_notes;
            m_created_at = other.m_created_at;
            m_closed_at = other.m_closed_at;
            m_updated_at = other.m_updated_at;
        }
        return *this;
    }

    // Destructor
    CaseProfile::~CaseProfile() {
        // Nothing to clean up for now
    }

    // Status management methods
    bool CaseProfile::isActive() const {
        return m_status == STATUS_ACTIVE;
    }

    bool CaseProfile::isClosed() const {
        return m_status == STATUS_CLOSED;
    }

    bool CaseProfile::isPending() const {
        return m_status == STATUS_PENDING;
    }

    void CaseProfile::activate() {
        setStatusId(STATUS_ACTIVE);
        utils::logMessage("INFO", "Case Profile " + to_string(m_case_profile_id) + " activated");
    }

    void CaseProfile::close(const string& reason) {
        setStatusId(STATUS_CLOSED);
        m_closed_at = DateTime::now();
        if (!reason.empty()) {
            string closeNote = (m_notes.empty() ? "" : "\n") + string("Closed: ") + reason;
            string newNotes = m_notes + closeNote;
            
            if (newNotes.length() > MAX_NOTES_LENGTH) {
                // Truncate to fit within limit
                int availableSpace = MAX_NOTES_LENGTH - m_notes.length();
                if (availableSpace > 10) { // Minimum space for close note
                    string truncatedCloseNote = closeNote.substr(0, availableSpace);
                    m_notes += truncatedCloseNote;
                    utils::logMessage("WARNING", "Close reason truncated to fit within notes limit");
                } else {
                    utils::logMessage("WARNING", "Cannot add close reason - notes too long");
                }
            } else {
                m_notes = newNotes;
            }
        }
        updateTimestamp();
        utils::logMessage("INFO", "Case Profile " + to_string(m_case_profile_id) + " closed");
    }

    void CaseProfile::setPending() {
        setStatusId(STATUS_PENDING);
        utils::logMessage("INFO", "Case Profile " + to_string(m_case_profile_id) + " set to pending");
    }

    // Validation methods
    bool CaseProfile::isValidClientId() const {
        return m_client_id >= 300001; // Client IDs start at 300001
    }

    bool CaseProfile::isValidAssessorId() const {
        return m_assessor_id >= 100001; // Assessor IDs start at 100001
    }

    bool CaseProfile::isValidNotes(const string& notes) const {
        return notes.length() <= MAX_NOTES_LENGTH;
    }

    void CaseProfile::displayInfo() const {
        cout << string(60, '=') << endl;
        cout << "              CASE PROFILE INFORMATION              " << endl;
        cout << string(60, '=') << endl;
        cout << "Case Profile ID: " << m_case_profile_id << endl;
        cout << "Client ID: " << m_client_id << endl;
        cout << "Assessor ID: " << m_assessor_id << endl;
        cout << "Status: " << getStatus() << endl;
        cout << "Notes (" << m_notes.length() << "/" << MAX_NOTES_LENGTH << " chars): " << (m_notes.empty() ? "No notes" : m_notes) << endl;
        cout << "Created: " << m_created_at.toString() << endl;
        if (!m_closed_at.toString().empty()) {
            cout << "Closed: " << m_closed_at.toString() << endl;
        }
        cout << "Updated: " << m_updated_at.toString() << endl;
        cout << "Client ID Valid: " << (isValidClientId() ? "Yes" : "No") << endl;
        cout << "Assessor ID Valid: " << (isValidAssessorId() ? "Yes" : "No") << endl;
        cout << string(60, '=') << endl;
    }

    // Static methods for ID management
    int CaseProfile::getNextCaseProfileId() {
        return ID_PREFIX + case_profile_id_counter++;
    }

    void CaseProfile::resetIdCounter() {
        case_profile_id_counter = 1;
        utils::logMessage("INFO", "CaseProfile ID counter reset");
    }

    // Status conversion helper methods
    string CaseProfile::getStatusString(int statusId) {
        switch (statusId) {
            case STATUS_PENDING: return "PENDING";
            case STATUS_ACTIVE: return "ACTIVE";
            case STATUS_CLOSED: return "CLOSED";
            case STATUS_CANCELLED: return "CANCELLED";
            default: return "UNKNOWN";
        }
    }

    int CaseProfile::getStatusFromString(const string& status) {
        string normalizedStatus = utils::normalizeForDatabase(status);
        if (normalizedStatus == "PENDING") return STATUS_PENDING;
        if (normalizedStatus == "ACTIVE") return STATUS_ACTIVE;
        if (normalizedStatus == "CLOSED") return STATUS_CLOSED;
        if (normalizedStatus == "CANCELLED") return STATUS_CANCELLED;
        return STATUS_PENDING; // Default to pending if unknown
    }

    // Stream operators
    ostream& operator<<(ostream& os, const CaseProfile& caseProfile) {
        os << "Case Profile ID: " << caseProfile.m_case_profile_id << endl;
        os << "Client ID: " << caseProfile.m_client_id << endl;
        os << "Assessor ID: " << caseProfile.m_assessor_id << endl;
        os << "Status: " << caseProfile.getStatus() << endl;
        os << "Notes: " << caseProfile.m_notes << endl;
        os << "Created: " << caseProfile.m_created_at.toString() << endl;
        os << "Updated: " << caseProfile.m_updated_at.toString() << endl;
        return os;
    }

    istream& operator>>(istream& is, CaseProfile& caseProfile) {
        cout << "Enter client ID: ";
        is >> caseProfile.m_client_id;
        cout << "Enter assessor ID: ";
        is >> caseProfile.m_assessor_id;
        cout << "Enter status (Active/Pending/Closed): ";
        string statusInput;
        is >> statusInput;
        caseProfile.m_status = CaseProfile::getStatusFromString(statusInput);
        cout << "Enter notes: ";
        is.ignore(); // Clear the newline
        getline(is, caseProfile.m_notes);
        
        caseProfile.updateTimestamp();
        return is;
    }
}
