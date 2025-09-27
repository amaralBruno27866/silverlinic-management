#ifndef BECKANXIETYINVENTORY_H
#define BECKANXIETYINVENTORY_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class BeckAnxietyInventory {
        private:
            static const string FORM_TYPE;     // Form type identifier: "BAI"
            
            // Primary attributes
            int m_bai_id;                      // BAI ID (900000+)
            int m_case_profile_id;             // Foreign key to case_profile
            string m_type;                     // Form type (default: "BAI")
            
            // Clinical Questions (21 questions - Beck Anxiety Inventory)
            // NOTE: Each item scored 0-3 (0 Not at all, 1 Mildly, 2 Moderately, 3 Severely - could barely stand it)
            int m_question_1;                  // 1. Numbness or tingling (0-3)
            int m_question_2;                  // 2. Feeling hot (0-3)
            int m_question_3;                  // 3. Wobbliness in legs (0-3)
            int m_question_4;                  // 4. Unable to relax (0-3)
            int m_question_5;                  // 5. Fear of worst happening (0-3)
            int m_question_6;                  // 6. Dizzy or lightheaded (0-3)
            int m_question_7;                  // 7. Heart pounding / racing (0-3)
            int m_question_8;                  // 8. Unsteady (0-3)
            int m_question_9;                  // 9. Terrified or afraid (0-3)
            int m_question_10;                 // 10. Nervous (0-3)
            int m_question_11;                 // 11. Feeling of choking (0-3)
            int m_question_12;                 // 12. Hands trembling (0-3)
            int m_question_13;                 // 13. Shaky / unsteady (0-3)
            int m_question_14;                 // 14. Fear of losing control (0-3)
            int m_question_15;                 // 15. Difficulty in breathing (0-3)
            int m_question_16;                 // 16. Fear of dying (0-3)
            int m_question_17;                 // 17. Scared (0-3)
            int m_question_18;                 // 18. Indigestion (0-3)
            int m_question_19;                 // 19. Faint / lightheaded (0-3)
            int m_question_20;                 // 20. Face flushed (0-3)
            int m_question_21;                 // 21. Hot / cold sweats (0-3)
            
            DateTime m_bai_createdAt;          // Creation timestamp
            DateTime m_bai_updatedAt;          // Last update timestamp
            
            static int bai_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextBAIId();

        public:
            // Constructors
            BeckAnxietyInventory();
            BeckAnxietyInventory(int case_profile_id);
            BeckAnxietyInventory(int bai_id, int case_profile_id, 
                               int q1, int q2, int q3, int q4, int q5,
                               int q6, int q7, int q8, int q9, int q10,
                               int q11, int q12, int q13, int q14, int q15,
                               int q16, int q17, int q18, int q19, int q20, int q21,
                               const DateTime& createdAt, const DateTime& updatedAt);
            
            // Destructor
            ~BeckAnxietyInventory() = default;
            
            // Getters - Basic Info
            int getBAIId() const { return m_bai_id; }
            int getCaseProfileId() const { return m_case_profile_id; }
            string getType() const { return m_type; }
            DateTime getBAICreatedAt() const { return m_bai_createdAt; }
            DateTime getBAIUpdatedAt() const { return m_bai_updatedAt; }
            
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
            void setBAICreatedAt(const DateTime& createdAt) { m_bai_createdAt = createdAt; }
            void setBAIUpdatedAt(const DateTime& updatedAt) { m_bai_updatedAt = updatedAt; }
            
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
            double getAnxietyPercentage() const;
            string getSeverityLevel() const;
            // Severity helpers (BAI standard thresholds)
            // Minimal: 0-7 | Mild: 8-15 | Moderate: 16-25 | Severe: 26-63
            bool hasMinimalAnxiety() const;    // 0-7
            bool hasMildAnxiety() const;       // 8-15
            bool hasModerateAnxiety() const;   // 16-25
            bool hasSevereAnxiety() const;     // 26-63
            
            // Question category analysis (adapted for anxiety symptoms)
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
            // Sequential IDs starting from 1
            
            // Static clinical interpretation methods
            static string interpretScore(int total_score);
            static bool isHighRiskScore(int total_score);
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const BeckAnxietyInventory& bai);
            friend std::istream& operator>>(std::istream& is, BeckAnxietyInventory& bai);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // BECKANXIETYINVENTORY_H
