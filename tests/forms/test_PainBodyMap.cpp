#include "forms/PainBodyMap.h"
#include <iostream>
#include <cassert>

using namespace std;
using namespace SilverClinic::Forms;

// Simple test framework macros
#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        cout << "❌ FAIL: " << message << endl; \
        return false; \
    } else { \
        cout << "✅ PASS: " << message << endl; \
    }

#define RUN_TEST(test_function) \
    cout << "\n🧪 Running " << #test_function << "..." << endl; \
    if (test_function()) { \
        cout << "✅ " << #test_function << " completed successfully" << endl; \
    } else { \
        cout << "❌ " << #test_function << " FAILED!" << endl; \
        return 1; \
    }

// Test BodyPartPain struct
bool test_BodyPartPainConstructor() {
    BodyPartPain pain;
    
    TEST_ASSERT(pain.body_part == "", "Default body_part should be empty");
    TEST_ASSERT(pain.side_left == false, "Default side_left should be false");
    TEST_ASSERT(pain.side_right == false, "Default side_right should be false");
    TEST_ASSERT(pain.pain_level == 0, "Default pain_level should be 0");
    TEST_ASSERT(pain.comments == "", "Default comments should be empty");
    
    return true;
}

bool test_BodyPartPainParameterizedConstructor() {
    BodyPartPain pain("head", true, false, 7, "Severe headache");
    
    TEST_ASSERT(pain.body_part == "head", "Body part should be 'head'");
    TEST_ASSERT(pain.side_left == true, "Left side should be true");
    TEST_ASSERT(pain.side_right == false, "Right side should be false");
    TEST_ASSERT(pain.pain_level == 7, "Pain level should be 7");
    TEST_ASSERT(pain.comments == "Severe headache", "Comments should match");
    
    return true;
}

bool test_BodyPartPainJsonSerialization() {
    BodyPartPain pain("shoulders", true, true, 5, "Stiff muscles");
    
    string json = pain.toJson();
    TEST_ASSERT(json.find("\"body_part\":\"shoulders\"") != string::npos, "JSON should contain body_part");
    TEST_ASSERT(json.find("\"side_left\":true") != string::npos, "JSON should contain side_left");
    TEST_ASSERT(json.find("\"side_right\":true") != string::npos, "JSON should contain side_right");
    TEST_ASSERT(json.find("\"pain_level\":5") != string::npos, "JSON should contain pain_level");
    TEST_ASSERT(json.find("\"comments\":\"Stiff muscles\"") != string::npos, "JSON should contain comments");
    
    return true;
}

bool test_BodyPartPainJsonDeserialization() {
    string json = R"({"body_part":"jaw","side_left":false,"side_right":true,"pain_level":3,"comments":"TMJ pain"})";
    
    BodyPartPain pain;
    pain.fromJson(json);
    
    TEST_ASSERT(pain.body_part == "jaw", "Body part should be 'jaw'");
    TEST_ASSERT(pain.side_left == false, "Left side should be false");
    TEST_ASSERT(pain.side_right == true, "Right side should be true");
    TEST_ASSERT(pain.pain_level == 3, "Pain level should be 3");
    TEST_ASSERT(pain.comments == "TMJ pain", "Comments should match");
    
    return true;
}

// Test PainBodyMap constructors
bool test_DefaultConstructor() {
    PainBodyMap::resetIdCounter();
    PainBodyMap pbm;
    
    TEST_ASSERT(pbm.getPBMId() > PainBodyMap::ID_PREFIX, "ID should be greater than prefix");
    TEST_ASSERT(pbm.getCaseProfileId() == 0, "Default case profile ID should be 0");
    TEST_ASSERT(pbm.getType() == "PBM", "Type should be 'PBM'");
    TEST_ASSERT(pbm.getPainDataJson() == "{}", "Pain data JSON should be empty");
    TEST_ASSERT(pbm.getAdditionalComments() == "", "Additional comments should be empty");
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 0, "No affected body parts initially");
    
    return true;
}

