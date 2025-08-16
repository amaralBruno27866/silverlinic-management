#ifndef CASE_PROFILE_MANAGER_H
#define CASE_PROFILE_MANAGER_H

#include <vector>
#include <optional>
#include <memory>
#include <map>
#include <sqlite3.h>
#include "core/CaseProfile.h"
#include "core/Client.h"
#include "core/Assessor.h"

using namespace std;
using namespace SilverClinic;

namespace SilverClinic {
    
    /**
     * @brief Manager class for CaseProfile CRUD operations and workflow management
     * 
     * This class implements the Repository pattern for managing CaseProfile data
     * with special focus on relationship management and case lifecycle.
     * 
     * Key Features:
     * - Standard CRUD operations
     * - Workflow management (Pending -> Active -> Closed)
     * - Relationship validation (Client + Assessor)
     * - Business rules enforcement
     * - Advanced reporting and analytics
     */
    class CaseProfileManager {
    private:
        sqlite3* m_db;
        
        // Helper methods for database operations
        bool executeStatement(const string& sql, const string& operation) const;
        CaseProfile createCaseProfileFromRow(sqlite3_stmt* stmt) const;
        
    public:
        // Constructor and Destructor
        explicit CaseProfileManager(sqlite3* database);
        ~CaseProfileManager() = default;
        
        // Disable copy constructor and assignment operator
        CaseProfileManager(const CaseProfileManager&) = delete;
        CaseProfileManager& operator=(const CaseProfileManager&) = delete;
        
        // ========================================
        // CRUD Operations
        // ========================================
        
        /**
         * @brief Create a new case profile
         * @param caseProfile The case profile object to create
         * @return true if creation was successful, false otherwise
         */
        bool create(const CaseProfile& caseProfile);
        
        /**
         * @brief Retrieve all case profiles
         * @return Vector of all case profiles
         */
        vector<CaseProfile> readAll() const;
        
        /**
         * @brief Retrieve a specific case profile by ID
         * @param caseProfileId The ID of the case profile to retrieve
         * @return Optional containing the case profile if found, empty otherwise
         */
        optional<CaseProfile> readById(int caseProfileId) const;
        
        /**
         * @brief Update an existing case profile
         * @param caseProfile The case profile object with updated data
         * @return true if update was successful, false otherwise
         */
        bool update(const CaseProfile& caseProfile);
        
        /**
         * @brief Delete a case profile (soft delete - mark as cancelled)
         * @param caseProfileId The ID of the case profile to delete
         * @return true if deletion was successful, false otherwise
         */
        bool deleteById(int caseProfileId);
        
        // ========================================
        // Relationship-based Operations
        // ========================================
        
        /**
         * @brief Get all cases for a specific client
         * @param clientId The ID of the client
         * @return Vector of case profiles for the client
         */
        vector<CaseProfile> getCasesByClientId(int clientId) const;
        
        /**
         * @brief Get all cases assigned to a specific assessor
         * @param assessorId The ID of the assessor
         * @return Vector of case profiles for the assessor
         */
        vector<CaseProfile> getCasesByAssessorId(int assessorId) const;
        
        /**
         * @brief Get cases for a specific client-assessor pair
         * @param clientId The ID of the client
         * @param assessorId The ID of the assessor
         * @return Vector of case profiles for the pair
         */
        vector<CaseProfile> getCasesByClientAndAssessor(int clientId, int assessorId) const;
        
        // ========================================
        // Workflow Management
        // ========================================
        
        /**
         * @brief Activate a pending case profile
         * @param caseProfileId The ID of the case to activate
         * @param assessorId The ID of the assessor performing the action
         * @return true if activation was successful, false otherwise
         */
        bool activateCase(int caseProfileId, int assessorId);
        
        /**
         * @brief Close an active case profile
         * @param caseProfileId The ID of the case to close
         * @param assessorId The ID of the assessor performing the action
         * @param reason Optional reason for closing
         * @return true if closure was successful, false otherwise
         */
        bool closeCase(int caseProfileId, int assessorId, const string& reason = "");
        
        /**
         * @brief Reopen a closed case (if business rules allow)
         * @param caseProfileId The ID of the case to reopen
         * @param assessorId The ID of the assessor performing the action
         * @return true if reopening was successful, false otherwise
         */
        bool reopenCase(int caseProfileId, int assessorId);
        
        /**
         * @brief Transfer a case to another assessor
         * @param caseProfileId The ID of the case to transfer
         * @param newAssessorId The ID of the new assessor
         * @param currentAssessorId The ID of the current assessor (for validation)
         * @return true if transfer was successful, false otherwise
         */
        bool transferCase(int caseProfileId, int newAssessorId, int currentAssessorId);
        
        // ========================================
        // Search and Filter Operations
        // ========================================
        
        /**
         * @brief Get cases by status
         * @param status The status to filter by ("Pending", "Active", "Closed", "Cancelled")
         * @return Vector of case profiles with the specified status
         */
        vector<CaseProfile> getCasesByStatus(const string& status) const;
        
        /**
         * @brief Get cases created within a date range
         * @param startDate Start date (YYYY-MM-DD format)
         * @param endDate End date (YYYY-MM-DD format)
         * @return Vector of case profiles created in the date range
         */
        vector<CaseProfile> getCasesByDateRange(const string& startDate, const string& endDate) const;
        
        /**
         * @brief Search cases by notes content
         * @param searchTerm The term to search for in notes
         * @return Vector of matching case profiles
         */
        vector<CaseProfile> searchByNotes(const string& searchTerm) const;
        
