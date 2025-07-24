#include "forms/BeckDepressionInventory.h"
#include "core/DateTime.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;
using namespace SilverClinic::Forms;
using namespace SilverClinic;

class BeckDepressionInventoryTest {
private:
    int tests_run = 0;
    int tests_passed = 0;

    void assert_test(bool condition, const string& test_name) {
        tests_run++;
        if (condition) {
            tests_passed++;
            cout << "[PASS] " << test_name << endl;
        } else {
            cout << "[FAIL] " << test_name << endl;
        }
    }

public:
    void run_all_tests() {
        cout << "\n=== Beck Depression Inventory Tests ===" << endl;
        
        test_default_constructor();
        test_constructor_with_case_id();
        test_full_constructor();
        test_question_setters_and_getters();
        test_score_calculation();
        test_severity_levels();
        test_validation();
        test_category_scores();
        test_display_methods();
        test_static_methods();
        test_clinical_interpretation();
        
        cout << "\n=== Test Results ===" << endl;
        cout << "Tests run: " << tests_run << endl;
        cout << "Tests passed: " << tests_passed << endl;
        cout << "Tests failed: " << (tests_run - tests_passed) << endl;
        cout << "Success rate: " << (tests_passed * 100.0 / tests_run) << "%" << endl;
    }

    void test_default_constructor() {
        BeckDepressionInventory bdi;
        
        assert_test(bdi.getBDIId() > 800000, "Default constructor - ID generation");
        assert_test(bdi.getCaseProfileId() == 0, "Default constructor - case profile ID");
        assert_test(bdi.getType() == "BDI", "Default constructor - type");
        assert_test(bdi.getTotalScore() == 0, "Default constructor - initial score");
        assert_test(bdi.getSeverityLevel() == "Minimal", "Default constructor - initial severity");
    }

    void test_constructor_with_case_id() {
        int case_id = 12345;
        BeckDepressionInventory bdi(case_id);
        
        assert_test(bdi.getBDIId() > 800000, "Constructor with case ID - ID generation");
        assert_test(bdi.getCaseProfileId() == case_id, "Constructor with case ID - case profile ID");
        assert_test(bdi.getType() == "BDI", "Constructor with case ID - type");
        assert_test(bdi.getTotalScore() == 0, "Constructor with case ID - initial score");
    }

    void test_full_constructor() {
        DateTime now = DateTime::now();
        BeckDepressionInventory bdi(800001, 12345, 
                                  1, 2, 1, 0, 3, 2, 1, 2, 0, 1,
                                  2, 1, 3, 2, 1, 2, 1, 0, 2, 1, 3,
                                  now, now);
        
        assert_test(bdi.getBDIId() == 800001, "Full constructor - BDI ID");
        assert_test(bdi.getCaseProfileId() == 12345, "Full constructor - case profile ID");
        assert_test(bdi.getQuestion1() == 1, "Full constructor - question 1");
        assert_test(bdi.getQuestion21() == 3, "Full constructor - question 21");
        assert_test(bdi.getTotalScore() == 31, "Full constructor - total score");
    }

    void test_question_setters_and_getters() {
        BeckDepressionInventory bdi;
        
        // Test setting valid values
        bdi.setQuestion1(2);
        bdi.setQuestion5(3);
        bdi.setQuestion10(1);
        bdi.setQuestion21(0);
        
        assert_test(bdi.getQuestion1() == 2, "Question setters/getters - question 1");
        assert_test(bdi.getQuestion5() == 3, "Question setters/getters - question 5");
        assert_test(bdi.getQuestion10() == 1, "Question setters/getters - question 10");
        assert_test(bdi.getQuestion21() == 0, "Question setters/getters - question 21");
        
        // Test that invalid values are rejected (should remain unchanged)
        int original_q1 = bdi.getQuestion1();
        bdi.setQuestion1(-1); // Invalid
        bdi.setQuestion1(4);  // Invalid
        assert_test(bdi.getQuestion1() == original_q1, "Question setters - validation");
    }

