#include "forms/AutomobileAnxietyInventory.h"
#include <iostream>
#include <sstream>
#include <vector>

namespace SilverClinic {
    namespace Forms {

        // Static member initialization
        const string AutomobileAnxietyInventory::FORM_TYPE = "AAI";
        int AutomobileAnxietyInventory::aai_id_counter = 0;

        // Default constructor
        AutomobileAnxietyInventory::AutomobileAnxietyInventory() 
            : m_aai_id(getNextAAIId()),
              m_case_profile_id(0),
              m_type(FORM_TYPE),
              m_question_1(false), m_question_2(false), m_question_3(false),
              m_question_4(false), m_question_5(false), m_question_6(false),
              m_question_7(false), m_question_8(false), m_question_9(false),
              m_question_10(false), m_question_11(false), m_question_12(false),
              m_question_13(false),
              m_question_14_driver(false), m_question_14_passenger(false), m_question_14_no_difference(false),
              m_question_15_a(false), m_question_15_b(""),
              m_question_16(false), m_question_17(false), m_question_18(false),
              m_question_19(false), m_question_19_sidewalks(false), m_question_19_crossing(false), m_question_19_both(false),
              m_question_20(false), m_question_21(false), m_question_22(false), m_question_23(false) {
            setTimestamps();
            utils::logMessage("INFO", "AutomobileAnxietyInventory created with ID: " + to_string(m_aai_id));
        }

        // Constructor with case profile ID
        AutomobileAnxietyInventory::AutomobileAnxietyInventory(int case_profile_id)
            : m_aai_id(getNextAAIId()),
              m_case_profile_id(case_profile_id),
              m_type(FORM_TYPE),
              m_question_1(false), m_question_2(false), m_question_3(false),
              m_question_4(false), m_question_5(false), m_question_6(false),
              m_question_7(false), m_question_8(false), m_question_9(false),
              m_question_10(false), m_question_11(false), m_question_12(false),
              m_question_13(false),
              m_question_14_driver(false), m_question_14_passenger(false), m_question_14_no_difference(false),
              m_question_15_a(false), m_question_15_b(""),
              m_question_16(false), m_question_17(false), m_question_18(false),
              m_question_19(false), m_question_19_sidewalks(false), m_question_19_crossing(false), m_question_19_both(false),
              m_question_20(false), m_question_21(false), m_question_22(false), m_question_23(false) {
            
            if (isValidCaseProfileId(case_profile_id)) {
                setTimestamps();
                utils::logMessage("INFO", "AutomobileAnxietyInventory created with ID: " + to_string(m_aai_id) + 
                                         " for CaseProfile: " + to_string(case_profile_id));
            } else {
                utils::logMessage("WARNING", "Invalid case_profile_id provided: " + to_string(case_profile_id));
            }
        }

        // Full constructor for database loading
        AutomobileAnxietyInventory::AutomobileAnxietyInventory(int aai_id, int case_profile_id,
                                                             bool q1, bool q2, bool q3, bool q4, bool q5,
                                                             bool q6, bool q7, bool q8, bool q9, bool q10,
                                                             bool q11, bool q12, bool q13,
                                                             bool q14_driver, bool q14_passenger, bool q14_no_diff,
                                                             bool q15_a, const string& q15_b,
                                                             bool q16, bool q17, bool q18,
                                                             bool q19, bool q19_sidewalks, bool q19_crossing, bool q19_both,
                                                             bool q20, bool q21, bool q22, bool q23,
                                                             const DateTime& createdAt, const DateTime& updatedAt)
            : m_aai_id(aai_id), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(q1), m_question_2(q2), m_question_3(q3), m_question_4(q4), m_question_5(q5),
              m_question_6(q6), m_question_7(q7), m_question_8(q8), m_question_9(q9), m_question_10(q10),
              m_question_11(q11), m_question_12(q12), m_question_13(q13),
              m_question_14_driver(q14_driver), m_question_14_passenger(q14_passenger), m_question_14_no_difference(q14_no_diff),
              m_question_15_a(q15_a), m_question_15_b(q15_b),
              m_question_16(q16), m_question_17(q17), m_question_18(q18),
              m_question_19(q19), m_question_19_sidewalks(q19_sidewalks), m_question_19_crossing(q19_crossing), m_question_19_both(q19_both),
              m_question_20(q20), m_question_21(q21), m_question_22(q22), m_question_23(q23),
              m_aai_createdAt(createdAt), m_aai_updatedAt(updatedAt) {
            
            utils::logMessage("INFO", "AutomobileAnxietyInventory loaded from database with ID: " + to_string(m_aai_id));
        }

        // Private helper methods
        void AutomobileAnxietyInventory::setTimestamps() {
            DateTime now = DateTime::now();
            m_aai_createdAt = now;
            m_aai_updatedAt = now;
        }

        void AutomobileAnxietyInventory::updateTimestamp() {
            m_aai_updatedAt = DateTime::now();
        }

        int AutomobileAnxietyInventory::getNextAAIId() {
            return ID_PREFIX + (++aai_id_counter);
        }