bool test_ParameterizedConstructor() {
    int case_id = 400001; // Valid case profile ID
    PainBodyMap pbm(case_id);
    
    TEST_ASSERT(pbm.getPBMId() > PainBodyMap::ID_PREFIX, "ID should be greater than prefix");
    TEST_ASSERT(pbm.getCaseProfileId() == case_id, "Case profile ID should match");
    TEST_ASSERT(pbm.getType() == "PBM", "Type should be 'PBM'");
    TEST_ASSERT(pbm.getPainDataJson() == "{}", "Pain data JSON should be empty");
    
    return true;
}

bool test_ParameterizedConstructorInvalidCaseId() {
    bool exception_thrown = false;
    
    try {
        PainBodyMap pbm(-1);
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for invalid case ID");
    
    exception_thrown = false;
    try {
        PainBodyMap pbm(100); // Below case profile prefix
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for case ID below prefix");
    
    return true;
}

// Test pain data manipulation
bool test_SetPainForBodyPart() {
    PainBodyMap pbm(400001);
    
    pbm.setPainForBodyPart("head", true, false, 7, "Severe headache");
    
    TEST_ASSERT(pbm.hasEntryForBodyPart("head"), "Should have entry for 'head'");
    
    BodyPartPain pain = pbm.getPainForBodyPart("head");
    TEST_ASSERT(pain.body_part == "head", "Body part should be 'head'");
    TEST_ASSERT(pain.side_left == true, "Left side should be true");
    TEST_ASSERT(pain.side_right == false, "Right side should be false");
    TEST_ASSERT(pain.pain_level == 7, "Pain level should be 7");
    TEST_ASSERT(pain.comments == "Severe headache", "Comments should match");
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 1, "Should have 1 affected body part");
    
    return true;
}

bool test_SetPainForBodyPartStruct() {
    PainBodyMap pbm(400001);
    BodyPartPain pain("shoulders", true, true, 5, "Muscle tension");
    
    pbm.setPainForBodyPart(pain);
    
    TEST_ASSERT(pbm.hasEntryForBodyPart("shoulders"), "Should have entry for 'shoulders'");
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 1, "Should have 1 affected body part");
    
    BodyPartPain retrieved = pbm.getPainForBodyPart("shoulders");
    TEST_ASSERT(retrieved.body_part == "shoulders", "Body part should be 'shoulders'");
    TEST_ASSERT(retrieved.side_left == true, "Left side should be true");
    TEST_ASSERT(retrieved.side_right == true, "Right side should be true");
    TEST_ASSERT(retrieved.pain_level == 5, "Pain level should be 5");
    
    return true;
}

bool test_SetInvalidBodyPart() {
    PainBodyMap pbm(400001);
    bool exception_thrown = false;
    
    try {
        pbm.setPainForBodyPart("invalid_part", true, false, 5);
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for invalid body part");
    
    return true;
}

bool test_SetInvalidPainLevel() {
    PainBodyMap pbm(400001);
    bool exception_thrown = false;
    
    try {
        pbm.setPainForBodyPart("head", true, false, -1);
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for negative pain level");
    
    exception_thrown = false;
    try {
        pbm.setPainForBodyPart("head", true, false, 11);
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for pain level > 10");
    
    return true;
}

bool test_SetInvalidComments() {
    PainBodyMap pbm(400001);
    string longComment(501, 'x'); // exceeds 500 character limit in implementation
    bool exception_thrown = false;
    
    try {
        pbm.setPainForBodyPart("head", true, false, 5, longComment);
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for comments too long");
    
    return true;
}

bool test_RemovePainForBodyPart() {
    PainBodyMap pbm(400001);
    
    pbm.setPainForBodyPart("head", true, false, 7);
    pbm.setPainForBodyPart("shoulders", false, true, 5);
    
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 2, "Should have 2 affected body parts");
    
    pbm.removePainForBodyPart("head");
    
    TEST_ASSERT(!pbm.hasEntryForBodyPart("head"), "Should not have entry for 'head'");
    TEST_ASSERT(pbm.hasEntryForBodyPart("shoulders"), "Should still have entry for 'shoulders'");
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 1, "Should have 1 affected body part");
    
    return true;
}

bool test_ClearAllPainData() {
    PainBodyMap pbm(400001);
    
    pbm.setPainForBodyPart("head", true, false, 7);
    pbm.setPainForBodyPart("shoulders", false, true, 5);
    pbm.setPainForBodyPart("low_back", true, true, 8);
    
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 3, "Should have 3 affected body parts");
    
    pbm.clearAllPainData();
    
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 0, "Should have 0 affected body parts");
    TEST_ASSERT(pbm.getPainDataJson() == "{}", "Pain data JSON should be empty");
    
    return true;
}