    void test_score_calculation() {
        BeckDepressionInventory bdi;
        
        // Set all questions to 1 (21 * 1 = 21)
        for (int i = 1; i <= 21; i++) {
            switch(i) {
                case 1: bdi.setQuestion1(1); break;
                case 2: bdi.setQuestion2(1); break;
                case 3: bdi.setQuestion3(1); break;
                case 4: bdi.setQuestion4(1); break;
                case 5: bdi.setQuestion5(1); break;
                case 6: bdi.setQuestion6(1); break;
                case 7: bdi.setQuestion7(1); break;
                case 8: bdi.setQuestion8(1); break;
                case 9: bdi.setQuestion9(1); break;
                case 10: bdi.setQuestion10(1); break;
                case 11: bdi.setQuestion11(1); break;
                case 12: bdi.setQuestion12(1); break;
                case 13: bdi.setQuestion13(1); break;
                case 14: bdi.setQuestion14(1); break;
                case 15: bdi.setQuestion15(1); break;
                case 16: bdi.setQuestion16(1); break;
                case 17: bdi.setQuestion17(1); break;
                case 18: bdi.setQuestion18(1); break;
                case 19: bdi.setQuestion19(1); break;
                case 20: bdi.setQuestion20(1); break;
                case 21: bdi.setQuestion21(1); break;
            }
        }
        
        assert_test(bdi.getTotalScore() == 21, "Score calculation - total score");
        assert_test(abs(bdi.getDepressionPercentage() - 33.33) < 0.1, "Score calculation - percentage");
    }

    void test_severity_levels() {
        BeckDepressionInventory bdi;
        
        // Test Minimal (0-13)
        assert_test(bdi.hasMinimalDepression(), "Severity levels - minimal depression (0)");
        assert_test(bdi.getSeverityLevel() == "Minimal", "Severity levels - minimal string");
        
        // Test Mild (14-19) - set score to 15
        bdi.setQuestion1(3); bdi.setQuestion2(3); bdi.setQuestion3(3); 
        bdi.setQuestion4(3); bdi.setQuestion5(3); // 15 points
        assert_test(bdi.hasMildDepression(), "Severity levels - mild depression");
        assert_test(bdi.getSeverityLevel() == "Mild", "Severity levels - mild string");
        
        // Reset and test Moderate (20-28) - set score to 25
        BeckDepressionInventory bdi2;
        for (int i = 1; i <= 8; i++) {
            switch(i) {
                case 1: bdi2.setQuestion1(3); break;
                case 2: bdi2.setQuestion2(3); break;
                case 3: bdi2.setQuestion3(3); break;
                case 4: bdi2.setQuestion4(3); break;
                case 5: bdi2.setQuestion5(3); break;
                case 6: bdi2.setQuestion6(3); break;
                case 7: bdi2.setQuestion7(3); break;
                case 8: bdi2.setQuestion8(1); break; // 7*3 + 1 = 22
            }
        }
        assert_test(bdi2.hasModerateDepression(), "Severity levels - moderate depression");
        assert_test(bdi2.getSeverityLevel() == "Moderate", "Severity levels - moderate string");
    }

    void test_validation() {
        BeckDepressionInventory bdi(12345);
        
        assert_test(bdi.isValidCaseProfileId(12345), "Validation - valid case profile ID");
        assert_test(!bdi.isValidCaseProfileId(0), "Validation - invalid case profile ID (0)");
        assert_test(!bdi.isValidCaseProfileId(-1), "Validation - invalid case profile ID (negative)");
        
        assert_test(bdi.isValidQuestionValue(0), "Validation - valid question value (0)");
        assert_test(bdi.isValidQuestionValue(3), "Validation - valid question value (3)");
        assert_test(!bdi.isValidQuestionValue(-1), "Validation - invalid question value (negative)");
        assert_test(!bdi.isValidQuestionValue(4), "Validation - invalid question value (too high)");
        
        assert_test(bdi.isValidData(), "Validation - valid data");
    }

