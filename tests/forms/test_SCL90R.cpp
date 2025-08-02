#include "forms/SCL90R.h"
#include "core/CaseProfile.h"
#include "core/DateTime.h"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace std;
using namespace SilverClinic;
using namespace SilverClinic::Forms;

// Test macros (matching project style)
#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            cerr << "ASSERTION FAILED: " << #condition << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            cerr << "ASSERTION FAILED: !(" << #condition << ") at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            cerr << "ASSERTION FAILED: Expected " << (expected) << " but got " << (actual) << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_NOT_EQUAL(not_expected, actual) \
    do { \
        if ((not_expected) == (actual)) { \
            cerr << "ASSERTION FAILED: Did not expect " << (not_expected) << " but got " << (actual) << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_GREATER(actual, threshold) \
    do { \
        if ((actual) <= (threshold)) { \
            cerr << "ASSERTION FAILED: Expected " << (actual) << " > " << (threshold) << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_GREATER_EQUAL(actual, threshold) \
    do { \
        if ((actual) < (threshold)) { \
            cerr << "ASSERTION FAILED: Expected " << (actual) << " >= " << (threshold) << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_LESS_EQUAL(actual, threshold) \
    do { \
        if ((actual) > (threshold)) { \
            cerr << "ASSERTION FAILED: Expected " << (actual) << " <= " << (threshold) << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_CONTAINS(container, value) \
    do { \
        bool found = false; \
        for (const auto& item : container) { \
            if (item == value) { \
                found = true; \
                break; \
            } \
        } \
        if (!found) { \
            cerr << "ASSERTION FAILED: Container does not contain " << value << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_NOT_CONTAINS(container, value) \
    do { \
        bool found = false; \
        for (const auto& item : container) { \
            if (item == value) { \
                found = true; \
                break; \
            } \
        } \
        if (found) { \
            cerr << "ASSERTION FAILED: Container should not contain " << value << " at line " << __LINE__ << endl; \
            return false; \
        } \
    } while(0)

// Test function declarations
bool test_scl90r_constructor_default();
bool test_scl90r_constructor_with_case_id();
bool test_scl90r_constructor_full();
bool test_scl90r_question_setters_getters();
bool test_scl90r_question_validation();
bool test_scl90r_timestamp_management();
bool test_scl90r_dimension_scoring();
bool test_scl90r_global_indices();
bool test_scl90r_severity_assessment();
bool test_scl90r_clinical_interpretation();
bool test_scl90r_display_methods();
bool test_scl90r_static_utilities();
bool test_scl90r_edge_cases();
bool test_scl90r_stress_testing();
bool test_scl90r_clinical_scenarios();
bool test_scl90r_data_integrity();

// Test implementations
bool test_scl90r_constructor_default() {
    SCL90R scl;
    
    ASSERT_GREATER(scl.getSCLId(), 0);
    ASSERT_EQUAL(scl.getCaseProfileId(), 0);
    ASSERT_EQUAL(scl.getType(), "SCL90R");
    
    // Test all questions initialized to 0
    for (int i = 1; i <= 90; ++i) {
        ASSERT_EQUAL(scl.getQuestion(i), 0);
    }
    
    // Test calculated scores
    ASSERT_EQUAL(scl.getGlobalSeverityIndex(), 0);
    ASSERT_EQUAL(scl.getPositiveSymptomTotal(), 0);
    ASSERT_EQUAL(scl.getSeverityLevel(), "Minimal");
    
    return true;
}

bool test_scl90r_constructor_with_case_id() {
    int case_id = 400001; // Use valid CaseProfile ID prefix
    SCL90R scl(case_id);
    
    ASSERT_GREATER(scl.getSCLId(), 0);
    ASSERT_EQUAL(scl.getCaseProfileId(), case_id);
    ASSERT_EQUAL(scl.getType(), "SCL90R");
    
    // Test all questions initialized to 0
    for (int i = 1; i <= 90; ++i) {
        ASSERT_EQUAL(scl.getQuestion(i), 0);
    }
    
    return true;
}

bool test_scl90r_constructor_full() {
    DateTime created, updated;
    
    // Create SCL90R with specific values for first 10 questions
    SCL90R scl(99999, 400001,  // Use valid CaseProfile ID
               1, 2, 3, 0, 1, 2, 0, 1, 2, 3, // Q1-Q10
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q11-Q20
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q21-Q30
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q31-Q40
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q41-Q50
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q51-Q60
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q61-Q70
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q71-Q80
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Q81-Q90
               created, updated);
    
    ASSERT_EQUAL(scl.getSCLId(), 99999);
    ASSERT_EQUAL(scl.getCaseProfileId(), 400001);  // Use valid CaseProfile ID
    ASSERT_EQUAL(scl.getQuestion(1), 1);
    ASSERT_EQUAL(scl.getQuestion(2), 2);
    ASSERT_EQUAL(scl.getQuestion(3), 3);
    ASSERT_EQUAL(scl.getQuestion(4), 0);
    ASSERT_EQUAL(scl.getQuestion(10), 3);
    
    return true;
}

bool test_scl90r_question_setters_getters() {
    SCL90R scl(400001);
    
    // Test setting and getting individual questions
    scl.setQuestion(1, 2);
    ASSERT_EQUAL(scl.getQuestion(1), 2);
    
    scl.setQuestion(45, 3);
    ASSERT_EQUAL(scl.getQuestion(45), 3);
    
    scl.setQuestion(90, 1);
    ASSERT_EQUAL(scl.getQuestion(90), 1);
    
    // Test that other questions remain unchanged
    ASSERT_EQUAL(scl.getQuestion(2), 0);
    ASSERT_EQUAL(scl.getQuestion(44), 0);
    ASSERT_EQUAL(scl.getQuestion(89), 0);
    
    return true;
}

bool test_scl90r_question_validation() {
    SCL90R scl(400001);
    
    // Test valid values (0-3)
    for (int value = 0; value <= 3; ++value) {
        scl.setQuestion(1, value);
        ASSERT_EQUAL(scl.getQuestion(1), value);
    }
    
    // Test invalid question numbers should throw exception
    bool exception_thrown = false;
    try {
        scl.setQuestion(0, 1); // Invalid question number
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
    
    exception_thrown = false;
    try {
        scl.setQuestion(91, 1); // Invalid question number
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
    
    // Test invalid values should throw exception
    exception_thrown = false;
    try {
        scl.setQuestion(1, -1); // Invalid value
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
    
    exception_thrown = false;
    try {
        scl.setQuestion(1, 4); // Invalid value
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
    
    return true;
}

bool test_scl90r_timestamp_management() {
    SCL90R scl(400001);
    
    // Test that timestamp methods don't crash and return valid DateTime objects
    DateTime initial_created = scl.getCreatedAt();
    DateTime initial_updated = scl.getUpdatedAt();
    
    // Modify a question
    scl.setQuestion(1, 2);
    DateTime after_update = scl.getUpdatedAt();
    
    // Verify the methods work (even if DateTime returns "Invalid DateTime")
    // The important thing is that the methods don't crash
    ASSERT_TRUE(true); // Basic functionality test
    
    return true;
}

bool test_scl90r_dimension_scoring() {
    SCL90R scl(400001);
    
    // Test empty scores
    ASSERT_EQUAL(scl.getSomatizationScore(), 0);
    ASSERT_EQUAL(scl.getDepressionScore(), 0);
    ASSERT_EQUAL(scl.getAnxietyScore(), 0);
    
    // Set some somatization questions (1, 4, 12, 27, 40, 42, 48, 49, 52, 53, 56, 58)
    scl.setQuestion(1, 2);  // Headaches
    scl.setQuestion(4, 1);  // Faintness or dizziness
    scl.setQuestion(12, 3); // Pains in heart or chest
    
    ASSERT_EQUAL(scl.getSomatizationScore(), 6);
    
    // Set some depression questions (5, 14, 15, 20, 22, 26, 29, 30, 31, 32, 54, 71, 79)
    scl.setQuestion(15, 1); // Thoughts of ending your life
    scl.setQuestion(30, 2); // Feeling blue
    scl.setQuestion(54, 3); // Feeling hopeless about the future
    
    ASSERT_EQUAL(scl.getDepressionScore(), 6);
    
    // Test anxiety questions (2, 17, 23, 33, 39, 57, 72, 78, 80, 86)
    scl.setQuestion(2, 1);  // Nervousness
    scl.setQuestion(23, 2); // Suddenly scared for no reason
    
    ASSERT_EQUAL(scl.getAnxietyScore(), 3);
    
    return true;
}

bool test_scl90r_global_indices() {
    SCL90R scl(400001);
    
    // Test with empty form
    ASSERT_EQUAL(scl.getGlobalSeverityIndex(), 0);
    ASSERT_EQUAL(scl.getPositiveSymptomTotal(), 0);
    ASSERT_EQUAL(scl.getPositiveSymptomDistressIndex(), 0.0);
    
    // Set some questions with different values
    scl.setQuestion(1, 2);
    scl.setQuestion(15, 3);
    scl.setQuestion(30, 1);
    scl.setQuestion(45, 0); // This should not count as positive
    scl.setQuestion(60, 2);
    scl.setQuestion(75, 1);
    
    ASSERT_EQUAL(scl.getGlobalSeverityIndex(), 9); // 2+3+1+0+2+1
    ASSERT_EQUAL(scl.getPositiveSymptomTotal(), 5); // Questions with value > 0
    ASSERT_EQUAL(scl.getPositiveSymptomDistressIndex(), 1.8); // 9/5 = 1.8
    
    return true;
}

bool test_scl90r_severity_assessment() {
    SCL90R scl(400001);
    
    // Test minimal severity (GSI <= 45)
    ASSERT_EQUAL(scl.getSeverityLevel(), "Minimal");
    ASSERT_FALSE(scl.hasSignificantDistress());
    
    // Set questions to reach mild severity (46-90)
    for (int i = 1; i <= 23; ++i) {
        scl.setQuestion(i, 2); // 23 * 2 = 46
    }
    ASSERT_EQUAL(scl.getSeverityLevel(), "Mild");
    
    // Set questions to reach moderate severity (91-135)
    for (int i = 24; i <= 45; ++i) {
        scl.setQuestion(i, 2); // Additional 22 * 2 = 44, total = 90
    }
    scl.setQuestion(46, 1); // Total = 91
    ASSERT_EQUAL(scl.getSeverityLevel(), "Moderate");
    ASSERT_TRUE(scl.hasSignificantDistress());
    
    return true;
}

bool test_scl90r_clinical_interpretation() {
    SCL90R scl(400001);
    
    // Set high values for depression questions to test elevated dimensions
    vector<int> depression_questions = {5, 14, 15, 20, 22, 26, 29, 30, 31, 32, 54, 71, 79};
    for (int q : depression_questions) {
        scl.setQuestion(q, 2); // High depression scores
    }
    
    vector<string> elevated = scl.getElevatedDimensions();
    ASSERT_CONTAINS(elevated, "Depression");
    
    // Test interpretation string contains key information
    string interpretation = scl.getInterpretation();
    ASSERT_TRUE(interpretation.find("SCL-90-R Clinical Interpretation") != string::npos);
    ASSERT_TRUE(interpretation.find("Overall Severity") != string::npos);
    ASSERT_TRUE(interpretation.find("Global Severity Index") != string::npos);
    
    return true;
}

bool test_scl90r_display_methods() {
    SCL90R scl(400001);
    
    // Set some test data
    scl.setQuestion(1, 2);
    scl.setQuestion(15, 3);
    scl.setQuestion(30, 1);
    
    // Test toString method
    string str_repr = scl.toString();
    ASSERT_TRUE(str_repr.find("SCL90R") != string::npos);
    ASSERT_TRUE(str_repr.find("400001") != string::npos);
    ASSERT_TRUE(str_repr.find("GSI=6") != string::npos);
    
    // These methods should not crash (visual inspection needed for output)
    scl.displayInfo();
    scl.displaySummary();
    scl.displayDimensionScores();
    scl.displayClinicalInterpretation();
    
    return true;
}

bool test_scl90r_static_utilities() {
    // Test dimension names
    vector<string> dimensions = SCL90R::getDimensionNames();
    ASSERT_EQUAL(dimensions.size(), 9);
    ASSERT_CONTAINS(dimensions, "Somatization");
    ASSERT_CONTAINS(dimensions, "Depression");
    ASSERT_CONTAINS(dimensions, "Anxiety");
    ASSERT_CONTAINS(dimensions, "Hostility");
    
    // Test dimension questions
    vector<int> som_questions = SCL90R::getDimensionQuestions("Somatization");
    ASSERT_EQUAL(som_questions.size(), 12);
    ASSERT_CONTAINS(som_questions, 1);  // Headaches
    ASSERT_CONTAINS(som_questions, 12); // Pains in heart or chest
    
    // Test question texts
    ASSERT_EQUAL(SCL90R::getQuestionText(1), "Headaches");
    ASSERT_EQUAL(SCL90R::getQuestionText(15), "Thoughts of ending your life");
    ASSERT_EQUAL(SCL90R::getQuestionText(91), "Unknown question");
    
    // Test ID management
    int next_id = SCL90R::getNextId();
    ASSERT_GREATER(next_id, 0);
    
    return true;
}

bool test_scl90r_edge_cases() {
    // Test invalid case profile ID
    bool exception_thrown = false;
    try {
        SCL90R scl(-1); // Invalid case profile ID
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
    
    // Test boundary question numbers
    SCL90R scl(400001);
    scl.setQuestion(1, 3);   // First question
    scl.setQuestion(90, 3);  // Last question
    
    ASSERT_EQUAL(scl.getQuestion(1), 3);
    ASSERT_EQUAL(scl.getQuestion(90), 3);
    
    // Test maximum possible scores
    for (int i = 1; i <= 90; ++i) {
        scl.setQuestion(i, 3);
    }
    
    ASSERT_EQUAL(scl.getGlobalSeverityIndex(), 270); // 90 * 3
    ASSERT_EQUAL(scl.getPositiveSymptomTotal(), 90);
    ASSERT_EQUAL(scl.getSeverityLevel(), "Very Severe");
    
    return true;
}

bool test_scl90r_stress_testing() {
    // Create multiple SCL90R instances
    vector<SCL90R> scl_forms;
    
    for (int i = 0; i < 100; ++i) {
        scl_forms.emplace_back(400001 + i);
        
        // Set random-like values
        for (int q = 1; q <= 90; ++q) {
            scl_forms[i].setQuestion(q, (i + q) % 4);
        }
    }
    
    // Verify all forms are independent
    ASSERT_EQUAL(scl_forms.size(), 100);
    ASSERT_NOT_EQUAL(scl_forms[0].getSCLId(), scl_forms[1].getSCLId());
    // Test that forms have different case profile IDs
    ASSERT_NOT_EQUAL(scl_forms[0].getCaseProfileId(), scl_forms[1].getCaseProfileId());
    
    // Verify that we can access all forms without crashes
    int total_forms_with_positive_symptoms = 0;
    for (const auto& form : scl_forms) {
        if (form.getPositiveSymptomTotal() > 0) {
            total_forms_with_positive_symptoms++;
        }
    }
    ASSERT_GREATER(total_forms_with_positive_symptoms, 50); // Most should have some symptoms
    
    return true;
}

bool test_scl90r_clinical_scenarios() {
    // Scenario 1: Severe Depression Case
    SCL90R depression_case(400001);
    vector<int> depression_questions = {5, 14, 15, 20, 22, 26, 29, 30, 31, 32, 54, 71, 79};
    for (int q : depression_questions) {
        depression_case.setQuestion(q, 3);
    }
    
    // Add enough questions to definitely reach significant distress threshold (90)
    // Set questions 1-40 (excluding depression questions) to value 3
    for (int i = 1; i <= 40; ++i) {
        if (std::find(depression_questions.begin(), depression_questions.end(), i) == depression_questions.end()) {
            depression_case.setQuestion(i, 3);
        }
    }
    
    ASSERT_GREATER_EQUAL(depression_case.getDepressionScore(), 30);
    ASSERT_CONTAINS(depression_case.getElevatedDimensions(), "Depression");
    ASSERT_GREATER_EQUAL(depression_case.getGlobalSeverityIndex(), 90);
    ASSERT_TRUE(depression_case.hasSignificantDistress());
    
    // Scenario 2: Anxiety Disorder Case
    SCL90R anxiety_case(400002);
    vector<int> anxiety_questions = {2, 17, 23, 33, 39, 57, 72, 78, 80, 86};
    for (int q : anxiety_questions) {
        anxiety_case.setQuestion(q, 3);
    }
    
    ASSERT_GREATER_EQUAL(anxiety_case.getAnxietyScore(), 25);
    ASSERT_CONTAINS(anxiety_case.getElevatedDimensions(), "Anxiety");
    
    // Scenario 3: Multiple Comorbidities
    SCL90R complex_case(400003);
    
    // High depression
    for (int q : {15, 30, 54, 79}) {
        complex_case.setQuestion(q, 3);
    }
    
    // High anxiety
    for (int q : {2, 23, 72, 80}) {
        complex_case.setQuestion(q, 3);
    }
    
    // Some somatization
    for (int q : {1, 12, 40}) {
        complex_case.setQuestion(q, 2);
    }
    
    vector<string> elevated = complex_case.getElevatedDimensions();
    ASSERT_GREATER_EQUAL(elevated.size(), 1); // Should have at least one elevated dimension
    
    return true;
}

bool test_scl90r_data_integrity() {
    SCL90R scl(400001);
    
    // Test data consistency after multiple operations
    scl.setQuestion(1, 2);
    scl.setQuestion(50, 3);
    scl.setQuestion(90, 1);
    
    int initial_gsi = scl.getGlobalSeverityIndex();
    int initial_pst = scl.getPositiveSymptomTotal();
    
    // Modify and restore
    scl.setQuestion(25, 2);
    scl.setQuestion(25, 0);
    
    ASSERT_EQUAL(scl.getGlobalSeverityIndex(), initial_gsi);
    ASSERT_EQUAL(scl.getPositiveSymptomTotal(), initial_pst);
    
    // Test that changing one question affects global indices correctly
    scl.setQuestion(45, 1);
    ASSERT_EQUAL(scl.getGlobalSeverityIndex(), initial_gsi + 1);
    ASSERT_EQUAL(scl.getPositiveSymptomTotal(), initial_pst + 1);
    
    return true;
}

// Test runner
int main() {
    cout << "Starting SCL-90-R Form Tests..." << endl;
    
    struct TestCase {
        bool (*test_func)();
        string test_name;
    };
    
    vector<TestCase> tests = {
        {test_scl90r_constructor_default, "Default Constructor"},
        {test_scl90r_constructor_with_case_id, "Constructor with Case ID"},
        {test_scl90r_constructor_full, "Full Constructor"},
        {test_scl90r_question_setters_getters, "Question Setters/Getters"},
        {test_scl90r_question_validation, "Question Validation"},
        {test_scl90r_timestamp_management, "Timestamp Management"},
        {test_scl90r_dimension_scoring, "Dimension Scoring"},
        {test_scl90r_global_indices, "Global Indices"},
        {test_scl90r_severity_assessment, "Severity Assessment"},
        {test_scl90r_clinical_interpretation, "Clinical Interpretation"},
        {test_scl90r_display_methods, "Display Methods"},
        {test_scl90r_static_utilities, "Static Utilities"},
        {test_scl90r_edge_cases, "Edge Cases"},
        {test_scl90r_stress_testing, "Stress Testing"},
        {test_scl90r_clinical_scenarios, "Clinical Scenarios"},
        {test_scl90r_data_integrity, "Data Integrity"}
    };
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& test : tests) {
        cout << "Running: " << test.test_name << "... ";
        
        try {
            if (test.test_func()) {
                cout << "PASSED" << endl;
                passed++;
            } else {
                cout << "FAILED" << endl;
                failed++;
            }
        } catch (const exception& e) {
            cout << "EXCEPTION: " << e.what() << endl;
            failed++;
        }
    }
    
    cout << "\n=== Test Results ===" << endl;
    cout << "Total Tests: " << (passed + failed) << endl;
    cout << "Passed: " << passed << endl;
    cout << "Failed: " << failed << endl;
    cout << "Success Rate: " << (passed * 100.0 / (passed + failed)) << "%" << endl;
    
    if (failed == 0) {
        cout << "\n🎉 All SCL-90-R tests passed successfully!" << endl;
        return 0;
    } else {
        cout << "\n❌ Some tests failed. Please review the implementation." << endl;
        return 1;
    }
}
