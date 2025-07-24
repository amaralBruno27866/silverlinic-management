#include "forms/BeckDepressionInventory.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static member initialization
        const string BeckDepressionInventory::FORM_TYPE = "BDI";
        int BeckDepressionInventory::bdi_id_counter = 0;

        // Default constructor
        BeckDepressionInventory::BeckDepressionInventory() 
            : m_bdi_id(getNextBDIId()), m_case_profile_id(0), m_type(FORM_TYPE),
              m_question_1(0), m_question_2(0), m_question_3(0), m_question_4(0), m_question_5(0),
              m_question_6(0), m_question_7(0), m_question_8(0), m_question_9(0), m_question_10(0),
              m_question_11(0), m_question_12(0), m_question_13(0), m_question_14(0), m_question_15(0),
              m_question_16(0), m_question_17(0), m_question_18(0), m_question_19(0), m_question_20(0), m_question_21(0) {
            setTimestamps();
            utils::logMessage("INFO", "BeckDepressionInventory created with ID: " + to_string(m_bdi_id));
        }

        // Constructor with case profile ID
        BeckDepressionInventory::BeckDepressionInventory(int case_profile_id) 
            : m_bdi_id(getNextBDIId()), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(0), m_question_2(0), m_question_3(0), m_question_4(0), m_question_5(0),
              m_question_6(0), m_question_7(0), m_question_8(0), m_question_9(0), m_question_10(0),
              m_question_11(0), m_question_12(0), m_question_13(0), m_question_14(0), m_question_15(0),
              m_question_16(0), m_question_17(0), m_question_18(0), m_question_19(0), m_question_20(0), m_question_21(0) {
            setTimestamps();
            utils::logMessage("INFO", "BeckDepressionInventory created with ID: " + to_string(m_bdi_id) + 
                      ", Case Profile ID: " + to_string(case_profile_id));
        }

        // Full constructor
        BeckDepressionInventory::BeckDepressionInventory(int bdi_id, int case_profile_id, 
                                                       int q1, int q2, int q3, int q4, int q5,
                                                       int q6, int q7, int q8, int q9, int q10,
                                                       int q11, int q12, int q13, int q14, int q15,
                                                       int q16, int q17, int q18, int q19, int q20, int q21,
                                                       const DateTime& createdAt, const DateTime& updatedAt)
            : m_bdi_id(bdi_id), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(q1), m_question_2(q2), m_question_3(q3), m_question_4(q4), m_question_5(q5),
              m_question_6(q6), m_question_7(q7), m_question_8(q8), m_question_9(q9), m_question_10(q10),
              m_question_11(q11), m_question_12(q12), m_question_13(q13), m_question_14(q14), m_question_15(q15),
              m_question_16(q16), m_question_17(q17), m_question_18(q18), m_question_19(q19), m_question_20(q20), m_question_21(q21),
              m_bdi_createdAt(createdAt), m_bdi_updatedAt(updatedAt) {
            utils::logMessage("INFO", "BeckDepressionInventory loaded with ID: " + to_string(m_bdi_id));
        }

        // Private helper methods
        void BeckDepressionInventory::setTimestamps() {
            DateTime now = DateTime::now();
            m_bdi_createdAt = now;
            m_bdi_updatedAt = now;
        }

        void BeckDepressionInventory::updateTimestamp() {
            m_bdi_updatedAt = DateTime::now();
        }

        int BeckDepressionInventory::getNextBDIId() {
            return ID_PREFIX + (++bdi_id_counter);
        }

        // Setters with validation
        void BeckDepressionInventory::setQuestion1(int value) {
            if (isValidQuestionValue(value)) {
                m_question_1 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 1: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion2(int value) {
            if (isValidQuestionValue(value)) {
                m_question_2 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 2: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion3(int value) {
            if (isValidQuestionValue(value)) {
                m_question_3 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 3: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion4(int value) {
            if (isValidQuestionValue(value)) {
                m_question_4 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 4: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion5(int value) {
            if (isValidQuestionValue(value)) {
                m_question_5 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 5: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion6(int value) {
            if (isValidQuestionValue(value)) {
                m_question_6 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 6: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion7(int value) {
            if (isValidQuestionValue(value)) {
                m_question_7 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 7: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion8(int value) {
            if (isValidQuestionValue(value)) {
                m_question_8 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 8: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion9(int value) {
            if (isValidQuestionValue(value)) {
                m_question_9 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 9: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion10(int value) {
            if (isValidQuestionValue(value)) {
                m_question_10 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 10: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion11(int value) {
            if (isValidQuestionValue(value)) {
                m_question_11 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 11: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion12(int value) {
            if (isValidQuestionValue(value)) {
                m_question_12 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 12: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion13(int value) {
            if (isValidQuestionValue(value)) {
                m_question_13 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 13: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion14(int value) {
            if (isValidQuestionValue(value)) {
                m_question_14 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 14: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion15(int value) {
            if (isValidQuestionValue(value)) {
                m_question_15 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 15: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion16(int value) {
            if (isValidQuestionValue(value)) {
                m_question_16 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 16: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion17(int value) {
            if (isValidQuestionValue(value)) {
                m_question_17 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 17: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion18(int value) {
            if (isValidQuestionValue(value)) {
                m_question_18 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 18: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion19(int value) {
            if (isValidQuestionValue(value)) {
                m_question_19 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 19: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion20(int value) {
            if (isValidQuestionValue(value)) {
                m_question_20 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 20: " + to_string(value) + ". Must be 0-3.");
            }
        }

        void BeckDepressionInventory::setQuestion21(int value) {
            if (isValidQuestionValue(value)) {
                m_question_21 = value;
                updateTimestamp();
            } else {
                utils::logMessage("WARNING", "Invalid value for Question 21: " + to_string(value) + ". Must be 0-3.");
            }
        }

        // Validation methods
        bool BeckDepressionInventory::isValidCaseProfileId(int case_profile_id) const {
            return case_profile_id > 0;
        }

        bool BeckDepressionInventory::isValidQuestionValue(int value) const {
            return value >= 0 && value <= 3;
        }

        bool BeckDepressionInventory::isValidData() const {
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
        int BeckDepressionInventory::getTotalScore() const {
            return m_question_1 + m_question_2 + m_question_3 + m_question_4 + m_question_5 +
                   m_question_6 + m_question_7 + m_question_8 + m_question_9 + m_question_10 +
                   m_question_11 + m_question_12 + m_question_13 + m_question_14 + m_question_15 +
                   m_question_16 + m_question_17 + m_question_18 + m_question_19 + m_question_20 + m_question_21;
        }

        double BeckDepressionInventory::getDepressionPercentage() const {
            int total = getTotalScore();
            int max_possible = 21 * 3; // 21 questions * 3 points maximum each
            return (total * 100.0) / max_possible;
        }

        string BeckDepressionInventory::getSeverityLevel() const {
            return interpretScore(getTotalScore());
        }

        bool BeckDepressionInventory::hasMinimalDepression() const {
            int score = getTotalScore();
            return score >= 0 && score <= 13;
        }

        bool BeckDepressionInventory::hasMildDepression() const {
            int score = getTotalScore();
            return score >= 14 && score <= 19;
        }

        bool BeckDepressionInventory::hasModerateDepression() const {
            int score = getTotalScore();
            return score >= 20 && score <= 28;
        }

        bool BeckDepressionInventory::hasSevereDepression() const {
            int score = getTotalScore();
            return score >= 29 && score <= 63;
        }

        // Question category analysis
        int BeckDepressionInventory::getCognitiveScore() const {
            // Cognitive symptoms: Questions 2, 3, 7, 8, 9, 13, 14
            return m_question_2 + m_question_3 + m_question_7 + m_question_8 + 
                   m_question_9 + m_question_13 + m_question_14;
        }

        int BeckDepressionInventory::getAffectiveScore() const {
            // Affective symptoms: Questions 1, 4, 5, 6, 10, 12, 17
            return m_question_1 + m_question_4 + m_question_5 + m_question_6 + 
                   m_question_10 + m_question_12 + m_question_17;
        }

        int BeckDepressionInventory::getSomaticScore() const {
            // Somatic symptoms: Questions 11, 15, 16, 18, 19, 20, 21
            return m_question_11 + m_question_15 + m_question_16 + m_question_18 + 
                   m_question_19 + m_question_20 + m_question_21;
        }

        // Display methods
        void BeckDepressionInventory::displayInfo() const {
            cout << "\n=== Beck Depression Inventory (BDI-II) ===" << endl;
            cout << "BDI ID: " << m_bdi_id << endl;
            cout << "Case Profile ID: " << m_case_profile_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Created: " << m_bdi_createdAt.toString() << endl;
            cout << "Updated: " << m_bdi_updatedAt.toString() << endl;
            cout << "Total Score: " << getTotalScore() << "/63" << endl;
            cout << "Depression Percentage: " << fixed << setprecision(1) << getDepressionPercentage() << "%" << endl;
            cout << "Severity Level: " << getSeverityLevel() << endl;
        }

        void BeckDepressionInventory::displaySummary() const {
            cout << "\n=== BDI Summary ===" << endl;
            cout << "ID: " << m_bdi_id << " | Score: " << getTotalScore() << "/63 | ";
            cout << "Level: " << getSeverityLevel() << " (" << fixed << setprecision(1) << getDepressionPercentage() << "%)" << endl;
        }

        void BeckDepressionInventory::displayAllResponses() const {
            cout << "\n=== All BDI Responses ===" << endl;
            cout << "Q1 (Sadness): " << m_question_1 << endl;
            cout << "Q2 (Pessimism): " << m_question_2 << endl;
            cout << "Q3 (Past Failure): " << m_question_3 << endl;
            cout << "Q4 (Loss of Pleasure): " << m_question_4 << endl;
            cout << "Q5 (Guilty Feelings): " << m_question_5 << endl;
            cout << "Q6 (Punishment Feelings): " << m_question_6 << endl;
            cout << "Q7 (Self-Dislike): " << m_question_7 << endl;
            cout << "Q8 (Self-Criticalness): " << m_question_8 << endl;
            cout << "Q9 (Suicidal Thoughts): " << m_question_9 << endl;
            cout << "Q10 (Crying): " << m_question_10 << endl;
            cout << "Q11 (Agitation): " << m_question_11 << endl;
            cout << "Q12 (Loss of Interest): " << m_question_12 << endl;
            cout << "Q13 (Indecisiveness): " << m_question_13 << endl;
            cout << "Q14 (Worthlessness): " << m_question_14 << endl;
            cout << "Q15 (Loss of Energy): " << m_question_15 << endl;
            cout << "Q16 (Changes in Sleeping): " << m_question_16 << endl;
            cout << "Q17 (Irritability): " << m_question_17 << endl;
            cout << "Q18 (Changes in Appetite): " << m_question_18 << endl;
            cout << "Q19 (Concentration Difficulty): " << m_question_19 << endl;
            cout << "Q20 (Tiredness or Fatigue): " << m_question_20 << endl;
            cout << "Q21 (Loss of Interest in Sex): " << m_question_21 << endl;
        }

        void BeckDepressionInventory::displayScoreAnalysis() const {
            cout << "\n=== BDI Score Analysis ===" << endl;
            cout << "Total Score: " << getTotalScore() << "/63" << endl;
            cout << "Cognitive Score: " << getCognitiveScore() << "/21" << endl;
            cout << "Affective Score: " << getAffectiveScore() << "/21" << endl;
            cout << "Somatic Score: " << getSomaticScore() << "/21" << endl;
            cout << "Severity Level: " << getSeverityLevel() << endl;
            cout << "High Risk: " << (isHighRiskScore(getTotalScore()) ? "Yes" : "No") << endl;
        }

        string BeckDepressionInventory::toString() const {
            stringstream ss;
            ss << "BDI{ID:" << m_bdi_id << ",CaseID:" << m_case_profile_id 
               << ",Score:" << getTotalScore() << ",Level:" << getSeverityLevel() << "}";
            return ss.str();
        }

        // Static methods
        void BeckDepressionInventory::resetIdCounter() {
            bdi_id_counter = 0;
            utils::logMessage("INFO", "BDI ID counter reset to 0");
        }

        int BeckDepressionInventory::getNextId() {
            return ID_PREFIX + (bdi_id_counter + 1);
        }

        string BeckDepressionInventory::interpretScore(int total_score) {
            if (total_score >= 0 && total_score <= 13) {
                return "Minimal";
            } else if (total_score >= 14 && total_score <= 19) {
                return "Mild";
            } else if (total_score >= 20 && total_score <= 28) {
                return "Moderate";
            } else if (total_score >= 29 && total_score <= 63) {
                return "Severe";
            } else {
                return "Invalid";
            }
        }

        bool BeckDepressionInventory::isHighRiskScore(int total_score) {
            return total_score >= 20; // Moderate or Severe depression
        }

        // Stream operators
        std::ostream& operator<<(std::ostream& os, const BeckDepressionInventory& bdi) {
            os << bdi.toString();
            return os;
        }

        std::istream& operator>>(std::istream& is, BeckDepressionInventory& bdi) {
            // Basic input implementation for serialization
            is >> bdi.m_bdi_id >> bdi.m_case_profile_id;
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
