#ifndef AUTOMOBILEANXIETYINVENTORY_H
#define AUTOMOBILEANXIETYINVENTORY_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class AutomobileAnxietyInventory {
        private:
            static const int MAX_TEXT_LENGTH = 500;
            static const string FORM_TYPE;     // Form type identifier: "AAI"
            
            // Primary attributes
            int m_aai_id;                      // AAI ID (700000+)
            int m_case_profile_id;             // Foreign key to case_profile
            string m_type;                     // Form type (default: "AAI")
            
            // Questions 1-13 (simple yes/no)
            bool m_question_1;                 // Are you more nervous in automobile since accident?
            bool m_question_2;                 // Do you have motor vehicle license?
            bool m_question_3;                 // Have you driven car since accident?
            bool m_question_4;                 // Do you avoid driving whenever possible?
            bool m_question_5;                 // Drive slower/more carefully since accident?
            bool m_question_6;                 // Have you been passenger since accident?
            bool m_question_7;                 // Avoid being passenger whenever possible?
            bool m_question_8;                 // More nervous in car after nightfall?
            bool m_question_9;                 // More nervous in traffic?
            bool m_question_10;                // More nervous traveling on highway?
            bool m_question_11;                // More nervous in intersection?
            bool m_question_12;                // More nervous outside immediate home area?
            bool m_question_13;                // More nervous in rain/bad weather?
            
            // Question 14 (multiple choice)
            bool m_question_14_driver;         // More nervous as a driver
            bool m_question_14_passenger;      // More nervous as a passenger
            bool m_question_14_no_difference;  // No difference
            
            // Question 15 (yes/no + text)
            bool m_question_15_a;              // More nervous when certain people in car?
            string m_question_15_b;            // If yes, who?
            
            // Questions 16-18 (simple yes/no)
            bool m_question_16;                // More nervous when other vehicles nearby?
            bool m_question_17;                // More nervous hearing car horn/brakes?
            bool m_question_18;                // More nervous passing accident scene?
            
            // Question 19 (yes/no + checkboxes)
            bool m_question_19;                // More nervous as pedestrian?
            bool m_question_19_sidewalks;      // More nervous on sidewalks
            bool m_question_19_crossing;       // More nervous crossing streets
            bool m_question_19_both;           // More nervous on both
            
            // Questions 20-23 (simple yes/no)
            bool m_question_20;                // Keep thinking about accident in car?
            bool m_question_21;                // Feel about to have another accident?
            bool m_question_22;                // Never regain former confidence?
            bool m_question_23;                // Have panic attacks?
            
            DateTime m_aai_createdAt;          // Creation timestamp
            DateTime m_aai_updatedAt;          // Last update timestamp
            
            static int aai_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextAAIId();

        public:
            // Constructors
            AutomobileAnxietyInventory();
            AutomobileAnxietyInventory(int case_profile_id);
            AutomobileAnxietyInventory(int aai_id, int case_profile_id, 
                                     bool q1, bool q2, bool q3, bool q4, bool q5,
                                     bool q6, bool q7, bool q8, bool q9, bool q10,
                                     bool q11, bool q12, bool q13,
                                     bool q14_driver, bool q14_passenger, bool q14_no_diff,
                                     bool q15_a, const string& q15_b,
                                     bool q16, bool q17, bool q18,
                                     bool q19, bool q19_sidewalks, bool q19_crossing, bool q19_both,
                                     bool q20, bool q21, bool q22, bool q23,
                                     const DateTime& createdAt, const DateTime& updatedAt);
            
            // Destructor
            ~AutomobileAnxietyInventory() = default;
            
            // Getters - Basic Info
            int getAAIId() const { return m_aai_id; }
            int getCaseProfileId() const { return m_case_profile_id; }
            string getType() const { return m_type; }
            DateTime getAAICreatedAt() const { return m_aai_createdAt; }
            DateTime getAAIUpdatedAt() const { return m_aai_updatedAt; }
            
            // Getters - Questions 1-13
            bool getQuestion1() const { return m_question_1; }
            bool getQuestion2() const { return m_question_2; }
            bool getQuestion3() const { return m_question_3; }
            bool getQuestion4() const { return m_question_4; }
            bool getQuestion5() const { return m_question_5; }
            bool getQuestion6() const { return m_question_6; }
            bool getQuestion7() const { return m_question_7; }
            bool getQuestion8() const { return m_question_8; }
            bool getQuestion9() const { return m_question_9; }
            bool getQuestion10() const { return m_question_10; }
            bool getQuestion11() const { return m_question_11; }
            bool getQuestion12() const { return m_question_12; }
            bool getQuestion13() const { return m_question_13; }
            
            // Getters - Question 14 (multiple choice)
            bool getQuestion14Driver() const { return m_question_14_driver; }
            bool getQuestion14Passenger() const { return m_question_14_passenger; }
            bool getQuestion14NoDifference() const { return m_question_14_no_difference; }
            string getQuestion14Response() const;
            
            // Getters - Question 15 (yes/no + text)
            bool getQuestion15A() const { return m_question_15_a; }
            string getQuestion15B() const { return m_question_15_b; }
            
            // Getters - Questions 16-18
            bool getQuestion16() const { return m_question_16; }
            bool getQuestion17() const { return m_question_17; }
            bool getQuestion18() const { return m_question_18; }
            
            // Getters - Question 19 (yes/no + checkboxes)
            bool getQuestion19() const { return m_question_19; }
            bool getQuestion19Sidewalks() const { return m_question_19_sidewalks; }
            bool getQuestion19Crossing() const { return m_question_19_crossing; }
            bool getQuestion19Both() const { return m_question_19_both; }
            string getQuestion19Options() const;
            
            // Getters - Questions 20-23
            bool getQuestion20() const { return m_question_20; }
            bool getQuestion21() const { return m_question_21; }
            bool getQuestion22() const { return m_question_22; }
            bool getQuestion23() const { return m_question_23; }
            
            // Setters with validation - Basic Info
            void setCaseProfileId(int case_profile_id) { m_case_profile_id = case_profile_id; updateTimestamp(); }
            void setAAICreatedAt(const DateTime& createdAt) { m_aai_createdAt = createdAt; }
            void setAAIUpdatedAt(const DateTime& updatedAt) { m_aai_updatedAt = updatedAt; }
            
            // Setters - Questions 1-13
            void setQuestion1(bool response) { m_question_1 = response; updateTimestamp(); }
            void setQuestion2(bool response) { m_question_2 = response; updateTimestamp(); }
            void setQuestion3(bool response) { m_question_3 = response; updateTimestamp(); }
            void setQuestion4(bool response) { m_question_4 = response; updateTimestamp(); }
            void setQuestion5(bool response) { m_question_5 = response; updateTimestamp(); }
            void setQuestion6(bool response) { m_question_6 = response; updateTimestamp(); }
            void setQuestion7(bool response) { m_question_7 = response; updateTimestamp(); }
            void setQuestion8(bool response) { m_question_8 = response; updateTimestamp(); }
            void setQuestion9(bool response) { m_question_9 = response; updateTimestamp(); }
            void setQuestion10(bool response) { m_question_10 = response; updateTimestamp(); }
            void setQuestion11(bool response) { m_question_11 = response; updateTimestamp(); }
            void setQuestion12(bool response) { m_question_12 = response; updateTimestamp(); }
            void setQuestion13(bool response) { m_question_13 = response; updateTimestamp(); }
            
            // Setters - Question 14 (multiple choice)
            void setQuestion14Driver(bool response) { m_question_14_driver = response; updateTimestamp(); }
            void setQuestion14Passenger(bool response) { m_question_14_passenger = response; updateTimestamp(); }
            void setQuestion14NoDifference(bool response) { m_question_14_no_difference = response; updateTimestamp(); }
            void setQuestion14Response(const string& option);
            
            // Setters - Question 15 (yes/no + text)
            void setQuestion15A(bool response) { m_question_15_a = response; updateTimestamp(); }
            void setQuestion15B(const string& text);
            
            // Setters - Questions 16-18
            void setQuestion16(bool response) { m_question_16 = response; updateTimestamp(); }
            void setQuestion17(bool response) { m_question_17 = response; updateTimestamp(); }
            void setQuestion18(bool response) { m_question_18 = response; updateTimestamp(); }
            
            // Setters - Question 19 (yes/no + checkboxes)
            void setQuestion19(bool response) { m_question_19 = response; updateTimestamp(); }
            void setQuestion19Sidewalks(bool response) { m_question_19_sidewalks = response; updateTimestamp(); }
            void setQuestion19Crossing(bool response) { m_question_19_crossing = response; updateTimestamp(); }
            void setQuestion19Both(bool response) { m_question_19_both = response; updateTimestamp(); }
            void setQuestion19Options(bool sidewalks, bool crossing, bool both);
            
            // Setters - Questions 20-23
            void setQuestion20(bool response) { m_question_20 = response; updateTimestamp(); }
            void setQuestion21(bool response) { m_question_21 = response; updateTimestamp(); }
            void setQuestion22(bool response) { m_question_22 = response; updateTimestamp(); }
            void setQuestion23(bool response) { m_question_23 = response; updateTimestamp(); }
            
            // Validation methods
            bool isValidCaseProfileId(int case_profile_id) const;
            bool isValidQuestion15Text(const string& text) const;
            bool isValidData() const;
            
            // Utility methods
            int getTotalYesResponses() const;
            double getAnxietyScore() const;
            bool hasHighAnxiety() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            void displayAllResponses() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            static const int ID_PREFIX = 700000; // AAI IDs start at 700001
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const AutomobileAnxietyInventory& aai);
            friend std::istream& operator>>(std::istream& is, AutomobileAnxietyInventory& aai);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // AUTOMOBILEANXIETYINVENTORY_H
