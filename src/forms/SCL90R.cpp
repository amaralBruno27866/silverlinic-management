#include "forms/SCL90R.h"
#include "core/CaseProfile.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static member initialization
        const string SCL90R::FORM_TYPE = "SCL90R";
        int SCL90R::scl_id_counter = 1; // Sequential IDs starting from 1

        // Question texts for reference
        static const map<int, string> QUESTION_TEXTS = {
            {1, "Headaches"},
            {2, "Nervousness or shakiness inside"},
            {3, "Repeated unpleasant thoughts that won't leave your mind"},
            {4, "Faintness or dizziness"},
            {5, "Loss of sexual interest or pleasure"},
            {6, "Feeling critical of others"},
            {7, "The idea that someone else can control your thoughts"},
            {8, "Feeling others are to blame for most of your troubles"},
            {9, "Trouble remembering things"},
            {10, "Worried about sloppiness or carelessness"},
            {11, "Feeling easily annoyed or irritated"},
            {12, "Pains in heart or chest"},
            {13, "Feeling afraid in open places or in the street"},
            {14, "Feeling low in energy or slowed down"},
            {15, "Thoughts of ending your life"},
            {16, "Hearing voices that other people do not hear"},
            {17, "Trembling"},
            {18, "Feeling that most people cannot be trusted"},
            {19, "Poor appetite"},
            {20, "Crying easily"},
            {21, "Feeling shy or uneasy with opposite sex"},
            {22, "Feelings of being trapped or caught"},
            {23, "Suddenly scared for no reason"},
            {24, "Temper outbursts that you could not control"},
            {25, "Feeling afraid to go out of your house alone"},
            {26, "Blaming yourself for things"},
            {27, "Pains in lower back"},
            {28, "Feeling blocked in getting things done"},
            {29, "Feeling lonely"},
            {30, "Feeling blue"},
            {31, "Worrying too much about things"},
            {32, "Feeling no interest in things"},
            {33, "Feeling fearful"},
            {34, "Your feelings being easily hurt"},
            {35, "Other people being aware of your private thoughts"},
            {36, "Feelings others do not understand you or are unsympathetic"},
            {37, "Feeling that people are unfriendly or dislike you"},
            {38, "Having to do things very slowly to ensure correctness"},
            {39, "Heart pounding or racing"},
            {40, "Nausea or upset stomach"},
            {41, "Feeling inferior to others"},
            {42, "Soreness of your muscles"},
            {43, "Feeling that you are watched or talked about by others"},
            {44, "Trouble falling asleep"},
            {45, "Having to check and double-check what you do"},
            {46, "Difficulty making decisions"},
            {47, "Feeling afraid to travel on buses, subways or trains"},
            {48, "Trouble getting your breath"},
            {49, "Hot or cold spells"},
            {50, "Having to avoid certain things, places, or activities because they frighten you"},
            {51, "Your mind going blank"},
            {52, "Numbness or tingling in parts of your body"},
            {53, "A lump in your throat"},
            {54, "Feeling hopeless about the future"},
            {55, "Trouble concentrating"},
            {56, "Feeling weak in parts of your body"},
            {57, "Feeling tensed or keyed up"},
            {58, "Heavy feelings in your arms and legs"},
            {59, "Thoughts of death and dying"},
            {60, "Overeating"},
            {61, "Feeling uneasy when people are watching or talking about you"},
            {62, "Having thoughts that are not your own"},
            {63, "Having urges to beat, injure, or harm someone"},
            {64, "Awakening in early morning"},
            {65, "Having to repeat the same actions such as touching, counting, or washing"},
            {66, "Sleep that is restless or disturbed"},
            {67, "Having urges to break or smash things"},
            {68, "Having beliefs or ideas that others do not share"},
            {69, "Feeling very self-conscious with others"},
            {70, "Feeling uneasy in crowds, such as shopping or at a movie"},
            {71, "Feeling everything is an effort"},
            {72, "Spells of terror or panic"},
            {73, "Feeling uncomfortable about eating or drinking in public"},
            {74, "Getting into frequent arguments"},
            {75, "Feeling nervous when you are left alone"},
            {76, "Others not giving you proper credit for your achievements"},
            {77, "Feeling lonely even when you are with people"},
            {78, "Feeling so restless that you couldn't sit still"},
            {79, "Feelings of worthlessness"},
            {80, "The feeling that something bad is going to happen to you"},
            {81, "Shouting or throwing things"},
            {82, "Feeling afraid that you will faint in public"},
            {83, "Feeling that other people will take advantage of you if you let them"},
            {84, "Having thoughts about sex that bother you a lot"},
            {85, "The idea that you should be punished for your sins"},
            {86, "Thoughts and images of a frightening nature"},
            {87, "The idea that something serious is wrong with your body"},
            {88, "Never feeling close to another person"},
            {89, "Feelings of guilt"},
            {90, "The idea that something is wrong with your mind"}
        };

        // Clinical dimension question mappings
        static const map<string, vector<int>> DIMENSION_QUESTIONS = {
            {"Somatization", {1, 4, 12, 27, 40, 42, 48, 49, 52, 53, 56, 58}},
            {"Obsession-Compulsion", {3, 9, 10, 28, 38, 45, 46, 51, 55, 65}},
            {"Interpersonal Sensitivity", {6, 21, 34, 36, 37, 41, 61, 69, 73}},
            {"Depression", {5, 14, 15, 20, 22, 26, 29, 30, 31, 32, 54, 71, 79}},
            {"Anxiety", {2, 17, 23, 33, 39, 57, 72, 78, 80, 86}},
            {"Hostility", {11, 24, 63, 67, 74, 81}},
            {"Phobic Anxiety", {13, 25, 47, 50, 70, 75, 82}},
            {"Paranoid Ideation", {8, 18, 43, 68, 76, 83}},
            {"Psychoticism", {7, 16, 35, 62, 77, 84, 85, 87, 88, 90}}
        };

        // Constructors
        SCL90R::SCL90R() 
            : m_scl_id(getNextSCLId()), m_case_profile_id(0), m_type(FORM_TYPE),
              m_question_1(0), m_question_2(0), m_question_3(0), m_question_4(0), m_question_5(0),
              m_question_6(0), m_question_7(0), m_question_8(0), m_question_9(0), m_question_10(0),
              m_question_11(0), m_question_12(0), m_question_13(0), m_question_14(0), m_question_15(0),
              m_question_16(0), m_question_17(0), m_question_18(0), m_question_19(0), m_question_20(0),
              m_question_21(0), m_question_22(0), m_question_23(0), m_question_24(0), m_question_25(0),
              m_question_26(0), m_question_27(0), m_question_28(0), m_question_29(0), m_question_30(0),
              m_question_31(0), m_question_32(0), m_question_33(0), m_question_34(0), m_question_35(0),
              m_question_36(0), m_question_37(0), m_question_38(0), m_question_39(0), m_question_40(0),
              m_question_41(0), m_question_42(0), m_question_43(0), m_question_44(0), m_question_45(0),
              m_question_46(0), m_question_47(0), m_question_48(0), m_question_49(0), m_question_50(0),
              m_question_51(0), m_question_52(0), m_question_53(0), m_question_54(0), m_question_55(0),
              m_question_56(0), m_question_57(0), m_question_58(0), m_question_59(0), m_question_60(0),
              m_question_61(0), m_question_62(0), m_question_63(0), m_question_64(0), m_question_65(0),
              m_question_66(0), m_question_67(0), m_question_68(0), m_question_69(0), m_question_70(0),
              m_question_71(0), m_question_72(0), m_question_73(0), m_question_74(0), m_question_75(0),
              m_question_76(0), m_question_77(0), m_question_78(0), m_question_79(0), m_question_80(0),
              m_question_81(0), m_question_82(0), m_question_83(0), m_question_84(0), m_question_85(0),
              m_question_86(0), m_question_87(0), m_question_88(0), m_question_89(0), m_question_90(0) {
            setTimestamps();
            utils::logStructured(utils::LogLevel::INFO, {"FORM","create","SCL90R", to_string(m_scl_id), {}}, "Created");
        }

        SCL90R::SCL90R(int case_profile_id) 
            : m_scl_id(getNextSCLId()), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(0), m_question_2(0), m_question_3(0), m_question_4(0), m_question_5(0),
              m_question_6(0), m_question_7(0), m_question_8(0), m_question_9(0), m_question_10(0),
              m_question_11(0), m_question_12(0), m_question_13(0), m_question_14(0), m_question_15(0),
              m_question_16(0), m_question_17(0), m_question_18(0), m_question_19(0), m_question_20(0),
              m_question_21(0), m_question_22(0), m_question_23(0), m_question_24(0), m_question_25(0),
              m_question_26(0), m_question_27(0), m_question_28(0), m_question_29(0), m_question_30(0),
              m_question_31(0), m_question_32(0), m_question_33(0), m_question_34(0), m_question_35(0),
              m_question_36(0), m_question_37(0), m_question_38(0), m_question_39(0), m_question_40(0),
              m_question_41(0), m_question_42(0), m_question_43(0), m_question_44(0), m_question_45(0),
              m_question_46(0), m_question_47(0), m_question_48(0), m_question_49(0), m_question_50(0),
              m_question_51(0), m_question_52(0), m_question_53(0), m_question_54(0), m_question_55(0),
              m_question_56(0), m_question_57(0), m_question_58(0), m_question_59(0), m_question_60(0),
              m_question_61(0), m_question_62(0), m_question_63(0), m_question_64(0), m_question_65(0),
              m_question_66(0), m_question_67(0), m_question_68(0), m_question_69(0), m_question_70(0),
              m_question_71(0), m_question_72(0), m_question_73(0), m_question_74(0), m_question_75(0),
              m_question_76(0), m_question_77(0), m_question_78(0), m_question_79(0), m_question_80(0),
              m_question_81(0), m_question_82(0), m_question_83(0), m_question_84(0), m_question_85(0),
              m_question_86(0), m_question_87(0), m_question_88(0), m_question_89(0), m_question_90(0) {
            if (!isValidCaseProfileId(case_profile_id)) {
                throw invalid_argument("Invalid case profile ID provided");
            }
            setTimestamps();
            utils::logStructured(utils::LogLevel::INFO, {"FORM","create","SCL90R", to_string(m_scl_id), {}}, "Created for case: "+ to_string(case_profile_id));
        }

        SCL90R::SCL90R(int scl_id, int case_profile_id, 
                       int q1, int q2, int q3, int q4, int q5, int q6, int q7, int q8, int q9, int q10,
                       int q11, int q12, int q13, int q14, int q15, int q16, int q17, int q18, int q19, int q20,
                       int q21, int q22, int q23, int q24, int q25, int q26, int q27, int q28, int q29, int q30,
                       int q31, int q32, int q33, int q34, int q35, int q36, int q37, int q38, int q39, int q40,
                       int q41, int q42, int q43, int q44, int q45, int q46, int q47, int q48, int q49, int q50,
                       int q51, int q52, int q53, int q54, int q55, int q56, int q57, int q58, int q59, int q60,
                       int q61, int q62, int q63, int q64, int q65, int q66, int q67, int q68, int q69, int q70,
                       int q71, int q72, int q73, int q74, int q75, int q76, int q77, int q78, int q79, int q80,
                       int q81, int q82, int q83, int q84, int q85, int q86, int q87, int q88, int q89, int q90,
                       const DateTime& createdAt, const DateTime& updatedAt)
            : m_scl_id(scl_id), m_case_profile_id(case_profile_id), m_type(FORM_TYPE),
              m_question_1(q1), m_question_2(q2), m_question_3(q3), m_question_4(q4), m_question_5(q5),
              m_question_6(q6), m_question_7(q7), m_question_8(q8), m_question_9(q9), m_question_10(q10),
              m_question_11(q11), m_question_12(q12), m_question_13(q13), m_question_14(q14), m_question_15(q15),
              m_question_16(q16), m_question_17(q17), m_question_18(q18), m_question_19(q19), m_question_20(q20),
              m_question_21(q21), m_question_22(q22), m_question_23(q23), m_question_24(q24), m_question_25(q25),
              m_question_26(q26), m_question_27(q27), m_question_28(q28), m_question_29(q29), m_question_30(q30),
              m_question_31(q31), m_question_32(q32), m_question_33(q33), m_question_34(q34), m_question_35(q35),
              m_question_36(q36), m_question_37(q37), m_question_38(q38), m_question_39(q39), m_question_40(q40),
              m_question_41(q41), m_question_42(q42), m_question_43(q43), m_question_44(q44), m_question_45(q45),
              m_question_46(q46), m_question_47(q47), m_question_48(q48), m_question_49(q49), m_question_50(q50),
              m_question_51(q51), m_question_52(q52), m_question_53(q53), m_question_54(q54), m_question_55(q55),
              m_question_56(q56), m_question_57(q57), m_question_58(q58), m_question_59(q59), m_question_60(q60),
              m_question_61(q61), m_question_62(q62), m_question_63(q63), m_question_64(q64), m_question_65(q65),
              m_question_66(q66), m_question_67(q67), m_question_68(q68), m_question_69(q69), m_question_70(q70),
              m_question_71(q71), m_question_72(q72), m_question_73(q73), m_question_74(q74), m_question_75(q75),
              m_question_76(q76), m_question_77(q77), m_question_78(q78), m_question_79(q79), m_question_80(q80),
              m_question_81(q81), m_question_82(q82), m_question_83(q83), m_question_84(q84), m_question_85(q85),
              m_question_86(q86), m_question_87(q87), m_question_88(q88), m_question_89(q89), m_question_90(q90),
              m_scl_createdAt(createdAt), m_scl_updatedAt(updatedAt) {
            
            if (!isValidCaseProfileId(case_profile_id)) {
                throw invalid_argument("Invalid case profile ID provided");
            }
            if (!isValidData()) {
                throw invalid_argument("Invalid question values provided");
            }
            
            utils::logStructured(utils::LogLevel::INFO, {"FORM","load","SCL90R", to_string(m_scl_id), {}}, "Loaded from database");
        }

        // Private helper methods
        void SCL90R::setTimestamps() {
            m_scl_createdAt = DateTime();
            m_scl_updatedAt = DateTime();
        }

        void SCL90R::updateTimestamp() {
            m_scl_updatedAt = DateTime();
        }

    // NOTE: Persistence layer (DAO) should write computed fields (gsi, pst, psdi, severity_level)
    // after question updates. This class keeps computation on-the-fly to avoid stale cache.

        int SCL90R::getNextSCLId() {
            return ++scl_id_counter;
        }

        // Generic question getter
        int SCL90R::getQuestion(int questionNumber) const {
            return getQuestionRef(questionNumber);
        }

        // Generic question setter
        void SCL90R::setQuestion(int questionNumber, int value) {
            setQuestionValue(questionNumber, value);
        }

        // Helper method for setting question values with validation
        void SCL90R::setQuestionValue(int questionNumber, int value) {
            if (!isValidQuestionNumber(questionNumber)) {
                throw invalid_argument("Invalid question number: " + to_string(questionNumber));
            }
            if (!isValidQuestionValue(value)) {
                throw invalid_argument("Invalid question value: " + to_string(value) + " (must be 0-3)");
            }
            
            getQuestionRef(questionNumber) = value;
            updateTimestamp();
        }

        // Helper method to get question reference by number
        int& SCL90R::getQuestionRef(int questionNumber) {
            switch (questionNumber) {
                case 1: return m_question_1; case 2: return m_question_2; case 3: return m_question_3;
                case 4: return m_question_4; case 5: return m_question_5; case 6: return m_question_6;
                case 7: return m_question_7; case 8: return m_question_8; case 9: return m_question_9;
                case 10: return m_question_10; case 11: return m_question_11; case 12: return m_question_12;
                case 13: return m_question_13; case 14: return m_question_14; case 15: return m_question_15;
                case 16: return m_question_16; case 17: return m_question_17; case 18: return m_question_18;
                case 19: return m_question_19; case 20: return m_question_20; case 21: return m_question_21;
                case 22: return m_question_22; case 23: return m_question_23; case 24: return m_question_24;
                case 25: return m_question_25; case 26: return m_question_26; case 27: return m_question_27;
                case 28: return m_question_28; case 29: return m_question_29; case 30: return m_question_30;
                case 31: return m_question_31; case 32: return m_question_32; case 33: return m_question_33;
                case 34: return m_question_34; case 35: return m_question_35; case 36: return m_question_36;
                case 37: return m_question_37; case 38: return m_question_38; case 39: return m_question_39;
                case 40: return m_question_40; case 41: return m_question_41; case 42: return m_question_42;
                case 43: return m_question_43; case 44: return m_question_44; case 45: return m_question_45;
                case 46: return m_question_46; case 47: return m_question_47; case 48: return m_question_48;
                case 49: return m_question_49; case 50: return m_question_50; case 51: return m_question_51;
                case 52: return m_question_52; case 53: return m_question_53; case 54: return m_question_54;
                case 55: return m_question_55; case 56: return m_question_56; case 57: return m_question_57;
                case 58: return m_question_58; case 59: return m_question_59; case 60: return m_question_60;
                case 61: return m_question_61; case 62: return m_question_62; case 63: return m_question_63;
                case 64: return m_question_64; case 65: return m_question_65; case 66: return m_question_66;
                case 67: return m_question_67; case 68: return m_question_68; case 69: return m_question_69;
                case 70: return m_question_70; case 71: return m_question_71; case 72: return m_question_72;
                case 73: return m_question_73; case 74: return m_question_74; case 75: return m_question_75;
                case 76: return m_question_76; case 77: return m_question_77; case 78: return m_question_78;
                case 79: return m_question_79; case 80: return m_question_80; case 81: return m_question_81;
                case 82: return m_question_82; case 83: return m_question_83; case 84: return m_question_84;
                case 85: return m_question_85; case 86: return m_question_86; case 87: return m_question_87;
                case 88: return m_question_88; case 89: return m_question_89; case 90: return m_question_90;
                default: throw invalid_argument("Invalid question number: " + to_string(questionNumber));
            }
        }

        const int& SCL90R::getQuestionRef(int questionNumber) const {
            return const_cast<SCL90R*>(this)->getQuestionRef(questionNumber);
        }

        // Validation methods
        bool SCL90R::isValidCaseProfileId(int case_profile_id) const {
            return case_profile_id > 0; // Sequential IDs start from 1
        }

        bool SCL90R::isValidQuestionValue(int value) const {
            return value >= 0 && value <= 3;
        }

        bool SCL90R::isValidQuestionNumber(int questionNumber) const {
            return questionNumber >= 1 && questionNumber <= 90;
        }

        bool SCL90R::isValidData() const {
            if (!isValidCaseProfileId(m_case_profile_id)) return false;
            
            for (int i = 1; i <= 90; ++i) {
                if (!isValidQuestionValue(getQuestion(i))) return false;
            }
            
            return true;
        }

        // Clinical dimension scoring methods
        int SCL90R::getSomatizationScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Somatization")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getObsessionCompulsionScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Obsession-Compulsion")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getInterpersonalSensitivityScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Interpersonal Sensitivity")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getDepressionScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Depression")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getAnxietyScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Anxiety")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getHostilityScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Hostility")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getPhobicAnxietyScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Phobic Anxiety")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getParanoidIdeationScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Paranoid Ideation")) {
                total += getQuestion(q);
            }
            return total;
        }

        int SCL90R::getPsychoticismScore() const {
            int total = 0;
            for (int q : DIMENSION_QUESTIONS.at("Psychoticism")) {
                total += getQuestion(q);
            }
            return total;
        }

        // Global indices
        int SCL90R::getGlobalSeverityIndex() const {
            int total = 0;
            for (int i = 1; i <= 90; ++i) {
                total += getQuestion(i);
            }
            return total;
        }

        int SCL90R::getPositiveSymptomTotal() const {
            int count = 0;
            for (int i = 1; i <= 90; ++i) {
                if (getQuestion(i) > 0) count++;
            }
            return count;
        }

        double SCL90R::getPositiveSymptomDistressIndex() const {
            int pst = getPositiveSymptomTotal();
            if (pst == 0) return 0.0;
            return static_cast<double>(getGlobalSeverityIndex()) / pst;
        }

        // Interpretation methods
        string SCL90R::getSeverityLevel() const {
            int gsi = getGlobalSeverityIndex();
            
            if (gsi <= 45) {
                return "Minimal";
            } else if (gsi <= 90) {
                return "Mild";
            } else if (gsi <= 135) {
                return "Moderate";
            } else if (gsi <= 180) {
                return "Severe";
            } else {
                return "Very Severe";
            }
        }

        vector<string> SCL90R::getElevatedDimensions() const {
            vector<string> elevated;
            
            // Using clinical cutoff scores (approximate)
            if (getSomatizationScore() >= 12) elevated.push_back("Somatization");
            if (getObsessionCompulsionScore() >= 10) elevated.push_back("Obsession-Compulsion");
            if (getInterpersonalSensitivityScore() >= 9) elevated.push_back("Interpersonal Sensitivity");
            if (getDepressionScore() >= 13) elevated.push_back("Depression");
            if (getAnxietyScore() >= 10) elevated.push_back("Anxiety");
            if (getHostilityScore() >= 6) elevated.push_back("Hostility");
            if (getPhobicAnxietyScore() >= 7) elevated.push_back("Phobic Anxiety");
            if (getParanoidIdeationScore() >= 6) elevated.push_back("Paranoid Ideation");
            if (getPsychoticismScore() >= 10) elevated.push_back("Psychoticism");
            
            return elevated;
        }

        bool SCL90R::hasSignificantDistress() const {
            return getGlobalSeverityIndex() >= 90; // Moderate or higher
        }

        string SCL90R::getInterpretation() const {
            stringstream interpretation;
            interpretation << "SCL-90-R Clinical Interpretation:\n";
            interpretation << "Overall Severity: " << getSeverityLevel() << "\n";
            interpretation << "Global Severity Index: " << getGlobalSeverityIndex() << "\n";
            interpretation << "Positive Symptom Total: " << getPositiveSymptomTotal() << "/90\n";
            interpretation << "Positive Symptom Distress Index: " << fixed << setprecision(2) 
                          << getPositiveSymptomDistressIndex() << "\n";
            
            vector<string> elevated = getElevatedDimensions();
            if (!elevated.empty()) {
                interpretation << "Elevated Dimensions: ";
                for (size_t i = 0; i < elevated.size(); ++i) {
                    interpretation << elevated[i];
                    if (i < elevated.size() - 1) interpretation << ", ";
                }
                interpretation << "\n";
            }
            
            return interpretation.str();
        }

        // Display methods
        void SCL90R::displayInfo() const {
            cout << "\n=== SCL-90-R Information ===" << endl;
            cout << "SCL ID: " << m_scl_id << endl;
            cout << "Case Profile ID: " << m_case_profile_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Created: " << m_scl_createdAt.toString() << endl;
            cout << "Updated: " << m_scl_updatedAt.toString() << endl;
            cout << "Global Severity Index: " << getGlobalSeverityIndex() << endl;
            cout << "Severity Level: " << getSeverityLevel() << endl;
            cout << "Positive Symptoms: " << getPositiveSymptomTotal() << "/90" << endl;
        }

        void SCL90R::displaySummary() const {
            cout << "\n=== SCL-90-R Summary ===" << endl;
            cout << "SCL ID: " << m_scl_id << " | Case: " << m_case_profile_id << endl;
            cout << "Severity: " << getSeverityLevel() << " | GSI: " << getGlobalSeverityIndex() << endl;
            cout << "Positive Symptoms: " << getPositiveSymptomTotal() << "/90" << endl;
            
            vector<string> elevated = getElevatedDimensions();
            if (!elevated.empty()) {
                cout << "Elevated Dimensions: ";
                for (size_t i = 0; i < min(elevated.size(), size_t(3)); ++i) {
                    cout << elevated[i];
                    if (i < min(elevated.size(), size_t(3)) - 1) cout << ", ";
                }
                if (elevated.size() > 3) cout << " (+" << (elevated.size() - 3) << " more)";
                cout << endl;
            }
        }

        void SCL90R::displayDimensionScores() const {
            cout << "\n=== SCL-90-R Dimension Scores ===" << endl;
            cout << "Somatization: " << getSomatizationScore() << "/36" << endl;
            cout << "Obsession-Compulsion: " << getObsessionCompulsionScore() << "/30" << endl;
            cout << "Interpersonal Sensitivity: " << getInterpersonalSensitivityScore() << "/27" << endl;
            cout << "Depression: " << getDepressionScore() << "/39" << endl;
            cout << "Anxiety: " << getAnxietyScore() << "/30" << endl;
            cout << "Hostility: " << getHostilityScore() << "/18" << endl;
            cout << "Phobic Anxiety: " << getPhobicAnxietyScore() << "/21" << endl;
            cout << "Paranoid Ideation: " << getParanoidIdeationScore() << "/18" << endl;
            cout << "Psychoticism: " << getPsychoticismScore() << "/30" << endl;
        }

        void SCL90R::displayClinicalInterpretation() const {
            cout << getInterpretation() << endl;
        }

        string SCL90R::toString() const {
            stringstream ss;
            ss << "SCL90R[ID=" << m_scl_id 
               << ", CaseProfile=" << m_case_profile_id
               << ", Type=" << m_type
               << ", GSI=" << getGlobalSeverityIndex()
               << ", Severity=" << getSeverityLevel()
               << ", PST=" << getPositiveSymptomTotal()
               << ", Created=" << m_scl_createdAt.toString()
               << "]";
            return ss.str();
        }

        // Static utility methods
        void SCL90R::resetIdCounter() {
            scl_id_counter = 1; // Sequential IDs starting from 1
        }

        int SCL90R::getNextId() {
            return scl_id_counter + 1;
        }

        vector<string> SCL90R::getDimensionNames() {
            vector<string> names;
            for (const auto& pair : DIMENSION_QUESTIONS) {
                names.push_back(pair.first);
            }
            return names;
        }

        vector<int> SCL90R::getDimensionQuestions(const string& dimension) {
            auto it = DIMENSION_QUESTIONS.find(dimension);
            if (it != DIMENSION_QUESTIONS.end()) {
                return it->second;
            }
            return vector<int>();
        }

        string SCL90R::getQuestionText(int questionNumber) {
            auto it = QUESTION_TEXTS.find(questionNumber);
            if (it != QUESTION_TEXTS.end()) {
                return it->second;
            }
            return "Unknown question";
        }

        // Stream operators
        ostream& operator<<(ostream& os, const SCL90R& scl) {
            os << scl.toString();
            return os;
        }

        istream& operator>>(istream& is, SCL90R& scl) {
            // Basic implementation for reading from stream
            string line;
            getline(is, line);
            // Parse line and populate scl object
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
