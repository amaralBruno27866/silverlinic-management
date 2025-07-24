#include "forms/BeckAnxietyInventory.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

using namespace SilverClinic::Forms;
using namespace SilverClinic;
using namespace std;

// Test Helper Function
void runTest(const string& testName, bool condition) {
    cout << "Test: " << testName << " - ";
    if (condition) {
        cout << "PASSED" << endl;
    } else {
        cout << "FAILED" << endl;
        assert(condition); // This will terminate if test fails
    }
}

void testDefaultConstructor() {
    cout << "\n=== Testing Default Constructor ===" << endl;
    
    BeckAnxietyInventory::resetIdCounter();
    BeckAnxietyInventory bai;
    
    runTest("Default constructor creates valid BAI ID", bai.getBAIId() == 900001);
    runTest("Default constructor sets type to BAI", bai.getType() == "BAI");
    runTest("Default constructor sets case profile ID to 0", bai.getCaseProfileId() == 0);
    runTest("Default constructor initializes all questions to 0", bai.getTotalScore() == 0);
    runTest("Default constructor sets minimal anxiety level", bai.getSeverityLevel() == "Minimal");
}

void testConstructorWithCaseProfileId() {
    cout << "\n=== Testing Constructor with Case Profile ID ===" << endl;
    
    BeckAnxietyInventory::resetIdCounter();
    BeckAnxietyInventory bai(12345);
    
    runTest("Constructor with case profile ID creates valid BAI ID", bai.getBAIId() == 900001);
    runTest("Constructor with case profile ID sets correct case profile ID", bai.getCaseProfileId() == 12345);
    runTest("Constructor with case profile ID sets type to BAI", bai.getType() == "BAI");
    runTest("Constructor with case profile ID initializes all questions to 0", bai.getTotalScore() == 0);
}

void testFullConstructor() {
    cout << "\n=== Testing Full Constructor ===" << endl;
    
    DateTime created = DateTime::now();
    DateTime updated = DateTime::now();
    
    BeckAnxietyInventory bai(900999, 54321,
                           1, 2, 3, 0, 1, 2, 3, 0, 1, 2,
                           3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1,
                           created, updated);
    
    runTest("Full constructor sets correct BAI ID", bai.getBAIId() == 900999);
    runTest("Full constructor sets correct case profile ID", bai.getCaseProfileId() == 54321);
    runTest("Full constructor sets correct total score", bai.getTotalScore() == 32);
    runTest("Full constructor sets correct question 1", bai.getQuestion1() == 1);
    runTest("Full constructor sets correct question 21", bai.getQuestion21() == 1);
}

void testQuestionSettersAndGetters() {
    cout << "\n=== Testing Question Setters and Getters ===" << endl;
    
    BeckAnxietyInventory bai;
    
    // Test setting valid values (0-3)
    bai.setQuestion1(3);
    runTest("Question 1 setter/getter with valid value", bai.getQuestion1() == 3);
    
    bai.setQuestion10(2);
    runTest("Question 10 setter/getter with valid value", bai.getQuestion10() == 2);
    
    bai.setQuestion21(1);
    runTest("Question 21 setter/getter with valid value", bai.getQuestion21() == 1);
    
    // Test all questions with different values
    bai.setQuestion2(1);
    bai.setQuestion3(2);
    bai.setQuestion4(3);
    bai.setQuestion5(0);
    
    runTest("Multiple question setters work correctly", 
            bai.getQuestion2() == 1 && bai.getQuestion3() == 2 && 
            bai.getQuestion4() == 3 && bai.getQuestion5() == 0);
}

void testQuestionValidation() {
    cout << "\n=== Testing Question Validation ===" << endl;
    
    BeckAnxietyInventory bai;
    
    // Set initial valid value
    bai.setQuestion1(2);
    runTest("Initial valid value is set", bai.getQuestion1() == 2);
    
    // Try to set invalid values (should be ignored)
    bai.setQuestion1(-1);
    runTest("Negative value is rejected", bai.getQuestion1() == 2);
    
    bai.setQuestion1(4);
    runTest("Value > 3 is rejected", bai.getQuestion1() == 2);
    
    bai.setQuestion1(0);
    runTest("Valid boundary value 0 is accepted", bai.getQuestion1() == 0);
    
    bai.setQuestion1(3);
    runTest("Valid boundary value 3 is accepted", bai.getQuestion1() == 3);
}

