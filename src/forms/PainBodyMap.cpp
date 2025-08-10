#include "forms/PainBodyMap.h"
#include "core/CaseProfile.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static member initialization
        const string PainBodyMap::FORM_TYPE = "PBM";
        int PainBodyMap::pbm_id_counter = ID_PREFIX;

        // Standard body parts list based on the HTML form
        const vector<string> PainBodyMap::STANDARD_BODY_PARTS = {
            "head", "headache", "jaw", "shoulders", "shoulder_blades",
            "arms", "elbows", "wrists", "hands", "chest", "abdomen",
            "buttocks", "groin", "hips", "upper_back", "middle_back",
            "low_back", "pain_radiates_from_low_back", "thighs", "legs",
            "knees", "ankles", "feet", "toes"
        };

        // BodyPartPain JSON methods
        string BodyPartPain::toJson() const {
            stringstream json;
            json << "{";
            json << "\"body_part\":\"" << body_part << "\",";
            json << "\"side_left\":" << (side_left ? "true" : "false") << ",";
            json << "\"side_right\":" << (side_right ? "true" : "false") << ",";
            json << "\"pain_level\":" << pain_level << ",";
            json << "\"comments\":\"" << comments << "\"";
            json << "}";
            return json.str();
        }

        void BodyPartPain::fromJson(const string& json) {
            // Simple JSON parsing - in production, consider using a JSON library
            // This is a basic implementation for demonstration
            size_t pos;
            
            // Extract body_part
            pos = json.find("\"body_part\":\"");
            if (pos != string::npos) {
                pos += 13; // length of "\"body_part\":\""
                size_t end = json.find("\"", pos);
                if (end != string::npos) {
                    body_part = json.substr(pos, end - pos);
                }
            }
            
            // Extract side_left
            pos = json.find("\"side_left\":");
            if (pos != string::npos) {
                pos += 12; // length of "\"side_left\":"
                side_left = (json.substr(pos, 4) == "true");
            }
            
            // Extract side_right
            pos = json.find("\"side_right\":");
            if (pos != string::npos) {
                pos += 13; // length of "\"side_right\":"
                side_right = (json.substr(pos, 4) == "true");
            }
            
            // Extract pain_level
            pos = json.find("\"pain_level\":");
            if (pos != string::npos) {
                pos += 13; // length of "\"pain_level\":"
                size_t end = json.find(",", pos);
                if (end == string::npos) end = json.find("}", pos);
                if (end != string::npos) {
                    pain_level = stoi(json.substr(pos, end - pos));
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

        // Constructors
        PainBodyMap::PainBodyMap() : m_pbm_id(getNextPBMId()), m_case_profile_id(0), m_type(FORM_TYPE),
                                   m_pain_data_json("{}"), m_additional_comments("") {
            setTimestamps();
            utils::logMessage("INFO", "PainBodyMap created with ID: " + to_string(m_pbm_id));
        }

        PainBodyMap::PainBodyMap(int case_profile_id) : m_pbm_id(getNextPBMId()), m_case_profile_id(case_profile_id), 
                                                       m_type(FORM_TYPE), m_pain_data_json("{}"), m_additional_comments("") {
            if (!isValidCaseProfileId(case_profile_id)) {
                throw invalid_argument("Invalid case profile ID provided");
            }
            setTimestamps();
            utils::logMessage("INFO", "PainBodyMap created with ID: " + to_string(m_pbm_id) + " for case: " + to_string(case_profile_id));
        }

        PainBodyMap::PainBodyMap(int pbm_id, int case_profile_id, const string& painDataJson,
                                const string& additionalComments, const DateTime& createdAt, const DateTime& updatedAt)
            : m_pbm_id(pbm_id), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_pain_data_json(painDataJson), m_additional_comments(additionalComments),
              m_pbm_createdAt(createdAt), m_pbm_updatedAt(updatedAt) {
            
            if (!isValidCaseProfileId(case_profile_id)) {
                throw invalid_argument("Invalid case profile ID provided");
            }
            
            // Parse JSON to C++ map
            syncJsonToCppData();
            
            utils::logMessage("INFO", "PainBodyMap loaded from database with ID: " + to_string(m_pbm_id));
        }

        // Private helper methods
        void PainBodyMap::setTimestamps() {
            m_pbm_createdAt = DateTime();
            m_pbm_updatedAt = DateTime();
        }

        void PainBodyMap::updateTimestamp() {
            m_pbm_updatedAt = DateTime();
        }

        int PainBodyMap::getNextPBMId() {
            return ++pbm_id_counter;
        }

        void PainBodyMap::syncJsonToCppData() {
            // Simple JSON parsing to populate m_pain_data map
            // In production, consider using a proper JSON library like nlohmann/json
            m_pain_data.clear();
            
            if (m_pain_data_json.empty() || m_pain_data_json == "{}") {
                return;
            }
            
            // Basic parsing - this is simplified for demonstration
            // Real implementation should use a robust JSON parser
            size_t pos = 0;
            while (pos < m_pain_data_json.length()) {
                size_t key_start = m_pain_data_json.find("\"", pos);
                if (key_start == string::npos) break;
                key_start++;
                
                size_t key_end = m_pain_data_json.find("\"", key_start);
                if (key_end == string::npos) break;
                
                string key = m_pain_data_json.substr(key_start, key_end - key_start);
                
                size_t value_start = m_pain_data_json.find("{", key_end);
                if (value_start == string::npos) break;
                
                size_t value_end = m_pain_data_json.find("}", value_start);
                if (value_end == string::npos) break;
                value_end++;
                
                string value = m_pain_data_json.substr(value_start, value_end - value_start);
                
                BodyPartPain pain;
                pain.fromJson(value);
                m_pain_data[key] = pain;
                
                pos = value_end;
            }
        }

        void PainBodyMap::syncCppToJsonData() {
            // Convert C++ map to JSON string
            if (m_pain_data.empty()) {
                m_pain_data_json = "{}";
                return;
            }
            
            stringstream json;
            json << "{";
            
            bool first = true;
            for (const auto& [key, pain] : m_pain_data) {
                if (!first) json << ",";
                json << "\"" << key << "\":" << pain.toJson();
                first = false;
            }
            
            json << "}";
            m_pain_data_json = json.str();
        }

        // JSON access methods
        void PainBodyMap::setPainDataJson(const string& json) {
            m_pain_data_json = json;
            syncJsonToCppData();
            updateTimestamp();
        }

        // Pain data manipulation
        void PainBodyMap::setPainForBodyPart(const string& bodyPart, bool leftSide, bool rightSide, 
                                           int painLevel, const string& comments) {
            if (!isValidBodyPart(bodyPart)) {
                throw invalid_argument("Invalid body part: " + bodyPart);
            }
            if (!isValidPainLevel(painLevel)) {
                throw invalid_argument("Invalid pain level: " + to_string(painLevel));
            }
            if (!isValidComments(comments)) {
                throw invalid_argument("Comments too long (max 500 characters)");
            }
            
            BodyPartPain pain(bodyPart, leftSide, rightSide, painLevel, comments);
            m_pain_data[bodyPart] = pain;
            syncCppToJsonData();
            updateTimestamp();
        }

        void PainBodyMap::setPainForBodyPart(const BodyPartPain& pain) {
            if (!isValidBodyPart(pain.body_part)) {
                throw invalid_argument("Invalid body part: " + pain.body_part);
            }
            if (!isValidPainLevel(pain.pain_level)) {
                throw invalid_argument("Invalid pain level: " + to_string(pain.pain_level));
            }
            if (!isValidComments(pain.comments)) {
                throw invalid_argument("Comments too long (max 500 characters)");
            }
            
            m_pain_data[pain.body_part] = pain;
            syncCppToJsonData();
            updateTimestamp();
        }

        void PainBodyMap::removePainForBodyPart(const string& bodyPart) {
            auto it = m_pain_data.find(bodyPart);
            if (it != m_pain_data.end()) {
                m_pain_data.erase(it);
                syncCppToJsonData();
                updateTimestamp();
            }
        }

        void PainBodyMap::clearAllPainData() {
            m_pain_data.clear();
            m_pain_data_json = "{}";
            updateTimestamp();
        }

        void PainBodyMap::setAdditionalComments(const string& comments) {
            if (comments.length() > 2000) {
                throw invalid_argument("Additional comments too long (max 2000 characters)");
            }
            m_additional_comments = comments;
            updateTimestamp();
        }

        // Individual body part access
        BodyPartPain PainBodyMap::getPainForBodyPart(const string& bodyPart) const {
            auto it = m_pain_data.find(bodyPart);
            if (it != m_pain_data.end()) {
                return it->second;
            }
            return BodyPartPain(); // Return empty pain data if not found
        }

        bool PainBodyMap::hasEntryForBodyPart(const string& bodyPart) const {
            return m_pain_data.find(bodyPart) != m_pain_data.end();
        }

        // Validation methods
        bool PainBodyMap::isValidCaseProfileId(int case_profile_id) const {
            return case_profile_id > 0 && case_profile_id >= CaseProfile::ID_PREFIX;
        }

        bool PainBodyMap::isValidPainLevel(int level) const {
            return level >= 0 && level <= 10;
        }

        bool PainBodyMap::isValidBodyPart(const string& bodyPart) const {
            return find(STANDARD_BODY_PARTS.begin(), STANDARD_BODY_PARTS.end(), bodyPart) != STANDARD_BODY_PARTS.end();
        }

        bool PainBodyMap::isValidComments(const string& comments) const {
            return comments.length() <= 500;
        }

        bool PainBodyMap::isValidData() const {
            if (!isValidCaseProfileId(m_case_profile_id)) return false;
            if (m_additional_comments.length() > 2000) return false;
            
            for (const auto& [key, pain] : m_pain_data) {
                if (!isValidBodyPart(pain.body_part)) return false;
                if (!isValidPainLevel(pain.pain_level)) return false;
                if (!isValidComments(pain.comments)) return false;
            }
            
            return true;
        }

        // Analysis methods
        int PainBodyMap::getTotalAffectedBodyParts() const {
            int count = 0;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.pain_level > 0) {
                    count++;
                }
            }
            return count;
        }

        int PainBodyMap::getHighestPainLevel() const {
            int highest = 0;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.pain_level > highest) {
                    highest = pain.pain_level;
                }
            }
            return highest;
        }

        double PainBodyMap::getAveragePainLevel() const {
            if (m_pain_data.empty()) return 0.0;
            
            int total = 0;
            int count = 0;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.pain_level > 0) {
                    total += pain.pain_level;
                    count++;
                }
            }
            
            return count > 0 ? static_cast<double>(total) / count : 0.0;
        }

        vector<string> PainBodyMap::getMostPainfulBodyParts(int threshold) const {
            vector<string> result;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.pain_level >= threshold) {
                    result.push_back(pain.body_part);
                }
            }
            return result;
        }

        vector<string> PainBodyMap::getLeftSideAffected() const {
            vector<string> result;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.side_left && pain.pain_level > 0) {
                    result.push_back(pain.body_part);
                }
            }
            return result;
        }

        vector<string> PainBodyMap::getRightSideAffected() const {
            vector<string> result;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.side_right && pain.pain_level > 0) {
                    result.push_back(pain.body_part);
                }
            }
            return result;
        }

        vector<string> PainBodyMap::getBilateralAffected() const {
            vector<string> result;
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.side_left && pain.side_right && pain.pain_level > 0) {
                    result.push_back(pain.body_part);
                }
            }
            return result;
        }

        // Display methods
        void PainBodyMap::displayInfo() const {
            cout << "\n=== Pain Body Map Information ===" << endl;
            cout << "PBM ID: " << m_pbm_id << endl;
            cout << "Case Profile ID: " << m_case_profile_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Created: " << m_pbm_createdAt.toString() << endl;
            cout << "Updated: " << m_pbm_updatedAt.toString() << endl;
            cout << "Total Affected Body Parts: " << getTotalAffectedBodyParts() << endl;
            cout << "Highest Pain Level: " << getHighestPainLevel() << endl;
            cout << "Average Pain Level: " << fixed << setprecision(1) << getAveragePainLevel() << endl;
            
            if (!m_additional_comments.empty()) {
                cout << "Additional Comments: " << m_additional_comments << endl;
            }
        }

        void PainBodyMap::displaySummary() const {
            cout << "\n=== Pain Body Map Summary ===" << endl;
            cout << "PBM ID: " << m_pbm_id << " | Case: " << m_case_profile_id << endl;
            cout << "Affected Parts: " << getTotalAffectedBodyParts() << " | Max Pain: " << getHighestPainLevel() << "/10" << endl;
            
            auto highPain = getMostPainfulBodyParts(7);
            if (!highPain.empty()) {
                cout << "High Pain Areas (7+): ";
                for (size_t i = 0; i < highPain.size(); ++i) {
                    cout << highPain[i];
                    if (i < highPain.size() - 1) cout << ", ";
                }
                cout << endl;
            }
        }

        void PainBodyMap::displayPainMap() const {
            cout << "\n=== Detailed Pain Map ===" << endl;
            if (m_pain_data.empty()) {
                cout << "No pain data recorded." << endl;
                return;
            }
            
            cout << left << setw(20) << "Body Part" 
                 << setw(8) << "Left" 
                 << setw(8) << "Right" 
                 << setw(6) << "Pain" 
                 << "Comments" << endl;
            cout << string(60, '-') << endl;
            
            for (const auto& [key, pain] : m_pain_data) {
                if (pain.pain_level > 0) {
                    cout << left << setw(20) << pain.body_part
                         << setw(8) << (pain.side_left ? "Yes" : "No")
                         << setw(8) << (pain.side_right ? "Yes" : "No")
                         << setw(6) << pain.pain_level
                         << pain.comments << endl;
                }
            }
        }

        void PainBodyMap::displayAnalysis() const {
            cout << "\n=== Pain Analysis ===" << endl;
            cout << "Total Body Parts Affected: " << getTotalAffectedBodyParts() << endl;
            cout << "Highest Pain Level: " << getHighestPainLevel() << "/10" << endl;
            cout << "Average Pain Level: " << fixed << setprecision(1) << getAveragePainLevel() << "/10" << endl;
            
            auto leftSide = getLeftSideAffected();
            auto rightSide = getRightSideAffected();
            auto bilateral = getBilateralAffected();
            
            cout << "\nLateral Distribution:" << endl;
            cout << "  Left Side Affected: " << leftSide.size() << " parts" << endl;
            cout << "  Right Side Affected: " << rightSide.size() << " parts" << endl;
            cout << "  Bilateral Pain: " << bilateral.size() << " parts" << endl;
            
            auto severePain = getMostPainfulBodyParts(8);
            auto moderatePain = getMostPainfulBodyParts(5);
            
            cout << "\nSeverity Distribution:" << endl;
            cout << "  Severe Pain (8-10): " << severePain.size() << " parts" << endl;
            cout << "  Moderate+ Pain (5-10): " << moderatePain.size() << " parts" << endl;
        }

        string PainBodyMap::toString() const {
            stringstream ss;
            ss << "PainBodyMap[ID=" << m_pbm_id 
               << ", CaseProfile=" << m_case_profile_id
               << ", Type=" << m_type
               << ", AffectedParts=" << getTotalAffectedBodyParts()
               << ", MaxPain=" << getHighestPainLevel()
               << ", AvgPain=" << fixed << setprecision(1) << getAveragePainLevel()
               << ", Created=" << m_pbm_createdAt.toString()
               << "]";
            return ss.str();
        }

        // Static methods
        void PainBodyMap::resetIdCounter() {
            pbm_id_counter = ID_PREFIX;
        }

        int PainBodyMap::getNextId() {
            return pbm_id_counter + 1;
        }

        bool PainBodyMap::isValidBodyPartName(const string& bodyPart) {
            return find(STANDARD_BODY_PARTS.begin(), STANDARD_BODY_PARTS.end(), bodyPart) != STANDARD_BODY_PARTS.end();
        }

        vector<string> PainBodyMap::getStandardBodyParts() {
            return STANDARD_BODY_PARTS;
        }

        // Stream operators
        ostream& operator<<(ostream& os, const PainBodyMap& pbm) {
            os << pbm.toString();
            return os;
        }

        istream& operator>>(istream& is, PainBodyMap& pbm) {
            // Basic implementation for reading from stream
            // In practice, this would be more sophisticated
            string line;
            getline(is, line);
            // Parse line and populate pbm object
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