        /**
         * @brief Get overdue cases (active for more than specified days)
         * @param daysSinceCreation Number of days to consider overdue
         * @return Vector of overdue case profiles
         */
        vector<CaseProfile> getOverdueCases(int daysSinceCreation = 30) const;
        
        // ========================================
        // Analytics and Reporting
        // ========================================
        
        /**
         * @brief Get case count by status
         * @return Map of status to count
         */
        map<string, int> getCaseCountByStatus() const;
        
        /**
         * @brief Get assessor workload (number of active cases)
         * @param assessorId The ID of the assessor
         * @return Number of active cases assigned to the assessor
         */
        int getAssessorWorkload(int assessorId) const;
        
        /**
         * @brief Get average case duration for closed cases
         * @return Average duration in days
         */
        double getAverageCaseDuration() const;
        
        /**
         * @brief Get most active assessor (by number of cases)
         * @return Assessor ID with most cases, or -1 if none found
         */
        int getMostActiveAssessor() const;
        
        // ========================================
        // Utility and Validation Methods
        // ========================================
        
        /**
         * @brief Check if a case profile exists
         * @param caseProfileId The ID to check
         * @return true if case profile exists, false otherwise
         */
        bool exists(int caseProfileId) const;
        
        /**
         * @brief Get total count of case profiles
         * @return Number of case profiles in the database
         */
        int getCount() const;
        
        /**
         * @brief Validate case profile data before database operations
         * @param caseProfile The case profile to validate
         * @return true if valid, false otherwise
         */
        bool validateCaseProfile(const CaseProfile& caseProfile) const;
        
        /**
         * @brief Check if deletion is safe (business rules)
         * @param caseProfileId The ID of the case profile to check
         * @return true if safe to delete, false otherwise
         */
        bool canDelete(int caseProfileId) const;
        
        /**
         * @brief Validate if assessor can perform action on case
         * @param caseProfileId The ID of the case
         * @param assessorId The ID of the assessor
         * @return true if authorized, false otherwise
         */
        bool validateAssessorPermission(int caseProfileId, int assessorId) const;
        
        /**
         * @brief Check if client and assessor exist in database
         * @param clientId The ID of the client
         * @param assessorId The ID of the assessor
         * @return true if both exist, false otherwise
         */
        bool validateRelationship(int clientId, int assessorId) const;
        
        /**
         * @brief Get cases with pagination support
         * @param limit Maximum number of cases to return
         * @param offset Number of cases to skip
         * @return Vector of cases for the requested page
         */
        vector<CaseProfile> readWithPagination(int limit, int offset = 0) const;
        
        // ========================================
        // PDF Export and Reporting
        // ========================================
        
        /**
         * @brief Generate PDF report for a case profile
         * @param caseProfileId The ID of the case to export
         * @param outputPath The path where PDF should be saved
         * @param reportType Type of report ("summary", "detailed", "clinical")
         * @return true if PDF was generated successfully, false otherwise
         */
        bool generatePDFReport(int caseProfileId, const string& outputPath, const string& reportType = "detailed") const;
        
        /**
         * @brief Generate bulk PDF reports for multiple cases
         * @param caseProfileIds Vector of case IDs to export
         * @param outputDirectory Directory where PDFs should be saved
         * @param reportType Type of report for all cases
         * @return Number of PDFs successfully generated
         */
        int generateBulkPDFReports(const vector<int>& caseProfileIds, const string& outputDirectory, const string& reportType = "summary") const;
        
        /**
         * @brief Generate PDF with custom template and data
         * @param caseProfileId The ID of the case to export
         * @param outputPath The path where PDF should be saved
         * @param includeTimeline Include complete case timeline
         * @param includeFullNotes Include all case notes
         * @param watermarkText Optional watermark text
         * @return true if PDF was generated successfully
         */
        bool generateCustomPDFReport(int caseProfileId, const string& outputPath, 
                                    bool includeTimeline = true, 
                                    bool includeFullNotes = true, 
                                    const string& watermarkText = "") const;

    /**
     * @brief Import case profiles from a CSV (generated by form)
     * Expected headers: client_id, assessor_id, status, notes, created_at, closed_at(optional)
     * @param filePath CSV file path
     * @return number of successfully imported case profiles
     */
    int importFromCSV(const string& filePath);
    // Service layer support: expose raw db handle (read-only usage in services)
    sqlite3* getDb() const { return m_db; }
        
    private:
        // Internal helper methods
        void logDatabaseError(const string& operation) const;
        bool updateCaseStatus(int caseProfileId, const string& newStatus, const string& reason = "");
        string getCurrentTimestamp() const;
        
        // PDF Generation helper methods
        bool generatePDFHeader(void* pdf, const string& reportType) const;
        bool generatePDFCaseInfo(void* pdf, const CaseProfile& caseProfile) const;
        bool generatePDFClientInfo(void* pdf, int clientId) const;
        bool generatePDFAssessorInfo(void* pdf, int assessorId) const;
        bool generatePDFTimeline(void* pdf, int caseProfileId) const;
        bool generatePDFNotes(void* pdf, const string& notes) const;
        bool generatePDFFooter(void* pdf) const;
        string formatDateForPDF(const string& isoDate) const;
        string getStatusIcon(const string& status) const;

    };
    
} // namespace SilverClinic

#endif // CASE_PROFILE_MANAGER_H
