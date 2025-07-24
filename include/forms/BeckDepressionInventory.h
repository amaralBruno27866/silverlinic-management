#ifndef BECKDEPRESSIONINVENTORY_H
#define BECKDEPRESSIONINVENTORY_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class BeckDepressionInventory {
        private:
            static const string FORM_TYPE;     // Form type identifier: "BDI"
            
            // Primary attributes
            int m_bdi_id;                      // BDI ID (800000+)
            int m_case_profile_id;             // Foreign key to case_profile
            string m_type;                     // Form type (default: "BDI")
            
            // Clinical Questions (21 questions - BDI-II)
            int m_question_1;                  // Sadness (0-3)
            int m_question_2;                  // Pessimism (0-3)
            int m_question_3;                  // Past Failure (0-3)
            int m_question_4;                  // Loss of Pleasure (0-3)
            int m_question_5;                  // Guilty Feelings (0-3)
            int m_question_6;                  // Punishment Feelings (0-3)
            int m_question_7;                  // Self-Dislike (0-3)
            int m_question_8;                  // Self-Criticalness (0-3)
            int m_question_9;                  // Suicidal Thoughts or Wishes (0-3)
            int m_question_10;                 // Crying (0-3)
            int m_question_11;                 // Agitation (0-3)
            int m_question_12;                 // Loss of Interest (0-3)
            int m_question_13;                 // Indecisiveness (0-3)
            int m_question_14;                 // Worthlessness (0-3)
            int m_question_15;                 // Loss of Energy (0-3)
            int m_question_16;                 // Changes in Sleeping Pattern (0-3)
            int m_question_17;                 // Irritability (0-3)
            int m_question_18;                 // Changes in Appetite (0-3)
            int m_question_19;                 // Concentration Difficulty (0-3)
            int m_question_20;                 // Tiredness or Fatigue (0-3)
            int m_question_21;                 // Loss of Interest in Sex (0-3)
            
            DateTime m_bdi_createdAt;          // Creation timestamp
            DateTime m_bdi_updatedAt;          // Last update timestamp
            
            static int bdi_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextBDIId();

        public:
            // Constructors
            BeckDepressionInventory();
            BeckDepressionInventory(int case_profile_id);
            BeckDepressionInventory(int bdi_id, int case_profile_id, 
                                  int q1, int q2, int q3, int q4, int q5,
                                  int q6, int q7, int q8, int q9, int q10,
                                  int q11, int q12, int q13, int q14, int q15,
                                  int q16, int q17, int q18, int q19, int q20, int q21,
                                  const DateTime& createdAt, const DateTime& updatedAt);
            
            // Destructor
            ~BeckDepressionInventory() = default;
            
            // Getters - Basic Info
            int getBDIId() const { return m_bdi_id; }
            int getCaseProfileId() const { return m_case_profile_id; }
            string getType() const { return m_type; }
            DateTime getBDICreatedAt() const { return m_bdi_createdAt; }
            DateTime getBDIUpdatedAt() const { return m_bdi_updatedAt; }
            
            // Getters - Clinical Questions
            int getQuestion1() const { return m_question_1; }
            int getQuestion2() const { return m_question_2; }
            int getQuestion3() const { return m_question_3; }
            int getQuestion4() const { return m_question_4; }
            int getQuestion5() const { return m_question_5; }
            int getQuestion6() const { return m_question_6; }
            int getQuestion7() const { return m_question_7; }
            int getQuestion8() const { return m_question_8; }
            int getQuestion9() const { return m_question_9; }
            int getQuestion10() const { return m_question_10; }
            int getQuestion11() const { return m_question_11; }
            int getQuestion12() const { return m_question_12; }
            int getQuestion13() const { return m_question_13; }
            int getQuestion14() const { return m_question_14; }
            int getQuestion15() const { return m_question_15; }
            int getQuestion16() const { return m_question_16; }
            int getQuestion17() const { return m_question_17; }
            int getQuestion18() const { return m_question_18; }
            int getQuestion19() const { return m_question_19; }
            int getQuestion20() const { return m_question_20; }
            int getQuestion21() const { return m_question_21; }
            
            // Setters with validation - Basic Info
            void setCaseProfileId(int case_profile_id) { m_case_profile_id = case_profile_id; updateTimestamp(); }
            void setBDICreatedAt(const DateTime& createdAt) { m_bdi_createdAt = createdAt; }
            void setBDIUpdatedAt(const DateTime& updatedAt) { m_bdi_updatedAt = updatedAt; }
            
            // Setters - Clinical Questions (with validation 0-3)
            void setQuestion1(int value);
            void setQuestion2(int value);
            void setQuestion3(int value);
            void setQuestion4(int value);
            void setQuestion5(int value);
            void setQuestion6(int value);
            void setQuestion7(int value);
            void setQuestion8(int value);
            void setQuestion9(int value);
            void setQuestion10(int value);
            void setQuestion11(int value);
            void setQuestion12(int value);
            void setQuestion13(int value);
            void setQuestion14(int value);
            void setQuestion15(int value);
            void setQuestion16(int value);
            void setQuestion17(int value);
            void setQuestion18(int value);
            void setQuestion19(int value);
            void setQuestion20(int value);
            void setQuestion21(int value);
            
            // Validation methods
            bool isValidCaseProfileId(int case_profile_id) const;
            bool isValidQuestionValue(int value) const;
            bool isValidData() const;
            
            // Clinical scoring methods
            int getTotalScore() const;
            double getDepressionPercentage() const;
            string getSeverityLevel() const;
            bool hasMinimalDepression() const;
            bool hasMildDepression() const;
            bool hasModerateDepression() const;
            bool hasSevereDepression() const;
            
            // Question category analysis
            int getCognitiveScore() const;      // Questions related to cognitive symptoms
            int getAffectiveScore() const;      // Questions related to mood
            int getSomaticScore() const;        // Questions related to physical symptoms
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            void displayAllResponses() const;
            void displayScoreAnalysis() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            static const int ID_PREFIX = 800000; // BDI IDs start at 800001
            
            // Static clinical interpretation methods
            static string interpretScore(int total_score);
            static bool isHighRiskScore(int total_score);
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const BeckDepressionInventory& bdi);
            friend std::istream& operator>>(std::istream& is, BeckDepressionInventory& bdi);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // BECKDEPRESSIONINVENTORY_H
