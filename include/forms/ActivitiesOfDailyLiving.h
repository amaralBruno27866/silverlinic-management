#ifndef ACTIVITIESOFDAILYLIVING_H
#define ACTIVITIESOFDAILYLIVING_H

#include <string>
#include <map>
#include <vector>
#include "core/DateTime.h"
#include "core/Utils.h"
#include "core/CaseProfile.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        /**
         * @brief Structure representing a category of activities with difficulty status
         */
        struct ActivityCategory {
            string category_name;               // Name of the category (e.g., "functional_tasks")
            map<string, bool> activities;       // Activity name -> has difficulty (true/false)
            string comments;                    // Comments for this category (max 1000 characters)
            
            ActivityCategory() : category_name(""), comments("") {}
            
            ActivityCategory(const string& name, const string& comment = "")
                : category_name(name), comments(comment) {}

            // JSON conversion methods
            string toJson() const;
            void fromJson(const string& json);
            
            // Helper methods
            int getTotalDifficulties() const;
            vector<string> getDifficultActivities() const;
            bool hasDifficulties() const;
        };

        /**
         * @brief Activities of Daily Living form using HYBRID approach (C++ + JSON)
         * 
         * This class follows the Beck Inventory pattern for basic fields,
         * but uses JSON for complex activity data storage in SQLite.
         */
        class ActivitiesOfDailyLiving {
        private:
            static const string FORM_TYPE;     // Form type identifier: "ADL"
            
            // Primary attributes (traditional C++ approach)
            int m_adl_id;                      // ADL ID (1100000+)
            int m_case_profile_id;             // Foreign key to case_profile
            string m_type;                     // Form type (default: "ADL")
            
            // JSON field for complex data (stored as TEXT in SQLite)
            string m_activities_data_json;     // JSON string containing all activity data
            
            // Internal C++ representation (for easy manipulation)
            map<string, ActivityCategory> m_activities_data;  // Parsed from JSON for runtime use
            
            DateTime m_adl_createdAt;          // Creation timestamp
            DateTime m_adl_updatedAt;          // Last update timestamp
            
            static int adl_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextADLId();
            
            // JSON synchronization methods
            void syncJsonToCppData();          // Parse JSON to C++ map
            void syncCppToJsonData();          // Serialize C++ map to JSON

        public:
            // Standard activity categories and their activities
            static const vector<string> STANDARD_CATEGORIES;
            static const map<string, vector<string>> STANDARD_ACTIVITIES;
            
            // Constructors
            ActivitiesOfDailyLiving();
            ActivitiesOfDailyLiving(int case_profile_id);
            ActivitiesOfDailyLiving(int adl_id, int case_profile_id, 
                                   const string& activitiesDataJson,
                                   const DateTime& createdAt, const DateTime& updatedAt);
            
            // Destructor
            ~ActivitiesOfDailyLiving() = default;
            
            // Getters - Basic Info
            int getADLId() const { return m_adl_id; }
            int getCaseProfileId() const { return m_case_profile_id; }
            string getType() const { return m_type; }
            DateTime getADLCreatedAt() const { return m_adl_createdAt; }
            DateTime getADLUpdatedAt() const { return m_adl_updatedAt; }
            
            // JSON access (for database storage)
            string getActivitiesDataJson() const { return m_activities_data_json; }
            void setActivitiesDataJson(const string& json);
            
            // C++ map access (for runtime manipulation)
            map<string, ActivityCategory> getActivitiesDataMap() const { return m_activities_data; }
            
            // Individual category access
            ActivityCategory getCategoryData(const string& category) const;
            bool hasCategoryData(const string& category) const;
            map<string, ActivityCategory> getAllActivitiesData() const { return m_activities_data; }
            
            // Setters with validation - Basic Info
            void setCaseProfileId(int case_profile_id) { 
                m_case_profile_id = case_profile_id; 
                updateTimestamp(); 
            }
            void setADLCreatedAt(const DateTime& createdAt) { m_adl_createdAt = createdAt; }
            void setADLUpdatedAt(const DateTime& updatedAt) { m_adl_updatedAt = updatedAt; }
            
            // Activity data manipulation
            void setActivityDifficulty(const string& category, const string& activity, bool hasDifficulty);
            void setCategoryComments(const string& category, const string& comments);
            void setCategoryData(const ActivityCategory& categoryData);  // Uses categoryData.category_name as key
            void removeCategoryData(const string& category);
            void clearAllActivityData();
            
            // Validation methods
            bool isValidCaseProfileId(int case_profile_id) const;
            bool isValidCategory(const string& category) const;
            bool isValidActivity(const string& category, const string& activity) const;
            bool isValidComments(const string& comments) const;
            bool isValidData() const;
            
            // Analysis methods
            int getTotalDifficulties() const;
            int getDifficultiesInCategory(const string& category) const;
            vector<string> getMostProblematicCategories() const;
            vector<string> getDifficultActivitiesInCategory(const string& category) const;
            vector<string> getAllDifficultActivities() const;
            double getIndependencePercentage() const;
            
            // Clinical analysis methods
            bool needsAssistanceWithPersonalCare() const;
            bool needsAssistanceWithHousework() const;
            bool needsAssistanceWithChildCare() const;
            bool canLiveIndependently() const;
            string getIndependenceLevel() const;  // "Independent", "Minimal Assistance", "Moderate Assistance", "Maximum Assistance"
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            void displayCategoryAnalysis() const;
            void displayIndependenceAssessment() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            static const int ID_PREFIX = 1100000; // ADL IDs start at 1100001
            
            // Static validation methods
            static bool isValidCategoryName(const string& category);
            static bool isValidActivityName(const string& category, const string& activity);
            static vector<string> getStandardCategories();
            static vector<string> getStandardActivitiesForCategory(const string& category);
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const ActivitiesOfDailyLiving& adl);
            friend std::istream& operator>>(std::istream& is, ActivitiesOfDailyLiving& adl);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // ACTIVITIESOFDAILYLIVING_H