// Test JSON synchronization
bool test_JsonSynchronization() {
    PainBodyMap pbm(400001);
    
    // Add pain data via C++ interface
    pbm.setPainForBodyPart("head", true, false, 8, "Migraine");
    pbm.setPainForBodyPart("shoulders", true, true, 6, "Tension");
    
    // Check that JSON is updated
    string json = pbm.getPainDataJson();
    TEST_ASSERT(json.find("head") != string::npos, "JSON should contain 'head'");
    TEST_ASSERT(json.find("shoulders") != string::npos, "JSON should contain 'shoulders'");
    TEST_ASSERT(json.find("\"pain_level\":8") != string::npos, "JSON should contain pain level 8");
    TEST_ASSERT(json.find("\"pain_level\":6") != string::npos, "JSON should contain pain level 6");
    
    // Set new JSON and verify C++ data is updated
    string newJson = R"({"jaw":{"body_part":"jaw","side_left":false,"side_right":true,"pain_level":4,"comments":"TMJ"}})";
    pbm.setPainDataJson(newJson);
    
    TEST_ASSERT(pbm.hasEntryForBodyPart("jaw"), "Should have entry for 'jaw'");
    TEST_ASSERT(!pbm.hasEntryForBodyPart("head"), "Should not have entry for 'head' after JSON update");
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 1, "Should have 1 affected body part");
    
    return true;
}

// Test validation methods
bool test_ValidationMethods() {
    PainBodyMap pbm(400001);
    
    // Test pain level validation
    TEST_ASSERT(pbm.isValidPainLevel(0), "Pain level 0 should be valid");
    TEST_ASSERT(pbm.isValidPainLevel(5), "Pain level 5 should be valid");
    TEST_ASSERT(pbm.isValidPainLevel(10), "Pain level 10 should be valid");
    TEST_ASSERT(!pbm.isValidPainLevel(-1), "Pain level -1 should be invalid");
    TEST_ASSERT(!pbm.isValidPainLevel(11), "Pain level 11 should be invalid");
    
    // Test body part validation
    TEST_ASSERT(pbm.isValidBodyPart("head"), "Body part 'head' should be valid");
    TEST_ASSERT(pbm.isValidBodyPart("shoulders"), "Body part 'shoulders' should be valid");
    TEST_ASSERT(pbm.isValidBodyPart("low_back"), "Body part 'low_back' should be valid");
    TEST_ASSERT(!pbm.isValidBodyPart("invalid_part"), "Body part 'invalid_part' should be invalid");
    
    // Test comments validation
    TEST_ASSERT(pbm.isValidComments(""), "Empty comments should be valid");
    TEST_ASSERT(pbm.isValidComments("Short comment"), "Short comments should be valid");
    TEST_ASSERT(pbm.isValidComments(string(500, 'x')), "500 character comments should be valid");
    TEST_ASSERT(!pbm.isValidComments(string(501, 'x')), "501 character comments should be invalid");
    
    // Test case profile ID validation
    TEST_ASSERT(pbm.isValidCaseProfileId(400001), "Case profile ID 400001 should be valid");
    TEST_ASSERT(!pbm.isValidCaseProfileId(0), "Case profile ID 0 should be invalid");
    TEST_ASSERT(!pbm.isValidCaseProfileId(-1), "Case profile ID -1 should be invalid");
    TEST_ASSERT(!pbm.isValidCaseProfileId(100), "Case profile ID 100 should be invalid");
    
    return true;
}