void testCaseProfileIdValidation() {
    cout << "\n=== Testing Case Profile ID Validation ===" << endl;
    
    BeckAnxietyInventory bai;
    
    bai.setCaseProfileId(100);
    runTest("Valid case profile ID is set", bai.getCaseProfileId() == 100);
    
    runTest("Valid case profile ID validation", bai.isValidCaseProfileId(100));
    runTest("Invalid case profile ID validation (0)", !bai.isValidCaseProfileId(0));
    runTest("Invalid case profile ID validation (negative)", !bai.isValidCaseProfileId(-5));
}

void testValidationMethods() {
    cout << "\n=== Testing Validation Methods ===" << endl;
    
    BeckAnxietyInventory bai;
    
    runTest("isValidQuestionValue accepts 0", bai.isValidQuestionValue(0));
    runTest("isValidQuestionValue accepts 1", bai.isValidQuestionValue(1));
    runTest("isValidQuestionValue accepts 2", bai.isValidQuestionValue(2));
    runTest("isValidQuestionValue accepts 3", bai.isValidQuestionValue(3));
    runTest("isValidQuestionValue rejects -1", !bai.isValidQuestionValue(-1));
    runTest("isValidQuestionValue rejects 4", !bai.isValidQuestionValue(4));
    
    // Test isValidData with invalid case profile ID
    runTest("isValidData returns false for invalid case profile ID", !bai.isValidData());
    
    // Set valid case profile ID and test again
    bai.setCaseProfileId(123);
    runTest("isValidData returns true for valid data", bai.isValidData());
}

void testScoringMethods() {
    cout << "\n=== Testing Scoring Methods ===" << endl;
    
    BeckAnxietyInventory bai;
    
    // Test with all zeros
    runTest("Total score is 0 with all zeros", bai.getTotalScore() == 0);
    runTest("Anxiety percentage is 0% with all zeros", bai.getAnxietyPercentage() == 0.0);
    
    // Set all questions to 1
    for (int i = 1; i <= 21; ++i) {
        if (i == 1) bai.setQuestion1(1);
        else if (i == 2) bai.setQuestion2(1);
        else if (i == 3) bai.setQuestion3(1);
        else if (i == 4) bai.setQuestion4(1);
        else if (i == 5) bai.setQuestion5(1);
        else if (i == 6) bai.setQuestion6(1);
        else if (i == 7) bai.setQuestion7(1);
        else if (i == 8) bai.setQuestion8(1);
        else if (i == 9) bai.setQuestion9(1);
        else if (i == 10) bai.setQuestion10(1);
        else if (i == 11) bai.setQuestion11(1);
        else if (i == 12) bai.setQuestion12(1);
        else if (i == 13) bai.setQuestion13(1);
        else if (i == 14) bai.setQuestion14(1);
        else if (i == 15) bai.setQuestion15(1);
        else if (i == 16) bai.setQuestion16(1);
        else if (i == 17) bai.setQuestion17(1);
        else if (i == 18) bai.setQuestion18(1);
        else if (i == 19) bai.setQuestion19(1);
        else if (i == 20) bai.setQuestion20(1);
        else if (i == 21) bai.setQuestion21(1);
    }
    
    runTest("Total score is 21 with all ones", bai.getTotalScore() == 21);
    runTest("Anxiety percentage is 33.3% with all ones", 
            abs(bai.getAnxietyPercentage() - 33.333333) < 0.001);
    
    // Test maximum score
    BeckAnxietyInventory maxBai;
    for (int i = 1; i <= 21; ++i) {
        if (i == 1) maxBai.setQuestion1(3);
        else if (i == 2) maxBai.setQuestion2(3);
        else if (i == 3) maxBai.setQuestion3(3);
        else if (i == 4) maxBai.setQuestion4(3);
        else if (i == 5) maxBai.setQuestion5(3);
        else if (i == 6) maxBai.setQuestion6(3);
        else if (i == 7) maxBai.setQuestion7(3);
        else if (i == 8) maxBai.setQuestion8(3);
        else if (i == 9) maxBai.setQuestion9(3);
        else if (i == 10) maxBai.setQuestion10(3);
        else if (i == 11) maxBai.setQuestion11(3);
        else if (i == 12) maxBai.setQuestion12(3);
        else if (i == 13) maxBai.setQuestion13(3);
        else if (i == 14) maxBai.setQuestion14(3);
        else if (i == 15) maxBai.setQuestion15(3);
        else if (i == 16) maxBai.setQuestion16(3);
        else if (i == 17) maxBai.setQuestion17(3);
        else if (i == 18) maxBai.setQuestion18(3);
        else if (i == 19) maxBai.setQuestion19(3);
        else if (i == 20) maxBai.setQuestion20(3);
        else if (i == 21) maxBai.setQuestion21(3);
    }
    
    runTest("Maximum total score is 63", maxBai.getTotalScore() == 63);
    runTest("Maximum anxiety percentage is 100%", maxBai.getAnxietyPercentage() == 100.0);
}

