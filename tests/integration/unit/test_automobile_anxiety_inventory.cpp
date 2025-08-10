#include "forms/AutomobileAnxietyInventory.h"
#include "core/DateTime.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>

using namespace std;
using namespace SilverClinic;
using namespace SilverClinic::Forms;

// Simple test framework macros
#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        cout << "❌ FAIL: " << message << endl; \
        return false; \
    }

#define TEST_ASSERT_EQUAL(expected, actual, message) \
    if ((expected) != (actual)) { \
        cout << "❌ FAIL: " << message << " - Expected: " << (expected) << ", Got: " << (actual) << endl; \
        return false; \
    }

#define TEST_ASSERT_NEAR(expected, actual, tolerance, message) \
    if (abs((expected) - (actual)) > (tolerance)) { \
        cout << "❌ FAIL: " << message << " - Expected: " << (expected) << ", Got: " << (actual) << endl; \
        return false; \
    }

// Test functions
bool testDefaultConstructor() {
    cout << "🧪 Testing Default Constructor..." << endl;
    
    AutomobileAnxietyInventory::resetIdCounter();
    AutomobileAnxietyInventory aai;
    
    TEST_ASSERT(aai.getAAIId() > 700000, "AAI ID should be greater than 700000");
    TEST_ASSERT_EQUAL(0, aai.getCaseProfileId(), "Default case profile ID should be 0");
    TEST_ASSERT_EQUAL(string("AAI"), aai.getType(), "Form type should be AAI");
    
    // Check all questions start as false
    TEST_ASSERT(!aai.getQuestion1(), "Question 1 should start as false");
    TEST_ASSERT(!aai.getQuestion15A(), "Question 15A should start as false");
    TEST_ASSERT(!aai.getQuestion19(), "Question 19 should start as false");
    TEST_ASSERT(!aai.getQuestion23(), "Question 23 should start as false");
    
    // Check initial anxiety score
    TEST_ASSERT_EQUAL(0, aai.getTotalYesResponses(), "Initial yes responses should be 0");
    TEST_ASSERT_EQUAL(0.0, aai.getAnxietyScore(), "Initial anxiety score should be 0.0");
    TEST_ASSERT(!aai.hasHighAnxiety(), "Should not have high anxiety initially");
    
    cout << "✅ Default Constructor test passed!" << endl;
    return true;
}

bool testConstructorWithCaseProfileId() {
    cout << "🧪 Testing Constructor with Case Profile ID..." << endl;
    
    AutomobileAnxietyInventory::resetIdCounter();
    int case_profile_id = 400001;
    AutomobileAnxietyInventory aai(case_profile_id);
    
    TEST_ASSERT(aai.getAAIId() > 700000, "AAI ID should be greater than 700000");
    TEST_ASSERT_EQUAL(case_profile_id, aai.getCaseProfileId(), "Case profile ID should match");
    TEST_ASSERT_EQUAL(string("AAI"), aai.getType(), "Form type should be AAI");
    
    cout << "✅ Constructor with Case Profile ID test passed!" << endl;
    return true;
}

bool testIdGeneration() {
    cout << "🧪 Testing ID Generation..." << endl;
    
    AutomobileAnxietyInventory::resetIdCounter();
    AutomobileAnxietyInventory aai1;
    AutomobileAnxietyInventory aai2;
    
    TEST_ASSERT(aai1.getAAIId() > 700000, "First AAI ID should be greater than 700000");
    TEST_ASSERT(aai2.getAAIId() > 700000, "Second AAI ID should be greater than 700000");
    TEST_ASSERT(aai2.getAAIId() > aai1.getAAIId(), "Second AAI ID should be greater than first");
    
    cout << "✅ ID Generation test passed!" << endl;
    return true;
}

bool testSimpleYesNoQuestions() {
    cout << "🧪 Testing Simple Yes/No Questions..." << endl;
    
    AutomobileAnxietyInventory aai(400001);
    
    // Test setting and getting question 1
    aai.setQuestion1(true);
    TEST_ASSERT(aai.getQuestion1(), "Question 1 should be true after setting");
    
    // Test setting and getting question 9 (traffic nervousness)
    aai.setQuestion9(true);
    TEST_ASSERT(aai.getQuestion9(), "Question 9 should be true after setting");
    
    // Test question 23 (panic attacks)
    aai.setQuestion23(true);
    TEST_ASSERT(aai.getQuestion23(), "Question 23 should be true after setting");
    
    // Check total yes responses
    TEST_ASSERT_EQUAL(3, aai.getTotalYesResponses(), "Should have 3 yes responses");
    
    cout << "✅ Simple Yes/No Questions test passed!" << endl;
    return true;
}