// Test analysis methods
bool test_AnalysisMethods() {
    PainBodyMap pbm(400001);
    
    pbm.setPainForBodyPart("head", true, false, 8, "Severe headache");
    pbm.setPainForBodyPart("shoulders", true, true, 6, "Bilateral tension");
    pbm.setPainForBodyPart("low_back", false, true, 9, "Sharp pain");
    pbm.setPainForBodyPart("knees", false, false, 0, "No pain"); // Should not count
    
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 3, "Should have 3 affected body parts (knees doesn't count)");
    TEST_ASSERT(pbm.getHighestPainLevel() == 9, "Highest pain level should be 9");
    
    double avgPain = pbm.getAveragePainLevel();
    TEST_ASSERT(avgPain > 7.6 && avgPain < 7.7, "Average pain level should be around 7.67");
    
    auto highPain = pbm.getMostPainfulBodyParts(7);
    TEST_ASSERT(highPain.size() == 2, "Should have 2 high pain areas (>=7)");
    
    auto leftSide = pbm.getLeftSideAffected();
    TEST_ASSERT(leftSide.size() == 2, "Should have 2 left side affected areas");
    
    auto rightSide = pbm.getRightSideAffected();
    TEST_ASSERT(rightSide.size() == 2, "Should have 2 right side affected areas");
    
    auto bilateral = pbm.getBilateralAffected();
    TEST_ASSERT(bilateral.size() == 1, "Should have 1 bilateral affected area");
    
    return true;
}

bool test_AnalysisMethodsEmptyData() {
    PainBodyMap pbm(400001);
    
    TEST_ASSERT(pbm.getTotalAffectedBodyParts() == 0, "Should have 0 affected body parts");
    TEST_ASSERT(pbm.getHighestPainLevel() == 0, "Highest pain level should be 0");
    TEST_ASSERT(pbm.getAveragePainLevel() == 0.0, "Average pain level should be 0.0");
    
    auto highPain = pbm.getMostPainfulBodyParts(5);
    TEST_ASSERT(highPain.empty(), "High pain areas should be empty");
    
    auto leftSide = pbm.getLeftSideAffected();
    TEST_ASSERT(leftSide.empty(), "Left side affected should be empty");
    
    return true;
}

// Test additional comments
bool test_AdditionalComments() {
    PainBodyMap pbm(400001);
    
    pbm.setAdditionalComments("Patient reports chronic pain patterns.");
    TEST_ASSERT(pbm.getAdditionalComments() == "Patient reports chronic pain patterns.", "Additional comments should match");
    
    // Test long comment within 2000 char limit (implementation limit)
    string longComment(2000, 'x');
    pbm.setAdditionalComments(longComment);
    TEST_ASSERT(pbm.getAdditionalComments() == longComment, "Long comments should be accepted");
    
    // Test comment too long
    string tooLongComment(2001, 'x');
    bool exception_thrown = false;
    try {
        pbm.setAdditionalComments(tooLongComment);
    } catch (const invalid_argument&) {
        exception_thrown = true;
    }
    TEST_ASSERT(exception_thrown, "Should throw exception for comments too long");
    
    return true;
}