        // Question 14 special methods
        string AutomobileAnxietyInventory::getQuestion14Response() const {
            if (m_question_14_driver) return "as_a_driver";
            if (m_question_14_passenger) return "as_a_passenger";
            if (m_question_14_no_difference) return "no_difference";
            return "not_answered";
        }

        void AutomobileAnxietyInventory::setQuestion14Response(const string& option) {
            // Reset all options first
            m_question_14_driver = false;
            m_question_14_passenger = false;
            m_question_14_no_difference = false;
            
            string normalizedOption = utils::normalizeForDatabase(option);
            
            if (normalizedOption == "AS_A_DRIVER") {
                m_question_14_driver = true;
            } else if (normalizedOption == "AS_A_PASSENGER") {
                m_question_14_passenger = true;
            } else if (normalizedOption == "NO_DIFFERENCE") {
                m_question_14_no_difference = true;
            } else {
                utils::logMessage("WARNING", "Invalid Question 14 option: " + option);
                return;
            }
            
            updateTimestamp();
            utils::logMessage("INFO", "Question 14 set to: " + normalizedOption);
        }

        // Question 15 text setter with validation
        void AutomobileAnxietyInventory::setQuestion15B(const string& text) {
            string normalizedText = utils::normalizeForDatabase(text);
            
            if (isValidQuestion15Text(normalizedText)) {
                m_question_15_b = normalizedText;
                updateTimestamp();
                utils::logMessage("INFO", "Question 15 text set successfully");
            } else {
                // Truncate if too long
                m_question_15_b = normalizedText.substr(0, MAX_TEXT_LENGTH);
                updateTimestamp();
                utils::logMessage("WARNING", "Question 15 text truncated to " + to_string(MAX_TEXT_LENGTH) + " characters");
            }
        }

        // Question 19 special methods
        string AutomobileAnxietyInventory::getQuestion19Options() const {
            vector<string> options;
            if (m_question_19_sidewalks) options.push_back("sidewalks");
            if (m_question_19_crossing) options.push_back("crossing_streets");
            if (m_question_19_both) options.push_back("both");
            
            if (options.empty()) return "none";
            
            string result = "";
            for (size_t i = 0; i < options.size(); ++i) {
                if (i > 0) result += ",";
                result += options[i];
            }
            return result;
        }

        void AutomobileAnxietyInventory::setQuestion19Options(bool sidewalks, bool crossing, bool both) {
            m_question_19_sidewalks = sidewalks;
            m_question_19_crossing = crossing;
            m_question_19_both = both;
            updateTimestamp();
            utils::logMessage("INFO", "Question 19 options updated");
        }

        // Validation methods
        bool AutomobileAnxietyInventory::isValidCaseProfileId(int case_profile_id) const {
            return case_profile_id > 400000; // CaseProfile IDs start at 400001
        }

        bool AutomobileAnxietyInventory::isValidQuestion15Text(const string& text) const {
            return text.length() <= MAX_TEXT_LENGTH;
        }

        bool AutomobileAnxietyInventory::isValidData() const {
            if (!isValidCaseProfileId(m_case_profile_id)) {
                utils::logMessage("ERROR", "Invalid case_profile_id: " + to_string(m_case_profile_id));
                return false;
            }
            
            if (!isValidQuestion15Text(m_question_15_b)) {
                utils::logMessage("ERROR", "Question 15 text too long: " + to_string(m_question_15_b.length()) + " chars");
                return false;
            }
            
            return true;
        }

        // Utility methods
        int AutomobileAnxietyInventory::getTotalYesResponses() const {
            int count = 0;
            
            // Count simple yes/no questions
            if (m_question_1) count++;
            if (m_question_2) count++;
            if (m_question_3) count++;
            if (m_question_4) count++;
            if (m_question_5) count++;
            if (m_question_6) count++;
            if (m_question_7) count++;
            if (m_question_8) count++;
            if (m_question_9) count++;
            if (m_question_10) count++;
            if (m_question_11) count++;
            if (m_question_12) count++;
            if (m_question_13) count++;
            if (m_question_15_a) count++;
            if (m_question_16) count++;
            if (m_question_17) count++;
            if (m_question_18) count++;
            if (m_question_19) count++;
            if (m_question_20) count++;
            if (m_question_21) count++;
            if (m_question_22) count++;
            if (m_question_23) count++;
            
            return count;
        }

        double AutomobileAnxietyInventory::getAnxietyScore() const {
            // Calculate anxiety score as percentage of yes responses
            // Total possible yes responses: 22 (excluding question 14 which is multiple choice)
            return (getTotalYesResponses() / 22.0) * 100.0;
        }

        bool AutomobileAnxietyInventory::hasHighAnxiety() const {
            // Consider high anxiety if more than 60% yes responses
            return getAnxietyScore() > 60.0;
        }