    void test_category_scores() {
        BeckDepressionInventory bdi;
        
        // Set specific questions for each category
        // Cognitive: Q2, Q3, Q7, Q8, Q9, Q13, Q14
        bdi.setQuestion2(1); bdi.setQuestion3(1); bdi.setQuestion7(1);
        bdi.setQuestion8(1); bdi.setQuestion9(1); bdi.setQuestion13(1); bdi.setQuestion14(1);
        
        // Affective: Q1, Q4, Q5, Q6, Q10, Q12, Q17
        bdi.setQuestion1(2); bdi.setQuestion4(2); bdi.setQuestion5(2);
        bdi.setQuestion6(2); bdi.setQuestion10(2); bdi.setQuestion12(2); bdi.setQuestion17(2);
        
        // Somatic: Q11, Q15, Q16, Q18, Q19, Q20, Q21
        bdi.setQuestion11(3); bdi.setQuestion15(3); bdi.setQuestion16(3);
        bdi.setQuestion18(3); bdi.setQuestion19(3); bdi.setQuestion20(3); bdi.setQuestion21(3);
        
        assert_test(bdi.getCognitiveScore() == 7, "Category scores - cognitive score");
        assert_test(bdi.getAffectiveScore() == 14, "Category scores - affective score");
        assert_test(bdi.getSomaticScore() == 21, "Category scores - somatic score");
    }

    void test_display_methods() {
        BeckDepressionInventory bdi(12345);
        bdi.setQuestion1(2);
        bdi.setQuestion5(3);
        
        // Test that display methods don't crash (we can't easily test output)
        cout << "\n--- Testing display methods (output verification) ---" << endl;
        bdi.displaySummary();
        
        string str = bdi.toString();
        assert_test(!str.empty(), "Display methods - toString not empty");
        assert_test(str.find("BDI") != string::npos, "Display methods - toString contains BDI");
    }

    void test_static_methods() {
        // Test interpretation
        assert_test(BeckDepressionInventory::interpretScore(5) == "Minimal", "Static methods - interpret minimal");
        assert_test(BeckDepressionInventory::interpretScore(15) == "Mild", "Static methods - interpret mild");
        assert_test(BeckDepressionInventory::interpretScore(25) == "Moderate", "Static methods - interpret moderate");
        assert_test(BeckDepressionInventory::interpretScore(35) == "Severe", "Static methods - interpret severe");
        
        // Test high risk
        assert_test(!BeckDepressionInventory::isHighRiskScore(15), "Static methods - not high risk (mild)");
        assert_test(BeckDepressionInventory::isHighRiskScore(25), "Static methods - high risk (moderate)");
        assert_test(BeckDepressionInventory::isHighRiskScore(35), "Static methods - high risk (severe)");
        
        // Test ID management
        int next_id = BeckDepressionInventory::getNextId();
        assert_test(next_id > 800000, "Static methods - next ID generation");
    }

    void test_clinical_interpretation() {
        BeckDepressionInventory bdi;
        
        // Test minimal depression
        assert_test(bdi.hasMinimalDepression(), "Clinical interpretation - minimal depression");
        assert_test(!bdi.hasMildDepression(), "Clinical interpretation - not mild depression");
        assert_test(!bdi.hasModerateDepression(), "Clinical interpretation - not moderate depression");
        assert_test(!bdi.hasSevereDepression(), "Clinical interpretation - not severe depression");
        
        // Create a moderate depression case
        BeckDepressionInventory bdi_moderate;
        for (int i = 1; i <= 7; i++) {
            switch(i) {
                case 1: bdi_moderate.setQuestion1(3); break;
                case 2: bdi_moderate.setQuestion2(3); break;
                case 3: bdi_moderate.setQuestion3(3); break;
                case 4: bdi_moderate.setQuestion4(3); break;
                case 5: bdi_moderate.setQuestion5(3); break;
                case 6: bdi_moderate.setQuestion6(3); break;
                case 7: bdi_moderate.setQuestion7(2); break; // 6*3 + 2 = 20 (moderate)
            }
        }
        
        assert_test(!bdi_moderate.hasMinimalDepression(), "Clinical interpretation - not minimal (moderate case)");
        assert_test(!bdi_moderate.hasMildDepression(), "Clinical interpretation - not mild (moderate case)");
        assert_test(bdi_moderate.hasModerateDepression(), "Clinical interpretation - moderate depression");
        assert_test(!bdi_moderate.hasSevereDepression(), "Clinical interpretation - not severe (moderate case)");
    }
};

int main() {
    BeckDepressionInventoryTest test;
    test.run_all_tests();
    return 0;
}