void testSeverityLevels() {
    cout << "\n=== Testing Severity Levels ===" << endl;
    
    BeckAnxietyInventory bai;
    
    // Test Minimal (0-13)
    runTest("Score 0 is Minimal", bai.getSeverityLevel() == "Minimal" && bai.hasMinimalAnxiety());
    
    // Set score to 13 (still minimal)
    bai.setQuestion1(3);
    bai.setQuestion2(3);
    bai.setQuestion3(3);
    bai.setQuestion4(3);
    bai.setQuestion5(1);
    runTest("Score 13 is Minimal", bai.getTotalScore() == 13 && bai.getSeverityLevel() == "Minimal");
    
    // Test Mild (14-19)
    bai.setQuestion6(1);
    runTest("Score 14 is Mild", bai.getTotalScore() == 14 && bai.getSeverityLevel() == "Mild" && bai.hasMildAnxiety());
    
    // Test Moderate (20-28)
    bai.setQuestion7(3);
    bai.setQuestion8(3);
    runTest("Score 20 is Moderate", bai.getTotalScore() == 20 && bai.getSeverityLevel() == "Moderate" && bai.hasModerateAnxiety());
    
    // Test Severe (29-63)
    bai.setQuestion9(3);
    bai.setQuestion10(3);
    bai.setQuestion11(3);
    runTest("Score 29 is Severe", bai.getTotalScore() == 29 && bai.getSeverityLevel() == "Severe" && bai.hasSevereAnxiety());
}

void testStaticMethods() {
    cout << "\n=== Testing Static Methods ===" << endl;
    
    runTest("interpretScore(0) returns Minimal", BeckAnxietyInventory::interpretScore(0) == "Minimal");
    runTest("interpretScore(13) returns Minimal", BeckAnxietyInventory::interpretScore(13) == "Minimal");
    runTest("interpretScore(14) returns Mild", BeckAnxietyInventory::interpretScore(14) == "Mild");
    runTest("interpretScore(19) returns Mild", BeckAnxietyInventory::interpretScore(19) == "Mild");
    runTest("interpretScore(20) returns Moderate", BeckAnxietyInventory::interpretScore(20) == "Moderate");
    runTest("interpretScore(28) returns Moderate", BeckAnxietyInventory::interpretScore(28) == "Moderate");
    runTest("interpretScore(29) returns Severe", BeckAnxietyInventory::interpretScore(29) == "Severe");
    runTest("interpretScore(63) returns Severe", BeckAnxietyInventory::interpretScore(63) == "Severe");
    runTest("interpretScore(100) returns Invalid", BeckAnxietyInventory::interpretScore(100) == "Invalid");
    
    runTest("isHighRiskScore(19) returns false", !BeckAnxietyInventory::isHighRiskScore(19));
    runTest("isHighRiskScore(20) returns true", BeckAnxietyInventory::isHighRiskScore(20));
    runTest("isHighRiskScore(50) returns true", BeckAnxietyInventory::isHighRiskScore(50));
}

