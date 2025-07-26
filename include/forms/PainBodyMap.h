#ifndef PAINBODYMAP_H
#define PAINBODYMAP_H

#include <string>
#include <map>
#include <vector>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        /**
         * @brief Structure representing pain data for a specific body part
         */
        struct BodyPartPain {
            string body_part;               // Name of the body part (e.g., "head", "jaw", "shoulders")
            bool side_left;                 // Left side affected
            bool side_right;                // Right side affected  
            int pain_level;                 // 0-10 scale
            string comments;                // Max 255 characters
            
            BodyPartPain() : body_part(""), side_left(false), side_right(false), pain_level(0), comments("") {}
            
            BodyPartPain(const string& part, bool left, bool right, int level, const string& comment = "")
                : body_part(part), side_left(left), side_right(right), pain_level(level), comments(comment) {}

            // JSON conversion methods
            string toJson() const;
            void fromJson(const string& json);
        };

        /**
         * @brief Pain Body Map form using HYBRID approach (C++ + JSON)
         * 
         * This class follows the Beck Inventory pattern for basic fields,
         * but uses JSON for complex pain data storage in SQLite.
         */
        class PainBodyMap {
        private:
            static const string FORM_TYPE;     // Form type identifier: "PBM"
            
            // Primary attributes (traditional C++ approach)
            int m_pbm_id;                      // PBM ID (1000000+)
            int m_case_profile_id;             // Foreign key to case_profile
            string m_type;                     // Form type (default: "PBM")
            
            // JSON field for complex data (stored as TEXT in SQLite)
            string m_pain_data_json;           // JSON string containing all pain data
            string m_additional_comments;      // Other pain experiences
            
            // Internal C++ representation (for easy manipulation)
            map<string, BodyPartPain> m_pain_data;     // Parsed from JSON for runtime use
            
            DateTime m_pbm_createdAt;          // Creation timestamp
            DateTime m_pbm_updatedAt;          // Last update timestamp
            
            static int pbm_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextPBMId();
            
            // JSON synchronization methods
            void syncJsonToCppData();          // Parse JSON to C++ map
            void syncCppToJsonData();          // Serialize C++ map to JSON

        public:
            // Standard body parts list
            static const vector<string> STANDARD_BODY_PARTS;
            
            // Constructors
            PainBodyMap();
            PainBodyMap(int case_profile_id);
            PainBodyMap(int pbm_id, int case_profile_id, 
                       const string& painDataJson,
                       const string& additionalComments,
                       const DateTime& createdAt, const DateTime& updatedAt);
            
            // Destructor
            ~PainBodyMap() = default;
            
            // Getters - Basic Info
            int getPBMId() const { return m_pbm_id; }
            int getCaseProfileId() const { return m_case_profile_id; }
            string getType() const { return m_type; }
            DateTime getPBMCreatedAt() const { return m_pbm_createdAt; }
            DateTime getPBMUpdatedAt() const { return m_pbm_updatedAt; }
            string getAdditionalComments() const { return m_additional_comments; }
            
            // JSON access (for database storage)
            string getPainDataJson() const { return m_pain_data_json; }
            void setPainDataJson(const string& json);
            
            // C++ map access (for runtime manipulation)
            map<string, BodyPartPain> getPainDataMap() const { return m_pain_data; }
            
            // Individual body part access
            BodyPartPain getPainForBodyPart(const string& bodyPart) const;
            bool hasEntryForBodyPart(const string& bodyPart) const;
            map<string, BodyPartPain> getAllPainData() const { return m_pain_data; }
            
            // Setters with validation - Basic Info
            void setCaseProfileId(int case_profile_id) { 
                m_case_profile_id = case_profile_id; 
                updateTimestamp(); 
            }
            void setPBMCreatedAt(const DateTime& createdAt) { m_pbm_createdAt = createdAt; }
            void setPBMUpdatedAt(const DateTime& updatedAt) { m_pbm_updatedAt = updatedAt; }
            void setAdditionalComments(const string& comments);
            
            // Pain data manipulation
            void setPainForBodyPart(const string& bodyPart, bool leftSide, bool rightSide, 
                                  int painLevel, const string& comments = "");
            void setPainForBodyPart(const BodyPartPain& pain);  // Uses pain.body_part as key
            void removePainForBodyPart(const string& bodyPart);
            void clearAllPainData();
            
            // Validation methods
            bool isValidCaseProfileId(int case_profile_id) const;
            bool isValidPainLevel(int level) const;
            bool isValidBodyPart(const string& bodyPart) const;
            bool isValidComments(const string& comments) const;
            bool isValidData() const;
            
            // Analysis methods
            int getTotalAffectedBodyParts() const;
            int getHighestPainLevel() const;
            double getAveragePainLevel() const;
            vector<string> getMostPainfulBodyParts(int threshold = 7) const;
            vector<string> getLeftSideAffected() const;
            vector<string> getRightSideAffected() const;
            vector<string> getBilateralAffected() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            void displayPainMap() const;
            void displayAnalysis() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            static const int ID_PREFIX = 1000000; // PBM IDs start at 1000001
            
            // Static validation methods
            static bool isValidBodyPartName(const string& bodyPart);
            static vector<string> getStandardBodyParts();
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const PainBodyMap& pbm);
            friend std::istream& operator>>(std::istream& is, PainBodyMap& pbm);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // PAINBODYMAP_H