        // Display methods
        void AutomobileAnxietyInventory::displayInfo() const {
            cout << "\n=== Automobile Anxiety Inventory ===" << endl;
            cout << "AAI ID: " << m_aai_id << endl;
            cout << "Case Profile ID: " << m_case_profile_id << endl;
            cout << "Form Type: " << m_type << endl;
            cout << "Created: " << m_aai_createdAt.toString() << endl;
            cout << "Updated: " << m_aai_updatedAt.toString() << endl;
            cout << "Total Yes Responses: " << getTotalYesResponses() << "/22" << endl;
            cout << "Anxiety Score: " << getAnxietyScore() << "%" << endl;
            cout << "High Anxiety: " << (hasHighAnxiety() ? "Yes" : "No") << endl;
        }

        void AutomobileAnxietyInventory::displaySummary() const {
            cout << "\n=== AAI Summary ===" << endl;
            cout << "ID: " << m_aai_id << " | Case: " << m_case_profile_id << endl;
            cout << "Anxiety Score: " << getAnxietyScore() << "% (" << getTotalYesResponses() << "/22 yes)" << endl;
            if (hasHighAnxiety()) {
                cout << "⚠️ HIGH ANXIETY LEVEL DETECTED" << endl;
            }
        }

        void AutomobileAnxietyInventory::displayAllResponses() const {
            cout << "\n=== All AAI Responses ===" << endl;
            
            cout << "Q1. More nervous in automobile: " << (m_question_1 ? "Yes" : "No") << endl;
            cout << "Q2. Have motor vehicle license: " << (m_question_2 ? "Yes" : "No") << endl;
            cout << "Q3. Driven since accident: " << (m_question_3 ? "Yes" : "No") << endl;
            cout << "Q4. Avoid driving: " << (m_question_4 ? "Yes" : "No") << endl;
            cout << "Q5. Drive slower/more carefully: " << (m_question_5 ? "Yes" : "No") << endl;
            cout << "Q6. Been passenger since accident: " << (m_question_6 ? "Yes" : "No") << endl;
            cout << "Q7. Avoid being passenger: " << (m_question_7 ? "Yes" : "No") << endl;
            cout << "Q8. More nervous after nightfall: " << (m_question_8 ? "Yes" : "No") << endl;
            cout << "Q9. More nervous in traffic: " << (m_question_9 ? "Yes" : "No") << endl;
            cout << "Q10. More nervous on highway: " << (m_question_10 ? "Yes" : "No") << endl;
            cout << "Q11. More nervous in intersection: " << (m_question_11 ? "Yes" : "No") << endl;
            cout << "Q12. More nervous outside home area: " << (m_question_12 ? "Yes" : "No") << endl;
            cout << "Q13. More nervous in rain/bad weather: " << (m_question_13 ? "Yes" : "No") << endl;
            cout << "Q14. More nervous as: " << getQuestion14Response() << endl;
            cout << "Q15a. Nervous with certain people: " << (m_question_15_a ? "Yes" : "No") << endl;
            if (m_question_15_a) {
                cout << "Q15b. Who: " << m_question_15_b << endl;
            }
            cout << "Q16. Nervous with vehicles nearby: " << (m_question_16 ? "Yes" : "No") << endl;
            cout << "Q17. Nervous hearing horn/brakes: " << (m_question_17 ? "Yes" : "No") << endl;
            cout << "Q18. Nervous passing accident scene: " << (m_question_18 ? "Yes" : "No") << endl;
            cout << "Q19. More nervous as pedestrian: " << (m_question_19 ? "Yes" : "No") << endl;
            if (m_question_19) {
                cout << "Q19 options: " << getQuestion19Options() << endl;
            }
            cout << "Q20. Think about accident in car: " << (m_question_20 ? "Yes" : "No") << endl;
            cout << "Q21. Feel about to have another accident: " << (m_question_21 ? "Yes" : "No") << endl;
            cout << "Q22. Never regain confidence: " << (m_question_22 ? "Yes" : "No") << endl;
            cout << "Q23. Have panic attacks: " << (m_question_23 ? "Yes" : "No") << endl;
        }

        string AutomobileAnxietyInventory::toString() const {
            stringstream ss;
            ss << "AAI[ID:" << m_aai_id << ",Case:" << m_case_profile_id 
               << ",Score:" << getAnxietyScore() << "%]";
            return ss.str();
        }

        // Static methods
        void AutomobileAnxietyInventory::resetIdCounter() {
            aai_id_counter = 0;
            utils::logMessage("INFO", "AutomobileAnxietyInventory ID counter reset");
        }

        int AutomobileAnxietyInventory::getNextId() {
            return ID_PREFIX + (aai_id_counter + 1);
        }

        // Stream operators
        std::ostream& operator<<(std::ostream& os, const AutomobileAnxietyInventory& aai) {
            os << aai.toString();
            return os;
        }

        std::istream& operator>>(std::istream& is, AutomobileAnxietyInventory& aai) {
            // Basic input implementation - can be expanded as needed
            string input;
            getline(is, input);
            // For now, just log that input was received
            utils::logMessage("INFO", "AAI input received: " + input);
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