void testCategoryScores() {
    cout << "\n=== Testing Category Scores ===" << endl;
    
    BeckAnxietyInventory bai;
    
    // Set specific questions for cognitive symptoms (2, 3, 7, 8, 9, 13, 14)
    bai.setQuestion2(1);
    bai.setQuestion3(2);
    bai.setQuestion7(1);
    bai.setQuestion8(2);
    bai.setQuestion9(1);
    bai.setQuestion13(2);
    bai.setQuestion14(1);
    
    runTest("Cognitive score calculation", bai.getCognitiveScore() == 10);
    
    // Set specific questions for affective symptoms (1, 4, 5, 6, 10, 12, 17)
    bai.setQuestion1(1);
    bai.setQuestion4(2);
    bai.setQuestion5(1);
    bai.setQuestion6(2);
    bai.setQuestion10(1);
    bai.setQuestion12(2);
    bai.setQuestion17(1);
    
    runTest("Affective score calculation", bai.getAffectiveScore() == 10);
    
    // Set specific questions for somatic symptoms (11, 15, 16, 18, 19, 20, 21)
    bai.setQuestion11(1);
    bai.setQuestion15(2);
    bai.setQuestion16(1);
    bai.setQuestion18(2);
    bai.setQuestion19(1);
    bai.setQuestion20(2);
    bai.setQuestion21(1);
    
    runTest("Somatic score calculation", bai.getSomaticScore() == 10);
}

void testIdGeneration() {
    cout << "\n=== Testing ID Generation ===" << endl;
    
    BeckAnxietyInventory::resetIdCounter();
    
    runTest("getNextId returns correct next ID", BeckAnxietyInventory::getNextId() == 900001);
    
    BeckAnxietyInventory bai1;
    runTest("First BAI gets ID 900001", bai1.getBAIId() == 900001);
    
    BeckAnxietyInventory bai2;
    runTest("Second BAI gets ID 900002", bai2.getBAIId() == 900002);
    
    runTest("getNextId returns correct next ID after creation", BeckAnxietyInventory::getNextId() == 900003);
}

void testToStringMethod() {
    cout << "\n=== Testing toString Method ===" << endl;
    
    BeckAnxietyInventory::resetIdCounter();
    BeckAnxietyInventory bai(12345);
    bai.setQuestion1(1);
    bai.setQuestion2(2);
    
    string str = bai.toString();
    runTest("toString contains BAI ID", str.find("900001") != string::npos);
    runTest("toString contains case ID", str.find("12345") != string::npos);
    runTest("toString contains score", str.find("3") != string::npos);
    runTest("toString contains level", str.find("Minimal") != string::npos);
}

void testTimestamps() {
    cout << "\n=== Testing Timestamps ===" << endl;
    
    BeckAnxietyInventory bai;
    
    DateTime created = bai.getBAICreatedAt();
    DateTime updated = bai.getBAIUpdatedAt();
    
    runTest("Created and updated timestamps are initially the same", created.toString() == updated.toString());
    
    // Small delay to ensure different timestamp
    for (volatile int i = 0; i < 1000000; ++i) {}
    
    bai.setQuestion1(1);
    DateTime newUpdated = bai.getBAIUpdatedAt();
    
    runTest("Updated timestamp changes after modification", updated.toString() != newUpdated.toString());
}

int main() {
    cout << "Running Beck Anxiety Inventory Tests..." << endl;
    
    try {
        testDefaultConstructor();
        testConstructorWithCaseProfileId();
        testFullConstructor();
        testQuestionSettersAndGetters();
        testQuestionValidation();
        testCaseProfileIdValidation();
        testValidationMethods();
        testScoringMethods();
        testSeverityLevels();
        testStaticMethods();
        testCategoryScores();
        testIdGeneration();
        testToStringMethod();
        testTimestamps();
        
        cout << "\n=== All Beck Anxiety Inventory Tests PASSED! ===" << endl;
        cout << "Total tests run: 56" << endl;
        
    } catch (const exception& e) {
        cout << "\nTest failed with exception: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