bool testQuestion14MultipleChoice() {
    cout << "🧪 Testing Question 14 Multiple Choice..." << endl;
    
    AutomobileAnxietyInventory aai(400001);
    
    // Test setting as driver
    aai.setQuestion14Response("as_a_driver");
    TEST_ASSERT(aai.getQuestion14Driver(), "Should be nervous as driver");
    TEST_ASSERT(!aai.getQuestion14Passenger(), "Should not be nervous as passenger");
    TEST_ASSERT(!aai.getQuestion14NoDifference(), "Should not be no difference");
    TEST_ASSERT_EQUAL(string("as_a_driver"), aai.getQuestion14Response(), "Response should be as_a_driver");
    
    // Test setting as passenger
    aai.setQuestion14Response("as_a_passenger");
    TEST_ASSERT(!aai.getQuestion14Driver(), "Should not be nervous as driver");
    TEST_ASSERT(aai.getQuestion14Passenger(), "Should be nervous as passenger");
    TEST_ASSERT(!aai.getQuestion14NoDifference(), "Should not be no difference");
    TEST_ASSERT_EQUAL(string("as_a_passenger"), aai.getQuestion14Response(), "Response should be as_a_passenger");
    
    cout << "✅ Question 14 Multiple Choice test passed!" << endl;
    return true;
}

bool testQuestion15WithText() {
    cout << "🧪 Testing Question 15 with Text..." << endl;
    
    AutomobileAnxietyInventory aai(400001);
    
    // Test yes/no part
    aai.setQuestion15A(true);
    TEST_ASSERT(aai.getQuestion15A(), "Question 15A should be true");
    
    // Test text part
    string test_text = "Family members and strangers";
    aai.setQuestion15B(test_text);
    TEST_ASSERT_EQUAL(string("FAMILY MEMBERS AND STRANGERS"), aai.getQuestion15B(), "Text should be normalized");
    
    // Test total count includes question 15a
    TEST_ASSERT_EQUAL(1, aai.getTotalYesResponses(), "Should have 1 yes response");
    
    cout << "✅ Question 15 with Text test passed!" << endl;
    return true;
}

bool testQuestion19WithCheckboxes() {
    cout << "🧪 Testing Question 19 with Checkboxes..." << endl;
    
    AutomobileAnxietyInventory aai(400001);
    
    // Test main question
    aai.setQuestion19(true);
    TEST_ASSERT(aai.getQuestion19(), "Question 19 should be true");
    
    // Test checkboxes
    aai.setQuestion19Options(true, false, true); // sidewalks and both
    TEST_ASSERT(aai.getQuestion19Sidewalks(), "Should be nervous on sidewalks");
    TEST_ASSERT(!aai.getQuestion19Crossing(), "Should not be nervous crossing streets");
    TEST_ASSERT(aai.getQuestion19Both(), "Should be nervous on both");
    
    // Test options string
    string options = aai.getQuestion19Options();
    TEST_ASSERT(options.find("sidewalks") != string::npos, "Options should contain sidewalks");
    TEST_ASSERT(options.find("both") != string::npos, "Options should contain both");
    
    cout << "✅ Question 19 with Checkboxes test passed!" << endl;
    return true;
}

bool testAnxietyScoring() {
    cout << "🧪 Testing Anxiety Scoring..." << endl;
    
    AutomobileAnxietyInventory aai(400001);
    
    // Initially no anxiety
    TEST_ASSERT_EQUAL(0, aai.getTotalYesResponses(), "Initial yes responses should be 0");
    TEST_ASSERT_EQUAL(0.0, aai.getAnxietyScore(), "Initial anxiety score should be 0.0");
    TEST_ASSERT(!aai.hasHighAnxiety(), "Should not have high anxiety initially");
    
    // Set multiple questions to yes
    aai.setQuestion1(true);   // 1/22
    aai.setQuestion9(true);   // 2/22
    aai.setQuestion15A(true); // 3/22
    aai.setQuestion20(true);  // 4/22
    aai.setQuestion23(true);  // 5/22
    
    TEST_ASSERT_EQUAL(5, aai.getTotalYesResponses(), "Should have 5 yes responses");
    TEST_ASSERT_NEAR(22.73, aai.getAnxietyScore(), 0.1, "Anxiety score should be around 22.73%");
    TEST_ASSERT(!aai.hasHighAnxiety(), "Should not have high anxiety at 22.73%");
    
    cout << "✅ Anxiety Scoring test passed!" << endl;
    return true;
}

