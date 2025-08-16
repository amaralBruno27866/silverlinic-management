#include "forms/BeckAnxietyInventory.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static member initialization
        const string BeckAnxietyInventory::FORM_TYPE = "BAI";
        int BeckAnxietyInventory::bai_id_counter = 0;

        // Default constructor
        BeckAnxietyInventory::BeckAnxietyInventory() 
            : m_bai_id(getNextBAIId()), m_case_profile_id(0), m_type(FORM_TYPE),
              m_question_1(0), m_question_2(0), m_question_3(0), m_question_4(0), m_question_5(0),
              m_question_6(0), m_question_7(0), m_question_8(0), m_question_9(0), m_question_10(0),
              m_question_11(0), m_question_12(0), m_question_13(0), m_question_14(0), m_question_15(0),
              m_question_16(0), m_question_17(0), m_question_18(0), m_question_19(0), m_question_20(0), m_question_21(0) {
            setTimestamps();
            utils::logStructured(utils::LogLevel::INFO, {"FORM","create","BAI", to_string(m_bai_id), {}}, "Created");
        }

        // Constructor with case profile ID
        BeckAnxietyInventory::BeckAnxietyInventory(int case_profile_id) 
            : m_bai_id(getNextBAIId()), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(0), m_question_2(0), m_question_3(0), m_question_4(0), m_question_5(0),
              m_question_6(0), m_question_7(0), m_question_8(0), m_question_9(0), m_question_10(0),
              m_question_11(0), m_question_12(0), m_question_13(0), m_question_14(0), m_question_15(0),
              m_question_16(0), m_question_17(0), m_question_18(0), m_question_19(0), m_question_20(0), m_question_21(0) {
            setTimestamps();
            utils::logStructured(utils::LogLevel::INFO, {"FORM","create","BAI", to_string(m_bai_id), {}}, "Created for case: "+ to_string(case_profile_id));
        }

        // Full constructor
        BeckAnxietyInventory::BeckAnxietyInventory(int bai_id, int case_profile_id, 
                                                 int q1, int q2, int q3, int q4, int q5,
                                                 int q6, int q7, int q8, int q9, int q10,
                                                 int q11, int q12, int q13, int q14, int q15,
                                                 int q16, int q17, int q18, int q19, int q20, int q21,
                                                 const DateTime& createdAt, const DateTime& updatedAt)
            : m_bai_id(bai_id), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(q1), m_question_2(q2), m_question_3(q3), m_question_4(q4), m_question_5(q5),
              m_question_6(q6), m_question_7(q7), m_question_8(q8), m_question_9(q9), m_question_10(q10),
              m_question_11(q11), m_question_12(q12), m_question_13(q13), m_question_14(q14), m_question_15(q15),
              m_question_16(q16), m_question_17(q17), m_question_18(q18), m_question_19(q19), m_question_20(q20), m_question_21(q21),
              m_bai_createdAt(createdAt), m_bai_updatedAt(updatedAt) {
            utils::logStructured(utils::LogLevel::INFO, {"FORM","load","BAI", to_string(m_bai_id), {}}, "Loaded from database");
        }

        // Private helper methods
        void BeckAnxietyInventory::setTimestamps() {
            DateTime now = DateTime::now();
            m_bai_createdAt = now;
            m_bai_updatedAt = now;
        }

        void BeckAnxietyInventory::updateTimestamp() {
            m_bai_updatedAt = DateTime::now();
        }

        int BeckAnxietyInventory::getNextBAIId() {
            return ID_PREFIX + (++bai_id_counter);
        }

        // Setters with validation
        void BeckAnxietyInventory::setQuestion1(int value) {
            if (isValidQuestionValue(value)) {
                m_question_1 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q1","BAI", to_string(m_bai_id), {}}, "Invalid value for Q1");
            }
        }

        void BeckAnxietyInventory::setQuestion2(int value) {
            if (isValidQuestionValue(value)) {
                m_question_2 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q2","BAI", to_string(m_bai_id), {}}, "Invalid value for Q2");
            }
        }

        void BeckAnxietyInventory::setQuestion3(int value) {
            if (isValidQuestionValue(value)) {
                m_question_3 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q3","BAI", to_string(m_bai_id), {}}, "Invalid value for Q3");
            }
        }

        void BeckAnxietyInventory::setQuestion4(int value) {
            if (isValidQuestionValue(value)) {
                m_question_4 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q4","BAI", to_string(m_bai_id), {}}, "Invalid value for Q4");
            }
        }

        void BeckAnxietyInventory::setQuestion5(int value) {
            if (isValidQuestionValue(value)) {
                m_question_5 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q5","BAI", to_string(m_bai_id), {}}, "Invalid value for Q5");
            }
        }

        void BeckAnxietyInventory::setQuestion6(int value) {
            if (isValidQuestionValue(value)) {
                m_question_6 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q6","BAI", to_string(m_bai_id), {}}, "Invalid value for Q6");
            }
        }

        void BeckAnxietyInventory::setQuestion7(int value) {
            if (isValidQuestionValue(value)) {
                m_question_7 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q7","BAI", to_string(m_bai_id), {}}, "Invalid value for Q7");
            }
        }

        void BeckAnxietyInventory::setQuestion8(int value) {
            if (isValidQuestionValue(value)) {
                m_question_8 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q8","BAI", to_string(m_bai_id), {}}, "Invalid value for Q8");
            }
        }

        void BeckAnxietyInventory::setQuestion9(int value) {
            if (isValidQuestionValue(value)) {
                m_question_9 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q9","BAI", to_string(m_bai_id), {}}, "Invalid value for Q9");
            }
        }

        void BeckAnxietyInventory::setQuestion10(int value) {
            if (isValidQuestionValue(value)) {
                m_question_10 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q10","BAI", to_string(m_bai_id), {}}, "Invalid value for Q10");
            }
        }

        void BeckAnxietyInventory::setQuestion11(int value) {
            if (isValidQuestionValue(value)) {
                m_question_11 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q11","BAI", to_string(m_bai_id), {}}, "Invalid value for Q11");
            }
        }

        void BeckAnxietyInventory::setQuestion12(int value) {
            if (isValidQuestionValue(value)) {
                m_question_12 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q12","BAI", to_string(m_bai_id), {}}, "Invalid value for Q12");
            }
        }

        void BeckAnxietyInventory::setQuestion13(int value) {
            if (isValidQuestionValue(value)) {
                m_question_13 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q13","BAI", to_string(m_bai_id), {}}, "Invalid value for Q13");
            }
        }

        void BeckAnxietyInventory::setQuestion14(int value) {
            if (isValidQuestionValue(value)) {
                m_question_14 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q14","BAI", to_string(m_bai_id), {}}, "Invalid value for Q14");
            }
        }

        void BeckAnxietyInventory::setQuestion15(int value) {
            if (isValidQuestionValue(value)) {
                m_question_15 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q15","BAI", to_string(m_bai_id), {}}, "Invalid value for Q15");
            }
        }

        void BeckAnxietyInventory::setQuestion16(int value) {
            if (isValidQuestionValue(value)) {
                m_question_16 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q16","BAI", to_string(m_bai_id), {}}, "Invalid value for Q16");
            }
        }

        void BeckAnxietyInventory::setQuestion17(int value) {
            if (isValidQuestionValue(value)) {
                m_question_17 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q17","BAI", to_string(m_bai_id), {}}, "Invalid value for Q17");
            }
        }

        void BeckAnxietyInventory::setQuestion18(int value) {
            if (isValidQuestionValue(value)) {
                m_question_18 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q18","BAI", to_string(m_bai_id), {}}, "Invalid value for Q18");
            }
        }

        void BeckAnxietyInventory::setQuestion19(int value) {
            if (isValidQuestionValue(value)) {
                m_question_19 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q19","BAI", to_string(m_bai_id), {}}, "Invalid value for Q19");
            }
        }

        void BeckAnxietyInventory::setQuestion20(int value) {
            if (isValidQuestionValue(value)) {
                m_question_20 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q20","BAI", to_string(m_bai_id), {}}, "Invalid value for Q20");
            }
        }

        void BeckAnxietyInventory::setQuestion21(int value) {
            if (isValidQuestionValue(value)) {
                m_question_21 = value;
                updateTimestamp();
            } else {
                utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_q21","BAI", to_string(m_bai_id), {}}, "Invalid value for Q21");
            }
        }

        // Validation methods
        bool BeckAnxietyInventory::isValidCaseProfileId(int case_profile_id) const {
            return case_profile_id > 0;
        }

        bool BeckAnxietyInventory::isValidQuestionValue(int value) const {
            return value >= 0 && value <= 3;
        }

        bool BeckAnxietyInventory::isValidData() const {
            if (!isValidCaseProfileId(m_case_profile_id)) {
                utils::logMessage("WARNING", "Invalid case profile ID: " + to_string(m_case_profile_id));
                return false;
            }

            vector<int> questions = {
                m_question_1, m_question_2, m_question_3, m_question_4, m_question_5,
                m_question_6, m_question_7, m_question_8, m_question_9, m_question_10,
                m_question_11, m_question_12, m_question_13, m_question_14, m_question_15,
                m_question_16, m_question_17, m_question_18, m_question_19, m_question_20, m_question_21
            };

            for (size_t i = 0; i < questions.size(); ++i) {
                if (!isValidQuestionValue(questions[i])) {
                    utils::logMessage("WARNING", "Invalid value for question " + to_string(i + 1) + ": " + to_string(questions[i]));
                    return false;
                }
            }

            return true;
        }

        // Clinical scoring methods
        int BeckAnxietyInventory::getTotalScore() const {
            return m_question_1 + m_question_2 + m_question_3 + m_question_4 + m_question_5 +
                   m_question_6 + m_question_7 + m_question_8 + m_question_9 + m_question_10 +
                   m_question_11 + m_question_12 + m_question_13 + m_question_14 + m_question_15 +
                   m_question_16 + m_question_17 + m_question_18 + m_question_19 + m_question_20 + m_question_21;
        }

        double BeckAnxietyInventory::getAnxietyPercentage() const {
            int total = getTotalScore();
            int max_possible = 21 * 3; // 21 questions * 3 points maximum each
            return (total * 100.0) / max_possible;
        }

        string BeckAnxietyInventory::getSeverityLevel() const {
            return interpretScore(getTotalScore());
        }

        bool BeckAnxietyInventory::hasMinimalAnxiety() const {
            int score = getTotalScore();
            return score >= 0 && score <= 7;
        }

        bool BeckAnxietyInventory::hasMildAnxiety() const {
            int score = getTotalScore();
            return score >= 8 && score <= 15;
        }

        bool BeckAnxietyInventory::hasModerateAnxiety() const {
            int score = getTotalScore();
            return score >= 16 && score <= 25;
        }

        bool BeckAnxietyInventory::hasSevereAnxiety() const {
            int score = getTotalScore();
            return score >= 26 && score <= 63;
        }

        // Question category analysis (adapted for anxiety symptoms)
        int BeckAnxietyInventory::getCognitiveScore() const {
            // Cognitive symptoms (legacy grouping retained for backward compatibility): 2,3,7,8,9,13,14
            return m_question_2 + m_question_3 + m_question_7 + m_question_8 + 
                   m_question_9 + m_question_13 + m_question_14;
        }

        int BeckAnxietyInventory::getAffectiveScore() const {
            // Affective symptoms (legacy grouping retained): 1,4,5,6,10,12,17
            return m_question_1 + m_question_4 + m_question_5 + m_question_6 + 
                   m_question_10 + m_question_12 + m_question_17;
        }

        int BeckAnxietyInventory::getSomaticScore() const {
            // Somatic symptoms (legacy grouping retained): 11,15,16,18,19,20,21
            return m_question_11 + m_question_15 + m_question_16 + m_question_18 + 
                   m_question_19 + m_question_20 + m_question_21;
        }

        // Display methods
        void BeckAnxietyInventory::displayInfo() const {
            cout << "\n=== Beck Anxiety Inventory (BAI) ===" << endl;
            cout << "BAI ID: " << m_bai_id << endl;
            cout << "Case Profile ID: " << m_case_profile_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Created: " << m_bai_createdAt.toString() << endl;
            cout << "Updated: " << m_bai_updatedAt.toString() << endl;
            cout << "Total Score: " << getTotalScore() << "/63" << endl;
            cout << "Anxiety Percentage: " << fixed << setprecision(1) << getAnxietyPercentage() << "%" << endl;
            cout << "Severity Level: " << getSeverityLevel() << endl;
        }

        void BeckAnxietyInventory::displaySummary() const {
            cout << "\n=== BAI Summary ===" << endl;
            cout << "ID: " << m_bai_id << " | Score: " << getTotalScore() << "/63 | ";
            cout << "Level: " << getSeverityLevel() << " (" << fixed << setprecision(1) << getAnxietyPercentage() << "%)" << endl;
        }

        void BeckAnxietyInventory::displayAllResponses() const {
            cout << "\n=== All BAI Responses ===" << endl;
            cout << "Q1 (Numbness or tingling): " << m_question_1 << endl;
            cout << "Q2 (Feeling hot): " << m_question_2 << endl;
            cout << "Q3 (Wobbliness in legs): " << m_question_3 << endl;
            cout << "Q4 (Unable to relax): " << m_question_4 << endl;
            cout << "Q5 (Fear of worst happening): " << m_question_5 << endl;
            cout << "Q6 (Dizzy or lightheaded): " << m_question_6 << endl;
            cout << "Q7 (Heart pounding/racing): " << m_question_7 << endl;
            cout << "Q8 (Unsteady): " << m_question_8 << endl;
            cout << "Q9 (Terrified or afraid): " << m_question_9 << endl;
            cout << "Q10 (Nervous): " << m_question_10 << endl;
            cout << "Q11 (Feeling of choking): " << m_question_11 << endl;
            cout << "Q12 (Hands trembling): " << m_question_12 << endl;
            cout << "Q13 (Shaky / unsteady): " << m_question_13 << endl;
            cout << "Q14 (Fear of losing control): " << m_question_14 << endl;
            cout << "Q15 (Difficulty in breathing): " << m_question_15 << endl;
            cout << "Q16 (Fear of dying): " << m_question_16 << endl;
            cout << "Q17 (Scared): " << m_question_17 << endl;
            cout << "Q18 (Indigestion): " << m_question_18 << endl;
            cout << "Q19 (Faint/lightheaded): " << m_question_19 << endl;
            cout << "Q20 (Face flushed): " << m_question_20 << endl;
            cout << "Q21 (Hot/cold sweats): " << m_question_21 << endl;
        }

        void BeckAnxietyInventory::displayScoreAnalysis() const {
            cout << "\n=== BAI Score Analysis ===" << endl;
            cout << "Total Score: " << getTotalScore() << "/63" << endl;
            cout << "Cognitive Score: " << getCognitiveScore() << "/21" << endl;
            cout << "Affective Score: " << getAffectiveScore() << "/21" << endl;
            cout << "Somatic Score: " << getSomaticScore() << "/21" << endl;
            cout << "Severity Level: " << getSeverityLevel() << endl;
            cout << "High Risk: " << (isHighRiskScore(getTotalScore()) ? "Yes" : "No") << endl;
        }

        string BeckAnxietyInventory::toString() const {
            stringstream ss;
            ss << "BAI{ID:" << m_bai_id << ",CaseID:" << m_case_profile_id 
               << ",Score:" << getTotalScore() << ",Level:" << getSeverityLevel() << "}";
            return ss.str();
        }

        // Static methods
        void BeckAnxietyInventory::resetIdCounter() {
            bai_id_counter = 0;
            utils::logMessage("INFO", "BAI ID counter reset to 0");
        }

        int BeckAnxietyInventory::getNextId() {
            return ID_PREFIX + (bai_id_counter + 1);
        }

        string BeckAnxietyInventory::interpretScore(int total_score) {
            if (total_score >= 0 && total_score <= 7) {
                return "Minimal";
            } else if (total_score >= 8 && total_score <= 15) {
                return "Mild";
            } else if (total_score >= 16 && total_score <= 25) {
                return "Moderate";
            } else if (total_score >= 26 && total_score <= 63) {
                return "Severe";
            } else {
                return "Invalid";
            }
        }

        bool BeckAnxietyInventory::isHighRiskScore(int total_score) {
            return total_score >= 16; // Moderate or Severe anxiety (clinical attention)
        }

        // Stream operators
        std::ostream& operator<<(std::ostream& os, const BeckAnxietyInventory& bai) {
            os << bai.toString();
            return os;
        }

        std::istream& operator>>(std::istream& is, BeckAnxietyInventory& bai) {
            // Basic input implementation for serialization
            is >> bai.m_bai_id >> bai.m_case_profile_id;
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
