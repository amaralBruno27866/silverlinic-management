#include "forms/ActivitiesOfDailyLiving.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static member initialization
        const string ActivitiesOfDailyLiving::FORM_TYPE = "ADL";
        int ActivitiesOfDailyLiving::adl_id_counter = ID_PREFIX;

        // Standard categories based on the HTML form
        const vector<string> ActivitiesOfDailyLiving::STANDARD_CATEGORIES = {
            "functional_tasks",
            "household_chores", 
            "outdoor_chores",
            "personal_care",
            "child_care"
        };

        // Standard activities for each category based on the HTML form
        const map<string, vector<string>> ActivitiesOfDailyLiving::STANDARD_ACTIVITIES = {
            {"functional_tasks", {
                "lifting", "bending", "carrying", "sitting", "standing", "kneeling",
                "squatting", "gripping", "balancing", "walking_t", "reaching_overhead", "stairs"
            }},
            {"household_chores", {
                "bathtub", "dusting", "cooking", "vacuuming", "sweeping", "w_dishes",
                "m_bed", "b_sheets", "l_u_dishes", "laundry", "t_garbage", "grocery"
            }},
            {"outdoor_chores", {
                "mowing_lawn", "snow_removal", "gardening"
            }},
            {"personal_care", {
                "dressing", "bathing", "combing", "shaving", "nail", "cream", "shoes"
            }},
            {"child_care", {
                "tk_children", "bt_children", "pl_children"
            }}
        };

        // ActivityCategory JSON methods
        string ActivityCategory::toJson() const {
            stringstream json;
            json << "{";
            json << "\"category_name\":\"" << category_name << "\",";
            json << "\"activities\":{";
            
            bool first = true;
            for (const auto& [activity, hasDifficulty] : activities) {
                if (!first) json << ",";
                json << "\"" << activity << "\":" << (hasDifficulty ? "true" : "false");
                first = false;
            }
            
            json << "},";
            json << "\"comments\":\"" << comments << "\"";
            json << "}";
            return json.str();
        }

        void ActivityCategory::fromJson(const string& json) {
            // Simple JSON parsing - in production, consider using a JSON library
            activities.clear();
            
            // Extract category_name
            size_t pos = json.find("\"category_name\":\"");
            if (pos != string::npos) {
                pos += 17; // length of "\"category_name\":\""
                size_t end = json.find("\"", pos);
                if (end != string::npos) {
                    category_name = json.substr(pos, end - pos);
                }
            }
            
            // Extract activities object
            pos = json.find("\"activities\":{");
            if (pos != string::npos) {
                pos += 14; // length of "\"activities\":{"
                size_t end = json.find("}", pos);
                if (end != string::npos) {
                    string activitiesStr = json.substr(pos, end - pos);
                    
                    // Parse activities
                    size_t actPos = 0;
                    while (actPos < activitiesStr.length()) {
                        size_t keyStart = activitiesStr.find("\"", actPos);
                        if (keyStart == string::npos) break;
                        keyStart++;
                        
                        size_t keyEnd = activitiesStr.find("\"", keyStart);
                        if (keyEnd == string::npos) break;
                        
                        string activityName = activitiesStr.substr(keyStart, keyEnd - keyStart);
                        
                        size_t valuePos = activitiesStr.find(":", keyEnd);
                        if (valuePos == string::npos) break;
                        valuePos++;
                        
                        size_t valueEnd = activitiesStr.find(",", valuePos);
                        if (valueEnd == string::npos) valueEnd = activitiesStr.length();
                        
                        string valueStr = activitiesStr.substr(valuePos, valueEnd - valuePos);
                        bool hasDifficulty = (valueStr.find("true") != string::npos);
                        
                        activities[activityName] = hasDifficulty;
                        actPos = valueEnd + 1;
                    }
                }
            }
            
            // Extract comments
            pos = json.find("\"comments\":\"");
            if (pos != string::npos) {
                pos += 12; // length of "\"comments\":\""
                size_t end = json.find("\"", pos);
                if (end != string::npos) {
                    comments = json.substr(pos, end - pos);
                }
            }
        }

        int ActivityCategory::getTotalDifficulties() const {
            int count = 0;
            for (const auto& [activity, hasDifficulty] : activities) {
                if (hasDifficulty) count++;
            }
            return count;
        }

        vector<string> ActivityCategory::getDifficultActivities() const {
            vector<string> result;
            for (const auto& [activity, hasDifficulty] : activities) {
                if (hasDifficulty) {
                    result.push_back(activity);
                }
            }
            return result;
        }

        bool ActivityCategory::hasDifficulties() const {
            for (const auto& [activity, hasDifficulty] : activities) {
                if (hasDifficulty) return true;
            }
            return false;
        }

        // Constructors
        ActivitiesOfDailyLiving::ActivitiesOfDailyLiving() 
            : m_adl_id(getNextADLId()), m_case_profile_id(0), m_type(FORM_TYPE),
              m_activities_data_json("{}") {
            setTimestamps();
            utils::logMessage("INFO", "ActivitiesOfDailyLiving created with ID: " + to_string(m_adl_id));
        }

        ActivitiesOfDailyLiving::ActivitiesOfDailyLiving(int case_profile_id) 
            : m_adl_id(getNextADLId()), m_case_profile_id(case_profile_id), 
              m_type(FORM_TYPE), m_activities_data_json("{}") {
            if (!isValidCaseProfileId(case_profile_id)) {
                throw invalid_argument("Invalid case profile ID provided");
            }
            setTimestamps();
            utils::logMessage("INFO", "ActivitiesOfDailyLiving created with ID: " + to_string(m_adl_id) + " for case: " + to_string(case_profile_id));
        }

        ActivitiesOfDailyLiving::ActivitiesOfDailyLiving(int adl_id, int case_profile_id, 
                                                       const string& activitiesDataJson,
                                                       const DateTime& createdAt, const DateTime& updatedAt)
            : m_adl_id(adl_id), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_activities_data_json(activitiesDataJson),
              m_adl_createdAt(createdAt), m_adl_updatedAt(updatedAt) {
            
            if (!isValidCaseProfileId(case_profile_id)) {
                throw invalid_argument("Invalid case profile ID provided");
            }
            
            // Parse JSON to C++ map
            syncJsonToCppData();
            
            utils::logMessage("INFO", "ActivitiesOfDailyLiving loaded from database with ID: " + to_string(m_adl_id));
        }

        // Private helper methods
        void ActivitiesOfDailyLiving::setTimestamps() {
            m_adl_createdAt = DateTime();
            m_adl_updatedAt = DateTime();
        }

        void ActivitiesOfDailyLiving::updateTimestamp() {
            m_adl_updatedAt = DateTime();
        }

        int ActivitiesOfDailyLiving::getNextADLId() {
            return ++adl_id_counter;
        }

        void ActivitiesOfDailyLiving::syncJsonToCppData() {
            // Simple JSON parsing to populate m_activities_data map
            m_activities_data.clear();
            
            if (m_activities_data_json.empty() || m_activities_data_json == "{}") {
                return;
            }
            
            // Improved parsing for multiple categories
            size_t pos = 1; // Skip the opening brace
            while (pos < m_activities_data_json.length()) {
                // Find the key (category name)
                size_t key_start = m_activities_data_json.find("\"", pos);
                if (key_start == string::npos) break;
                key_start++;
                
                size_t key_end = m_activities_data_json.find("\"", key_start);
                if (key_end == string::npos) break;
                
                string key = m_activities_data_json.substr(key_start, key_end - key_start);
                
                // Find the value start (opening brace of the category object)
                size_t value_start = m_activities_data_json.find("{", key_end);
                if (value_start == string::npos) break;
                
                // Find the matching closing brace for this category object
                size_t value_end = value_start + 1;
                int brace_count = 1;
                while (value_end < m_activities_data_json.length() && brace_count > 0) {
                    if (m_activities_data_json[value_end] == '{') {
                        brace_count++;
                    } else if (m_activities_data_json[value_end] == '}') {
                        brace_count--;
                    }
                    value_end++;
                }
                
                if (brace_count != 0) break; // Malformed JSON
                
                string value = m_activities_data_json.substr(value_start, value_end - value_start);
                
                ActivityCategory category;
                category.fromJson(value);
                m_activities_data[key] = category;
                
                // Move to next category
                pos = value_end;
                // Skip comma if present
                size_t comma_pos = m_activities_data_json.find(",", pos);
                if (comma_pos != string::npos && comma_pos < m_activities_data_json.find("}", pos)) {
                    pos = comma_pos + 1;
                } else {
                    break; // No more categories
                }
            }
        }

        void ActivitiesOfDailyLiving::syncCppToJsonData() {
            if (m_activities_data.empty()) {
                m_activities_data_json = "{}";
                return;
            }
            
            stringstream json;
            json << "{";
            
            bool first = true;
            for (const auto& [key, category] : m_activities_data) {
                if (!first) json << ",";
                json << "\"" << key << "\":" << category.toJson();
                first = false;
            }
            
            json << "}";
            m_activities_data_json = json.str();
        }

        // JSON access methods
        void ActivitiesOfDailyLiving::setActivitiesDataJson(const string& json) {
            m_activities_data_json = json;
            syncJsonToCppData();
            updateTimestamp();
        }

        // Activity data manipulation
        void ActivitiesOfDailyLiving::setActivityDifficulty(const string& category, const string& activity, bool hasDifficulty) {
            if (!isValidCategory(category)) {
                throw invalid_argument("Invalid category: " + category);
            }
            if (!isValidActivity(category, activity)) {
                throw invalid_argument("Invalid activity '" + activity + "' for category '" + category + "'");
            }
            
            // Initialize category if it doesn't exist
            if (m_activities_data.find(category) == m_activities_data.end()) {
                m_activities_data[category] = ActivityCategory(category);
            }
            
            m_activities_data[category].activities[activity] = hasDifficulty;
            syncCppToJsonData();
            updateTimestamp();
        }

        void ActivitiesOfDailyLiving::setCategoryComments(const string& category, const string& comments) {
            if (!isValidCategory(category)) {
                throw invalid_argument("Invalid category: " + category);
            }
            if (!isValidComments(comments)) {
                throw invalid_argument("Comments too long (max 1000 characters)");
            }
            
            // Initialize category if it doesn't exist
            if (m_activities_data.find(category) == m_activities_data.end()) {
                m_activities_data[category] = ActivityCategory(category);
            }
            
            m_activities_data[category].comments = comments;
            syncCppToJsonData();
            updateTimestamp();
        }

        void ActivitiesOfDailyLiving::setCategoryData(const ActivityCategory& categoryData) {
            if (!isValidCategory(categoryData.category_name)) {
                throw invalid_argument("Invalid category: " + categoryData.category_name);
            }
            if (!isValidComments(categoryData.comments)) {
                throw invalid_argument("Comments too long (max 1000 characters)");
            }
            
            m_activities_data[categoryData.category_name] = categoryData;
            syncCppToJsonData();
            updateTimestamp();
        }

        void ActivitiesOfDailyLiving::removeCategoryData(const string& category) {
            auto it = m_activities_data.find(category);
            if (it != m_activities_data.end()) {
                m_activities_data.erase(it);
                syncCppToJsonData();
                updateTimestamp();
            }
        }

        void ActivitiesOfDailyLiving::clearAllActivityData() {
            m_activities_data.clear();
            m_activities_data_json = "{}";
            updateTimestamp();
        }

        // Individual category access
        ActivityCategory ActivitiesOfDailyLiving::getCategoryData(const string& category) const {
            auto it = m_activities_data.find(category);
            if (it != m_activities_data.end()) {
                return it->second;
            }
            return ActivityCategory(category); // Return empty category data if not found
        }

        bool ActivitiesOfDailyLiving::hasCategoryData(const string& category) const {
            return m_activities_data.find(category) != m_activities_data.end();
        }

        // Validation methods
        bool ActivitiesOfDailyLiving::isValidCaseProfileId(int case_profile_id) const {
            return case_profile_id > 0 && case_profile_id >= CaseProfile::ID_PREFIX;
        }

        bool ActivitiesOfDailyLiving::isValidCategory(const string& category) const {
            return find(STANDARD_CATEGORIES.begin(), STANDARD_CATEGORIES.end(), category) != STANDARD_CATEGORIES.end();
        }

        bool ActivitiesOfDailyLiving::isValidActivity(const string& category, const string& activity) const {
            auto it = STANDARD_ACTIVITIES.find(category);
            if (it == STANDARD_ACTIVITIES.end()) return false;
            
            const vector<string>& activities = it->second;
            return find(activities.begin(), activities.end(), activity) != activities.end();
        }

        bool ActivitiesOfDailyLiving::isValidComments(const string& comments) const {
            return comments.length() <= 1000;
        }

        bool ActivitiesOfDailyLiving::isValidData() const {
            if (!isValidCaseProfileId(m_case_profile_id)) return false;
            
            for (const auto& [key, category] : m_activities_data) {
                if (!isValidCategory(category.category_name)) return false;
                if (!isValidComments(category.comments)) return false;
                
                for (const auto& [activity, hasDifficulty] : category.activities) {
                    if (!isValidActivity(category.category_name, activity)) return false;
                }
            }
            
            return true;
        }

        // Analysis methods
        int ActivitiesOfDailyLiving::getTotalDifficulties() const {
            int total = 0;
            for (const auto& [key, category] : m_activities_data) {
                total += category.getTotalDifficulties();
            }
            return total;
        }

        int ActivitiesOfDailyLiving::getDifficultiesInCategory(const string& category) const {
            auto it = m_activities_data.find(category);
            if (it != m_activities_data.end()) {
                return it->second.getTotalDifficulties();
            }
            return 0;
        }

        vector<string> ActivitiesOfDailyLiving::getMostProblematicCategories() const {
            vector<pair<string, int>> categoryDifficulties;
            
            for (const auto& [key, category] : m_activities_data) {
                int difficulties = category.getTotalDifficulties();
                if (difficulties > 0) {
                    categoryDifficulties.push_back({key, difficulties});
                }
            }
            
            // Sort by number of difficulties (descending)
            sort(categoryDifficulties.begin(), categoryDifficulties.end(),
                 [](const pair<string, int>& a, const pair<string, int>& b) {
                     return a.second > b.second;
                 });
            
            vector<string> result;
            for (const auto& [category, difficulties] : categoryDifficulties) {
                result.push_back(category);
            }
            
            return result;
        }

        vector<string> ActivitiesOfDailyLiving::getDifficultActivitiesInCategory(const string& category) const {
            auto it = m_activities_data.find(category);
            if (it != m_activities_data.end()) {
                return it->second.getDifficultActivities();
            }
            return vector<string>();
        }

        vector<string> ActivitiesOfDailyLiving::getAllDifficultActivities() const {
            vector<string> result;
            for (const auto& [key, category] : m_activities_data) {
                vector<string> categoryActivities = category.getDifficultActivities();
                result.insert(result.end(), categoryActivities.begin(), categoryActivities.end());
            }
            return result;
        }

        double ActivitiesOfDailyLiving::getIndependencePercentage() const {
            int totalActivities = 0;
            int totalDifficulties = 0;
            
            for (const auto& [categoryName, activities] : STANDARD_ACTIVITIES) {
                totalActivities += activities.size();
            }
            
            totalDifficulties = getTotalDifficulties();
            
            if (totalActivities == 0) return 100.0;
            
            return ((totalActivities - totalDifficulties) * 100.0) / totalActivities;
        }

        // Clinical analysis methods
        bool ActivitiesOfDailyLiving::needsAssistanceWithPersonalCare() const {
            return getDifficultiesInCategory("personal_care") > 0;
        }

        bool ActivitiesOfDailyLiving::needsAssistanceWithHousework() const {
            return getDifficultiesInCategory("household_chores") > 0 || 
                   getDifficultiesInCategory("outdoor_chores") > 0;
        }

        bool ActivitiesOfDailyLiving::needsAssistanceWithChildCare() const {
            return getDifficultiesInCategory("child_care") > 0;
        }

        bool ActivitiesOfDailyLiving::canLiveIndependently() const {
            // Can live independently if they have minimal difficulties with personal care
            // and functional tasks
            int personalCareDifficulties = getDifficultiesInCategory("personal_care");
            int functionalDifficulties = getDifficultiesInCategory("functional_tasks");
            
            return personalCareDifficulties <= 1 && functionalDifficulties <= 2;
        }

        string ActivitiesOfDailyLiving::getIndependenceLevel() const {
            double independencePercentage = getIndependencePercentage();
            
            if (independencePercentage >= 90.0) {
                return "Independent";
            } else if (independencePercentage >= 70.0) {
                return "Minimal Assistance";
            } else if (independencePercentage >= 50.0) {
                return "Moderate Assistance";
            } else {
                return "Maximum Assistance";
            }
        }

        // Display methods
        void ActivitiesOfDailyLiving::displayInfo() const {
            cout << "\n=== Activities of Daily Living Information ===" << endl;
            cout << "ADL ID: " << m_adl_id << endl;
            cout << "Case Profile ID: " << m_case_profile_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Created: " << m_adl_createdAt.toString() << endl;
            cout << "Updated: " << m_adl_updatedAt.toString() << endl;
            cout << "Total Difficulties: " << getTotalDifficulties() << endl;
            cout << "Independence Level: " << getIndependenceLevel() << endl;
            cout << "Independence Percentage: " << fixed << setprecision(1) << getIndependencePercentage() << "%" << endl;
        }

        void ActivitiesOfDailyLiving::displaySummary() const {
            cout << "\n=== ADL Summary ===" << endl;
            cout << "ADL ID: " << m_adl_id << " | Case: " << m_case_profile_id << endl;
            cout << "Total Difficulties: " << getTotalDifficulties() << " | Independence: " << getIndependenceLevel() << endl;
            
            auto problematicCategories = getMostProblematicCategories();
            if (!problematicCategories.empty()) {
                cout << "Most Problematic Categories: ";
                for (size_t i = 0; i < min(problematicCategories.size(), size_t(3)); ++i) {
                    cout << problematicCategories[i];
                    if (i < min(problematicCategories.size(), size_t(3)) - 1) cout << ", ";
                }
                cout << endl;
            }
        }

        void ActivitiesOfDailyLiving::displayCategoryAnalysis() const {
            cout << "\n=== Category Analysis ===" << endl;
            
            for (const string& category : STANDARD_CATEGORIES) {
                int difficulties = getDifficultiesInCategory(category);
                auto it = STANDARD_ACTIVITIES.find(category);
                int totalActivities = (it != STANDARD_ACTIVITIES.end()) ? it->second.size() : 0;
                
                cout << "\n" << category << ":" << endl;
                cout << "  Difficulties: " << difficulties << "/" << totalActivities << endl;
                
                if (difficulties > 0) {
                    vector<string> difficultActivities = getDifficultActivitiesInCategory(category);
                    cout << "  Difficult Activities: ";
                    for (size_t i = 0; i < difficultActivities.size(); ++i) {
                        cout << difficultActivities[i];
                        if (i < difficultActivities.size() - 1) cout << ", ";
                    }
                    cout << endl;
                }
                
                // Show comments if available
                auto categoryData = getCategoryData(category);
                if (!categoryData.comments.empty()) {
                    cout << "  Comments: " << categoryData.comments << endl;
                }
            }
        }

        void ActivitiesOfDailyLiving::displayIndependenceAssessment() const {
            cout << "\n=== Independence Assessment ===" << endl;
            cout << "Overall Independence Level: " << getIndependenceLevel() << endl;
            cout << "Independence Percentage: " << fixed << setprecision(1) << getIndependencePercentage() << "%" << endl;
            
            cout << "\nSpecific Assistance Needs:" << endl;
            cout << "  Personal Care: " << (needsAssistanceWithPersonalCare() ? "Assistance Required" : "Independent") << endl;
            cout << "  Housework: " << (needsAssistanceWithHousework() ? "Assistance Required" : "Independent") << endl;
            cout << "  Child Care: " << (needsAssistanceWithChildCare() ? "Assistance Required" : "Independent") << endl;
            cout << "  Can Live Independently: " << (canLiveIndependently() ? "Yes" : "No") << endl;
        }

        string ActivitiesOfDailyLiving::toString() const {
            stringstream ss;
            ss << "ActivitiesOfDailyLiving[ID=" << m_adl_id 
               << ", CaseProfile=" << m_case_profile_id
               << ", Type=" << m_type
               << ", TotalDifficulties=" << getTotalDifficulties()
               << ", Independence=" << getIndependenceLevel()
               << ", IndependencePercentage=" << fixed << setprecision(1) << getIndependencePercentage() << "%"
               << ", Created=" << m_adl_createdAt.toString()
               << "]";
            return ss.str();
        }

        // Static methods
        void ActivitiesOfDailyLiving::resetIdCounter() {
            adl_id_counter = ID_PREFIX;
        }

        int ActivitiesOfDailyLiving::getNextId() {
            return adl_id_counter + 1;
        }

        bool ActivitiesOfDailyLiving::isValidCategoryName(const string& category) {
            return find(STANDARD_CATEGORIES.begin(), STANDARD_CATEGORIES.end(), category) != STANDARD_CATEGORIES.end();
        }

        bool ActivitiesOfDailyLiving::isValidActivityName(const string& category, const string& activity) {
            auto it = STANDARD_ACTIVITIES.find(category);
            if (it == STANDARD_ACTIVITIES.end()) return false;
            
            const vector<string>& activities = it->second;
            return find(activities.begin(), activities.end(), activity) != activities.end();
        }

        vector<string> ActivitiesOfDailyLiving::getStandardCategories() {
            return STANDARD_CATEGORIES;
        }

        vector<string> ActivitiesOfDailyLiving::getStandardActivitiesForCategory(const string& category) {
            auto it = STANDARD_ACTIVITIES.find(category);
            if (it != STANDARD_ACTIVITIES.end()) {
                return it->second;
            }
            return vector<string>();
        }

        // Stream operators
        ostream& operator<<(ostream& os, const ActivitiesOfDailyLiving& adl) {
            os << adl.toString();
            return os;
        }

        istream& operator>>(istream& is, ActivitiesOfDailyLiving& adl) {
            // Basic implementation for reading from stream
            string line;
            getline(is, line);
            // Parse line and populate adl object
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