bool testValidation() {
    cout << "🧪 Testing Validation..." << endl;
    
    AutomobileAnxietyInventory aai;
    
    // Test invalid case profile ID
    TEST_ASSERT(!aai.isValidCaseProfileId(100), "ID 100 should be invalid");
    TEST_ASSERT(!aai.isValidCaseProfileId(300001), "Client ID should be invalid for case profile");
    TEST_ASSERT(aai.isValidCaseProfileId(400001), "Valid CaseProfile ID should be valid");
    
    // Test question 15 text validation
    string short_text = "Parents and siblings";
    string long_text(600, 'x'); // 600 characters
    
    TEST_ASSERT(aai.isValidQuestion15Text(short_text), "Short text should be valid");
    TEST_ASSERT(!aai.isValidQuestion15Text(long_text), "Long text should be invalid");
    
    cout << "✅ Validation test passed!" << endl;
    return true;
}

bool testDisplayMethods() {
    cout << "🧪 Testing Display Methods..." << endl;
    
    AutomobileAnxietyInventory aai(400001);
    aai.setQuestion1(true);
    aai.setQuestion9(true);
    
    // Test toString
    string str = aai.toString();
    TEST_ASSERT(str.find("AAI") != string::npos, "toString should contain AAI");
    TEST_ASSERT(str.find("400001") != string::npos, "toString should contain case profile ID");
    
    cout << "✅ Display Methods test passed!" << endl;
    return true;
}

bool testStaticMethods() {
    cout << "🧪 Testing Static Methods..." << endl;
    
    // Test ID prefix
    TEST_ASSERT_EQUAL(700000, AutomobileAnxietyInventory::ID_PREFIX, "ID prefix should be 700000");
    
    // Test getNextId
    int next_id = AutomobileAnxietyInventory::getNextId();
    TEST_ASSERT(next_id > 700000, "Next ID should be greater than 700000");
    
    // Test reset
    AutomobileAnxietyInventory::resetIdCounter();
    int new_next_id = AutomobileAnxietyInventory::getNextId();
    TEST_ASSERT_EQUAL(700001, new_next_id, "Next ID after reset should be 700001");
    
    cout << "✅ Static Methods test passed!" << endl;
    return true;
}

// Main test runner
int main() {
    cout << "\n🚀 Starting AutomobileAnxietyInventory Tests..." << endl;
    cout << "=================================================" << endl;
    
    int tests_passed = 0;
    int total_tests = 0;
    
    // Run all tests
    vector<pair<string, bool(*)()>> tests = {
        {"Default Constructor", testDefaultConstructor},
        {"Constructor with Case Profile ID", testConstructorWithCaseProfileId},
        {"ID Generation", testIdGeneration},
        {"Simple Yes/No Questions", testSimpleYesNoQuestions},
        {"Question 14 Multiple Choice", testQuestion14MultipleChoice},
        {"Question 15 with Text", testQuestion15WithText},
        {"Question 19 with Checkboxes", testQuestion19WithCheckboxes},
        {"Anxiety Scoring", testAnxietyScoring},
        {"Validation", testValidation},
        {"Display Methods", testDisplayMethods},
        {"Static Methods", testStaticMethods}
    };
    
    for (auto& test : tests) {
        total_tests++;
        cout << "\n📋 Running: " << test.first << endl;
        if (test.second()) {
            tests_passed++;
        } else {
            cout << "❌ Test failed: " << test.first << endl;
        }
    }
    
    cout << "\n=================================================" << endl;
    cout << "🎯 Test Results: " << tests_passed << "/" << total_tests << " passed" << endl;
    
    if (tests_passed == total_tests) {
        cout << "🎉 All AutomobileAnxietyInventory tests passed!" << endl;
        return 0;
    } else {
        cout << "❌ Some tests failed!" << endl;
        return 1;
    }
}
