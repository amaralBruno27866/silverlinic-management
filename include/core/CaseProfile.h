#ifndef CASEPROFILE_H
#define CASEPROFILE_H

#include <string>
#include "DateTime.h"
#include "Utils.h"

using namespace std;
namespace SilverClinic {
  class CaseProfile {
    private:
    int m_case_profile_id;
    int m_client_id;
    int m_assessor_id;
    string m_status;
    string m_notes;
    DateTime m_created_at;
    DateTime m_closed_at;
    DateTime m_updated_at;

    public:
    // Constructors
    CaseProfile();
    CaseProfile(int client_id, int assessor_id, const string& notes = "");
    CaseProfile(int case_profile_id, int client_id, int assessor_id, const string& status,
                const string& notes, const DateTime& created_at, const DateTime& closed_at,
                const DateTime& updated_at);

    // Copy constructor
    CaseProfile(const CaseProfile& other);
    CaseProfile& operator=(const CaseProfile& other);

    // Destructor
    ~CaseProfile();

    // Getters
    int getCaseProfileId() const { return m_case_profile_id; }
    int getClientId() const { return m_client_id; }
    int getAssessorId() const { return m_assessor_id; }
    string getStatus() const { return m_status; }
    string getNotes() const { return m_notes; }
    DateTime getCreatedAt() const { return m_created_at; }
    DateTime getClosedAt() const { return m_closed_at; }
    DateTime getUpdatedAt() const { return m_updated_at; }

    // Setters
    void setCaseProfileId(int case_profile_id) { m_case_profile_id = case_profile_id; }
    void setClientId(int client_id) { m_client_id = client_id; }
    void setAssessorId(int assessor_id) { m_assessor_id = assessor_id; }
    void setStatus(const string& status) { m_status = status; updateTimestamp(); }
    void setNotes(const string& notes) { 
        if (isValidNotes(notes)) {
            m_notes = notes; 
            updateTimestamp(); 
        } else {
            utils::logMessage("WARNING", "Notes truncated to " + to_string(MAX_NOTES_LENGTH) + " characters");
            m_notes = notes.substr(0, MAX_NOTES_LENGTH);
            updateTimestamp();
        }
    }
    void setCreatedAt(const DateTime& created_at) { m_created_at = created_at; }
    void setClosedAt(const DateTime& closed_at) { m_closed_at = closed_at; }
    void setUpdatedAt(const DateTime& updated_at) { m_updated_at = updated_at; }

    // Timestamp management
    void updateTimestamp() { m_updated_at = DateTime::now(); }
    void setCreationTimestamp() { m_created_at = DateTime::now(); }
    void setTimestamps() { DateTime now = DateTime::now(); m_created_at = now; m_updated_at = now; }
    
    // Status management
    // Flow: "Pending" -> "Active" -> "Closed"
    // New case profiles always start as "Pending"
    // Only assessor can change status manually
    bool isActive() const;
    bool isClosed() const;
    bool isPending() const;
    void activate();
    void close(const string& reason = "");
    void setPending();
    
    // Utility methods
    bool isValidClientId() const;
    bool isValidAssessorId() const;
    bool isValidNotes(const string& notes) const;
    void displayInfo() const;
    
    // Static methods for ID management
    static int getNextCaseProfileId();
    static void resetIdCounter();
    static const int ID_PREFIX = 400000; // CaseProfile IDs start at 400001
    static const int MAX_NOTES_LENGTH = 1500; // Maximum characters for notes
    
    // Stream operators
    friend ostream& operator<<(ostream& os, const CaseProfile& caseProfile);
    friend istream& operator>>(istream& is, CaseProfile& caseProfile);
  };
  
  // Stream operators declarations
  ostream& operator<<(ostream& os, const CaseProfile& caseProfile);
  istream& operator>>(istream& is, CaseProfile& caseProfile);
}

#endif // CASEPROFILE_H