// Test static methods
bool test_StaticMethods() {
    // Test ID management
    PainBodyMap::resetIdCounter();
    int firstId = PainBodyMap::getNextId();
    int secondId = PainBodyMap::getNextId();
    TEST_ASSERT(secondId == firstId, "getNextId() should return the same ID until incremented");
    
    // Create an object to increment the counter
    PainBodyMap pbm1(400001);
    int thirdId = PainBodyMap::getNextId();
    TEST_ASSERT(thirdId == firstId + 1, "After creating object, next ID should be incremented");
    
    PainBodyMap::resetIdCounter();
    int resetId = PainBodyMap::getNextId();
    TEST_ASSERT(resetId == PainBodyMap::ID_PREFIX + 1, "Reset ID should be prefix + 1");
    
    // Test body part validation
    TEST_ASSERT(PainBodyMap::isValidBodyPartName("head"), "Body part 'head' should be valid");
    TEST_ASSERT(PainBodyMap::isValidBodyPartName("shoulders"), "Body part 'shoulders' should be valid");
    TEST_ASSERT(!PainBodyMap::isValidBodyPartName("invalid"), "Body part 'invalid' should be invalid");
    
    // Test standard body parts list
    auto bodyParts = PainBodyMap::getStandardBodyParts();
    TEST_ASSERT(!bodyParts.empty(), "Standard body parts list should not be empty");
    TEST_ASSERT(find(bodyParts.begin(), bodyParts.end(), "head") != bodyParts.end(), "Should contain 'head'");
    TEST_ASSERT(find(bodyParts.begin(), bodyParts.end(), "shoulders") != bodyParts.end(), "Should contain 'shoulders'");
    TEST_ASSERT(find(bodyParts.begin(), bodyParts.end(), "low_back") != bodyParts.end(), "Should contain 'low_back'");
    
    return true;
}

// Test display methods (basic functionality)
bool test_DisplayMethods() {
    PainBodyMap pbm(400001);
    pbm.setPainForBodyPart("head", true, false, 8, "Severe headache");
    pbm.setAdditionalComments("Patient has chronic migraines");
    
    // Test toString method
    string str = pbm.toString();
    TEST_ASSERT(str.find("PainBodyMap") != string::npos, "toString should contain 'PainBodyMap'");
    TEST_ASSERT(str.find("400001") != string::npos, "toString should contain case profile ID");
    TEST_ASSERT(str.find("AffectedParts=1") != string::npos, "toString should contain affected parts count");
    TEST_ASSERT(str.find("MaxPain=8") != string::npos, "toString should contain max pain level");
    
    // Test that display methods don't crash (can't easily test output)
    cout << "Testing display methods (output will be shown):" << endl;
    pbm.displayInfo();
    pbm.displaySummary();
    pbm.displayPainMap();
    pbm.displayAnalysis();
    
    return true;
}

int main() {
    cout << "🧪 Pain Body Map Tests" << endl;
    cout << "=====================" << endl;
    
    // Reset ID counter for consistent test results
    PainBodyMap::resetIdCounter();
    
    RUN_TEST(test_BodyPartPainConstructor);
    RUN_TEST(test_BodyPartPainParameterizedConstructor);
    RUN_TEST(test_BodyPartPainJsonSerialization);
    RUN_TEST(test_BodyPartPainJsonDeserialization);
    
    RUN_TEST(test_DefaultConstructor);
    RUN_TEST(test_ParameterizedConstructor);
    RUN_TEST(test_ParameterizedConstructorInvalidCaseId);
    
    RUN_TEST(test_SetPainForBodyPart);
    RUN_TEST(test_SetPainForBodyPartStruct);
    RUN_TEST(test_SetInvalidBodyPart);
    RUN_TEST(test_SetInvalidPainLevel);
    RUN_TEST(test_SetInvalidComments);
    RUN_TEST(test_RemovePainForBodyPart);
    RUN_TEST(test_ClearAllPainData);
    
    RUN_TEST(test_JsonSynchronization);
    RUN_TEST(test_ValidationMethods);
    RUN_TEST(test_AnalysisMethods);
    RUN_TEST(test_AnalysisMethodsEmptyData);
    RUN_TEST(test_AdditionalComments);
    RUN_TEST(test_StaticMethods);
    RUN_TEST(test_DisplayMethods);
    
    cout << "\n🎉 All Pain Body Map tests completed successfully!" << endl;
    return 0;
}
