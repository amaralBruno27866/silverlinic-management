#ifndef SCL90R_H
#define SCL90R_H

#include "core/DateTime.h"
#include "core/CaseProfile.h"
#include "core/Utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        /**
         * @class SCL90R
         * @brief SCL-90-R (Symptom Checklist-90-Revised) psychological assessment tool
         * 
         * The SCL-90-R is a 90-item psychological assessment instrument that evaluates
         * a broad range of psychological problems and symptoms. Each item is rated on a
         * 4-point scale from 0 (not at all) to 3 (extremely).
         * 
         * Clinical Dimensions:
         * - Somatization (SOM): 12 items
         * - Obsession-Compulsion (O-C): 10 items  
         * - Interpersonal Sensitivity (I-S): 9 items
         * - Depression (DEP): 13 items
         * - Anxiety (ANX): 10 items
         * - Hostility (HOS): 6 items
         * - Phobic Anxiety (PHOB): 7 items
         * - Paranoid Ideation (PAR): 6 items
         * - Psychoticism (PSY): 10 items
         * 
         * @author SilverClinic Team
         * @date 2025
         */
        class SCL90R {
        private:
            // Core identification
            int m_scl_id;                    // Internal unique ID (will map to DB column 'id')
            int m_case_profile_id;           // Foreign key to case_profile table
            string m_type;                   // Form type identifier "SCL90R"
            
            // 90 questionnaire items (all rated 0-3)
            int m_question_1;    // Headaches
            int m_question_2;    // Nervousness or shakiness inside
            int m_question_3;    // Repeated unpleasant thoughts that won't leave your mind
            int m_question_4;    // Faintness or dizziness
            int m_question_5;    // Loss of sexual interest or pleasure
            int m_question_6;    // Feeling critical of others
            int m_question_7;    // The idea that someone else can control your thoughts
            int m_question_8;    // Feeling others are to blame for most of your troubles
            int m_question_9;    // Trouble remembering things
            int m_question_10;   // Worried about sloppiness or carelessness
            int m_question_11;   // Feeling easily annoyed or irritated
            int m_question_12;   // Pains in heart or chest
            int m_question_13;   // Feeling afraid in open places or in the street
            int m_question_14;   // Feeling low in energy or slowed down
            int m_question_15;   // Thoughts of ending your life
            int m_question_16;   // Hearing voices that other people do not hear
            int m_question_17;   // Trembling
            int m_question_18;   // Feeling that most people cannot be trusted
            int m_question_19;   // Poor appetite
            int m_question_20;   // Crying easily
            int m_question_21;   // Feeling shy or uneasy with opposite sex
            int m_question_22;   // Feelings of being trapped or caught
            int m_question_23;   // Suddenly scared for no reason
            int m_question_24;   // Temper outbursts that you could not control
            int m_question_25;   // Feeling afraid to go out of your house alone
            int m_question_26;   // Blaming yourself for things
            int m_question_27;   // Pains in lower back
            int m_question_28;   // Feeling blocked in getting things done
            int m_question_29;   // Feeling lonely
            int m_question_30;   // Feeling blue
            int m_question_31;   // Worrying too much about things
            int m_question_32;   // Feeling no interest in things
            int m_question_33;   // Feeling fearful
            int m_question_34;   // Your feelings being easily hurt
            int m_question_35;   // Other people being aware of your private thoughts
            int m_question_36;   // Feelings others do not understand you or are unsympathetic
            int m_question_37;   // Feeling that people are unfriendly or dislike you
            int m_question_38;   // Having to do things very slowly to ensure correctness
            int m_question_39;   // Heart pounding or racing
            int m_question_40;   // Nausea or upset stomach
            int m_question_41;   // Feeling inferior to others
            int m_question_42;   // Soreness of your muscles
            int m_question_43;   // Feeling that you are watched or talked about by others
            int m_question_44;   // Trouble falling asleep
            int m_question_45;   // Having to check and double-check what you do
            int m_question_46;   // Difficulty making decisions
            int m_question_47;   // Feeling afraid to travel on buses, subways or trains
            int m_question_48;   // Trouble getting your breath
            int m_question_49;   // Hot or cold spells
            int m_question_50;   // Having to avoid certain things, places, or activities because they frighten you
            int m_question_51;   // Your mind going blank
            int m_question_52;   // Numbness or tingling in parts of your body
            int m_question_53;   // A lump in your throat
            int m_question_54;   // Feeling hopeless about the future
            int m_question_55;   // Trouble concentrating
            int m_question_56;   // Feeling weak in parts of your body
            int m_question_57;   // Feeling tensed or keyed up
            int m_question_58;   // Heavy feelings in your arms and legs
            int m_question_59;   // Thoughts of death and dying
            int m_question_60;   // Overeating
            int m_question_61;   // Feeling uneasy when people are watching or talking about you
            int m_question_62;   // Having thoughts that are not your own
            int m_question_63;   // Having urges to beat, injure, or harm someone
            int m_question_64;   // Awakening in early morning
            int m_question_65;   // Having to repeat the same actions such as touching, counting, or washing
            int m_question_66;   // Sleep that is restless or disturbed
            int m_question_67;   // Having urges to break or smash things
            int m_question_68;   // Having beliefs or ideas that others do not share
            int m_question_69;   // Feeling very self-conscious with others
            int m_question_70;   // Feeling uneasy in crowds, such as shopping or at a movie
            int m_question_71;   // Feeling everything is an effort
            int m_question_72;   // Spells of terror or panic
            int m_question_73;   // Feeling uncomfortable about eating or drinking in public
            int m_question_74;   // Getting into frequent arguments
            int m_question_75;   // Feeling nervous when you are left alone
            int m_question_76;   // Others not giving you proper credit for your achievements
            int m_question_77;   // Feeling lonely even when you are with people
            int m_question_78;   // Feeling so restless that you couldn't sit still
            int m_question_79;   // Feelings of worthlessness
            int m_question_80;   // The feeling that something bad is going to happen to you
            int m_question_81;   // Shouting or throwing things
            int m_question_82;   // Feeling afraid that you will faint in public
            int m_question_83;   // Feeling that other people will take advantage of you if you let them
            int m_question_84;   // Having thoughts about sex that bother you a lot
            int m_question_85;   // The idea that you should be punished for your sins
            int m_question_86;   // Thoughts and images of a frightening nature
            int m_question_87;   // The idea that something serious is wrong with your body
            int m_question_88;   // Never feeling close to another person
            int m_question_89;   // Feelings of guilt
            int m_question_90;   // The idea that something is wrong with your mind
            
            // Timestamps
            DateTime m_scl_createdAt; // Maps to created_at
            DateTime m_scl_updatedAt; // Maps to modified_at
            
            // Static members
            static int scl_id_counter;
            
            // Private helper methods
            void setTimestamps();
            void updateTimestamp();
            int getNextSCLId();
            
        public:
            // Static constants
            static const int ID_PREFIX = 1200000; // SCL-90-R IDs start at 1200001
            static const string FORM_TYPE;
            
            // Constructors
            SCL90R();
            SCL90R(int case_profile_id);
            SCL90R(int scl_id, int case_profile_id, 
                   int q1, int q2, int q3, int q4, int q5, int q6, int q7, int q8, int q9, int q10,
                   int q11, int q12, int q13, int q14, int q15, int q16, int q17, int q18, int q19, int q20,
                   int q21, int q22, int q23, int q24, int q25, int q26, int q27, int q28, int q29, int q30,
                   int q31, int q32, int q33, int q34, int q35, int q36, int q37, int q38, int q39, int q40,
                   int q41, int q42, int q43, int q44, int q45, int q46, int q47, int q48, int q49, int q50,
                   int q51, int q52, int q53, int q54, int q55, int q56, int q57, int q58, int q59, int q60,
                   int q61, int q62, int q63, int q64, int q65, int q66, int q67, int q68, int q69, int q70,
                   int q71, int q72, int q73, int q74, int q75, int q76, int q77, int q78, int q79, int q80,
                   int q81, int q82, int q83, int q84, int q85, int q86, int q87, int q88, int q89, int q90,
                   const DateTime& createdAt, const DateTime& updatedAt);
            
            // Destructor
            ~SCL90R() = default;
            
            // Getters
            int getSCLId() const { return m_scl_id; }
            int getCaseProfileId() const { return m_case_profile_id; }
            string getType() const { return m_type; }
            DateTime getCreatedAt() const { return m_scl_createdAt; }
            DateTime getUpdatedAt() const { return m_scl_updatedAt; }
            // Computed cached metrics (not persisted here, but columns exist in DB for ingestion)
            int computeGSI() const { return getGlobalSeverityIndex(); }
            int computePST() const { return getPositiveSymptomTotal(); }
            double computePSDI() const { return getPositiveSymptomDistressIndex(); }
            
            // Individual question getters
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
            int getQuestion22() const { return m_question_22; }
            int getQuestion23() const { return m_question_23; }
            int getQuestion24() const { return m_question_24; }
            int getQuestion25() const { return m_question_25; }
            int getQuestion26() const { return m_question_26; }
            int getQuestion27() const { return m_question_27; }
            int getQuestion28() const { return m_question_28; }
            int getQuestion29() const { return m_question_29; }
            int getQuestion30() const { return m_question_30; }
            int getQuestion31() const { return m_question_31; }
            int getQuestion32() const { return m_question_32; }
            int getQuestion33() const { return m_question_33; }
            int getQuestion34() const { return m_question_34; }
            int getQuestion35() const { return m_question_35; }
            int getQuestion36() const { return m_question_36; }
            int getQuestion37() const { return m_question_37; }
            int getQuestion38() const { return m_question_38; }
            int getQuestion39() const { return m_question_39; }
            int getQuestion40() const { return m_question_40; }
            int getQuestion41() const { return m_question_41; }
            int getQuestion42() const { return m_question_42; }
            int getQuestion43() const { return m_question_43; }
            int getQuestion44() const { return m_question_44; }
            int getQuestion45() const { return m_question_45; }
            int getQuestion46() const { return m_question_46; }
            int getQuestion47() const { return m_question_47; }
            int getQuestion48() const { return m_question_48; }
            int getQuestion49() const { return m_question_49; }
            int getQuestion50() const { return m_question_50; }
            int getQuestion51() const { return m_question_51; }
            int getQuestion52() const { return m_question_52; }
            int getQuestion53() const { return m_question_53; }
            int getQuestion54() const { return m_question_54; }
            int getQuestion55() const { return m_question_55; }
            int getQuestion56() const { return m_question_56; }
            int getQuestion57() const { return m_question_57; }
            int getQuestion58() const { return m_question_58; }
            int getQuestion59() const { return m_question_59; }
            int getQuestion60() const { return m_question_60; }
            int getQuestion61() const { return m_question_61; }
            int getQuestion62() const { return m_question_62; }
            int getQuestion63() const { return m_question_63; }
            int getQuestion64() const { return m_question_64; }
            int getQuestion65() const { return m_question_65; }
            int getQuestion66() const { return m_question_66; }
            int getQuestion67() const { return m_question_67; }
            int getQuestion68() const { return m_question_68; }
            int getQuestion69() const { return m_question_69; }
            int getQuestion70() const { return m_question_70; }
            int getQuestion71() const { return m_question_71; }
            int getQuestion72() const { return m_question_72; }
            int getQuestion73() const { return m_question_73; }
            int getQuestion74() const { return m_question_74; }
            int getQuestion75() const { return m_question_75; }
            int getQuestion76() const { return m_question_76; }
            int getQuestion77() const { return m_question_77; }
            int getQuestion78() const { return m_question_78; }
            int getQuestion79() const { return m_question_79; }
            int getQuestion80() const { return m_question_80; }
            int getQuestion81() const { return m_question_81; }
            int getQuestion82() const { return m_question_82; }
            int getQuestion83() const { return m_question_83; }
            int getQuestion84() const { return m_question_84; }
            int getQuestion85() const { return m_question_85; }
            int getQuestion86() const { return m_question_86; }
            int getQuestion87() const { return m_question_87; }
            int getQuestion88() const { return m_question_88; }
            int getQuestion89() const { return m_question_89; }
            int getQuestion90() const { return m_question_90; }
            
            // Generic question getter
            int getQuestion(int questionNumber) const;
            
            // Setters for individual questions
            void setQuestion1(int value) { setQuestionValue(1, value); }
            void setQuestion2(int value) { setQuestionValue(2, value); }
            void setQuestion3(int value) { setQuestionValue(3, value); }
            void setQuestion4(int value) { setQuestionValue(4, value); }
            void setQuestion5(int value) { setQuestionValue(5, value); }
            void setQuestion6(int value) { setQuestionValue(6, value); }
            void setQuestion7(int value) { setQuestionValue(7, value); }
            void setQuestion8(int value) { setQuestionValue(8, value); }
            void setQuestion9(int value) { setQuestionValue(9, value); }
            void setQuestion10(int value) { setQuestionValue(10, value); }
            void setQuestion11(int value) { setQuestionValue(11, value); }
            void setQuestion12(int value) { setQuestionValue(12, value); }
            void setQuestion13(int value) { setQuestionValue(13, value); }
            void setQuestion14(int value) { setQuestionValue(14, value); }
            void setQuestion15(int value) { setQuestionValue(15, value); }
            void setQuestion16(int value) { setQuestionValue(16, value); }
            void setQuestion17(int value) { setQuestionValue(17, value); }
            void setQuestion18(int value) { setQuestionValue(18, value); }
            void setQuestion19(int value) { setQuestionValue(19, value); }
            void setQuestion20(int value) { setQuestionValue(20, value); }
            void setQuestion21(int value) { setQuestionValue(21, value); }
            void setQuestion22(int value) { setQuestionValue(22, value); }
            void setQuestion23(int value) { setQuestionValue(23, value); }
            void setQuestion24(int value) { setQuestionValue(24, value); }
            void setQuestion25(int value) { setQuestionValue(25, value); }
            void setQuestion26(int value) { setQuestionValue(26, value); }
            void setQuestion27(int value) { setQuestionValue(27, value); }
            void setQuestion28(int value) { setQuestionValue(28, value); }
            void setQuestion29(int value) { setQuestionValue(29, value); }
            void setQuestion30(int value) { setQuestionValue(30, value); }
            void setQuestion31(int value) { setQuestionValue(31, value); }
            void setQuestion32(int value) { setQuestionValue(32, value); }
            void setQuestion33(int value) { setQuestionValue(33, value); }
            void setQuestion34(int value) { setQuestionValue(34, value); }
            void setQuestion35(int value) { setQuestionValue(35, value); }
            void setQuestion36(int value) { setQuestionValue(36, value); }
            void setQuestion37(int value) { setQuestionValue(37, value); }
            void setQuestion38(int value) { setQuestionValue(38, value); }
            void setQuestion39(int value) { setQuestionValue(39, value); }
            void setQuestion40(int value) { setQuestionValue(40, value); }
            void setQuestion41(int value) { setQuestionValue(41, value); }
            void setQuestion42(int value) { setQuestionValue(42, value); }
            void setQuestion43(int value) { setQuestionValue(43, value); }
            void setQuestion44(int value) { setQuestionValue(44, value); }
            void setQuestion45(int value) { setQuestionValue(45, value); }
            void setQuestion46(int value) { setQuestionValue(46, value); }
            void setQuestion47(int value) { setQuestionValue(47, value); }
            void setQuestion48(int value) { setQuestionValue(48, value); }
            void setQuestion49(int value) { setQuestionValue(49, value); }
            void setQuestion50(int value) { setQuestionValue(50, value); }
            void setQuestion51(int value) { setQuestionValue(51, value); }
            void setQuestion52(int value) { setQuestionValue(52, value); }
            void setQuestion53(int value) { setQuestionValue(53, value); }
            void setQuestion54(int value) { setQuestionValue(54, value); }
            void setQuestion55(int value) { setQuestionValue(55, value); }
            void setQuestion56(int value) { setQuestionValue(56, value); }
            void setQuestion57(int value) { setQuestionValue(57, value); }
            void setQuestion58(int value) { setQuestionValue(58, value); }
            void setQuestion59(int value) { setQuestionValue(59, value); }
            void setQuestion60(int value) { setQuestionValue(60, value); }
            void setQuestion61(int value) { setQuestionValue(61, value); }
            void setQuestion62(int value) { setQuestionValue(62, value); }
            void setQuestion63(int value) { setQuestionValue(63, value); }
            void setQuestion64(int value) { setQuestionValue(64, value); }
            void setQuestion65(int value) { setQuestionValue(65, value); }
            void setQuestion66(int value) { setQuestionValue(66, value); }
            void setQuestion67(int value) { setQuestionValue(67, value); }
            void setQuestion68(int value) { setQuestionValue(68, value); }
            void setQuestion69(int value) { setQuestionValue(69, value); }
            void setQuestion70(int value) { setQuestionValue(70, value); }
            void setQuestion71(int value) { setQuestionValue(71, value); }
            void setQuestion72(int value) { setQuestionValue(72, value); }
            void setQuestion73(int value) { setQuestionValue(73, value); }
            void setQuestion74(int value) { setQuestionValue(74, value); }
            void setQuestion75(int value) { setQuestionValue(75, value); }
            void setQuestion76(int value) { setQuestionValue(76, value); }
            void setQuestion77(int value) { setQuestionValue(77, value); }
            void setQuestion78(int value) { setQuestionValue(78, value); }
            void setQuestion79(int value) { setQuestionValue(79, value); }
            void setQuestion80(int value) { setQuestionValue(80, value); }
            void setQuestion81(int value) { setQuestionValue(81, value); }
            void setQuestion82(int value) { setQuestionValue(82, value); }
            void setQuestion83(int value) { setQuestionValue(83, value); }
            void setQuestion84(int value) { setQuestionValue(84, value); }
            void setQuestion85(int value) { setQuestionValue(85, value); }
            void setQuestion86(int value) { setQuestionValue(86, value); }
            void setQuestion87(int value) { setQuestionValue(87, value); }
            void setQuestion88(int value) { setQuestionValue(88, value); }
            void setQuestion89(int value) { setQuestionValue(89, value); }
            void setQuestion90(int value) { setQuestionValue(90, value); }
            
            // Generic question setter
            void setQuestion(int questionNumber, int value);
            
            // Validation methods
            bool isValidCaseProfileId(int case_profile_id) const;
            bool isValidQuestionValue(int value) const;
            bool isValidQuestionNumber(int questionNumber) const;
            bool isValidData() const;
            
            // Clinical dimension scoring methods
            int getSomatizationScore() const;        // SOM: Items 1,4,12,27,40,42,48,49,52,53,56,58
            int getObsessionCompulsionScore() const; // O-C: Items 3,9,10,28,38,45,46,51,55,65
            int getInterpersonalSensitivityScore() const; // I-S: Items 6,21,34,36,37,41,61,69,73
            int getDepressionScore() const;          // DEP: Items 5,14,15,20,22,26,29,30,31,32,54,71,79
            int getAnxietyScore() const;             // ANX: Items 2,17,23,33,39,57,72,78,80,86
            int getHostilityScore() const;           // HOS: Items 11,24,63,67,74,81
            int getPhobicAnxietyScore() const;       // PHOB: Items 13,25,47,50,70,75,82
            int getParanoidIdeationScore() const;    // PAR: Items 8,18,43,68,76,83
            int getPsychoticismScore() const;        // PSY: Items 7,16,35,62,77,84,85,87,88,90
            
            // Global indices
            int getGlobalSeverityIndex() const;      // GSI: Sum of all responses
            int getPositiveSymptomTotal() const;     // PST: Count of non-zero responses
            double getPositiveSymptomDistressIndex() const; // PSDI: GSI/PST
            
            // Interpretation methods
            string getSeverityLevel() const;
            vector<string> getElevatedDimensions() const;
            bool hasSignificantDistress() const;
            string getInterpretation() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            void displayDimensionScores() const;
            void displayClinicalInterpretation() const;
            string toString() const;
            
            // Static utility methods
            static void resetIdCounter();
            static int getNextId();
            static vector<string> getDimensionNames();
            static vector<int> getDimensionQuestions(const string& dimension);
            static string getQuestionText(int questionNumber);
            
            // Stream operators
            friend ostream& operator<<(ostream& os, const SCL90R& scl);
            friend istream& operator>>(istream& is, SCL90R& scl);
            
        private:
            // Helper method for setting question values with validation
            void setQuestionValue(int questionNumber, int value);
            
            // Helper method to get question reference by number
            int& getQuestionRef(int questionNumber);
            const int& getQuestionRef(int questionNumber) const;
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // SCL90R_H
