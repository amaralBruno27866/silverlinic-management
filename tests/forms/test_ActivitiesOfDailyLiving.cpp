#include "forms/ActivitiesOfDailyLiving.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

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

// Test ActivityCategory struct
bool test_ActivityCategoryConstructor() {
    ActivityCategory category;
    
    TEST_ASSERT(category.category_name == "", "Default category_name should be empty");
    TEST_ASSERT(category.activities.empty(), "Default activities should be empty");
    TEST_ASSERT(category.comments == "", "Default comments should be empty");
    
    return true;
}

bool test_ActivityCategoryParameterizedConstructor() {
    ActivityCategory category("functional_tasks");
    
    TEST_ASSERT(category.category_name == "functional_tasks", "Category name should be 'functional_tasks'");
    TEST_ASSERT(category.activities.empty(), "Activities should be empty initially");
    TEST_ASSERT(category.comments == "", "Comments should be empty initially");
    
    return true;
}

bool test_ActivityCategoryJsonSerialization() {
    ActivityCategory category("functional_tasks");
    category.activities["lifting"] = true;
    category.activities["sitting"] = false;
    category.comments = "Test comments";
    
    string json = category.toJson();
    TEST_ASSERT(!json.empty(), "JSON should not be empty");
    TEST_ASSERT(json.find("functional_tasks") != string::npos, "JSON should contain category name");
    TEST_ASSERT(json.find("lifting") != string::npos, "JSON should contain lifting activity");
    TEST_ASSERT(json.find("Test comments") != string::npos, "JSON should contain comments");
    
    // Test deserialization
    ActivityCategory newCategory;
    newCategory.fromJson(json);
    TEST_ASSERT(newCategory.category_name == "functional_tasks", "Deserialized category name should match");
    TEST_ASSERT(newCategory.activities["lifting"] == true, "Deserialized lifting should be true");
    TEST_ASSERT(newCategory.activities["sitting"] == false, "Deserialized sitting should be false");
    TEST_ASSERT(newCategory.comments == "Test comments", "Deserialized comments should match");
    
    return true;
}

bool test_ActivityCategoryAnalysis() {
    ActivityCategory category("functional_tasks");
    category.activities["lifting"] = true;
    category.activities["sitting"] = false;
    category.activities["walking_t"] = true;
    category.activities["stairs"] = false;
    
    TEST_ASSERT(category.getTotalDifficulties() == 2, "Should have 2 difficulties");
    TEST_ASSERT(category.hasDifficulties() == true, "Should have difficulties");
    
    vector<string> difficultActivities = category.getDifficultActivities();
    TEST_ASSERT(difficultActivities.size() == 2, "Should have 2 difficult activities");
    TEST_ASSERT(find(difficultActivities.begin(), difficultActivities.end(), "lifting") != difficultActivities.end(), 
                "Should contain 'lifting'");
    TEST_ASSERT(find(difficultActivities.begin(), difficultActivities.end(), "walking_t") != difficultActivities.end(), 
                "Should contain 'walking_t'");
    
    return true;
}

// Test ActivitiesOfDailyLiving class
bool test_ADLDefaultConstructor() {
    ActivitiesOfDailyLiving::resetIdCounter(); // Reset for consistent testing
    ActivitiesOfDailyLiving adl;
    
    TEST_ASSERT(adl.getADLId() >= ActivitiesOfDailyLiving::ID_PREFIX, "ADL ID should be valid");
    TEST_ASSERT(adl.getCaseProfileId() == 0, "Default case profile ID should be 0");
    TEST_ASSERT(adl.getType() == "ADL", "Type should be 'ADL'");
    TEST_ASSERT(adl.getActivitiesDataJson() == "{}", "Initial JSON should be empty object");
    
    return true;
}

bool test_ADLParameterizedConstructor() {
    ActivitiesOfDailyLiving::resetIdCounter();
    int testCaseId = 400001; // Valid case profile ID (CaseProfile::ID_PREFIX + 1)
    
    ActivitiesOfDailyLiving adl(testCaseId);
    
    TEST_ASSERT(adl.getCaseProfileId() == testCaseId, "Case profile ID should match");
    TEST_ASSERT(adl.getType() == "ADL", "Type should be 'ADL'");
    TEST_ASSERT(adl.getActivitiesDataJson() == "{}", "Initial JSON should be empty object");
    
    return true;
}

bool test_ADLInvalidCaseProfileId() {
    try {
        ActivitiesOfDailyLiving adl(-1); // Invalid case profile ID
        TEST_ASSERT(false, "Should throw exception for invalid case profile ID");
    } catch (const invalid_argument& e) {
        TEST_ASSERT(true, "Should throw invalid_argument for invalid case profile ID");
    }
    
    return true;
}

bool test_ADLStaticMethods() {
    vector<string> categories = ActivitiesOfDailyLiving::getStandardCategories();
    TEST_ASSERT(categories.size() == 5, "Should have 5 standard categories");
    TEST_ASSERT(find(categories.begin(), categories.end(), "functional_tasks") != categories.end(),
                "Should contain 'functional_tasks'");
    TEST_ASSERT(find(categories.begin(), categories.end(), "personal_care") != categories.end(),
                "Should contain 'personal_care'");
    
    TEST_ASSERT(ActivitiesOfDailyLiving::isValidCategoryName("functional_tasks"), 
                "functional_tasks should be valid category");
    TEST_ASSERT(!ActivitiesOfDailyLiving::isValidCategoryName("invalid_category"), 
                "invalid_category should not be valid");
    
    TEST_ASSERT(ActivitiesOfDailyLiving::isValidActivityName("functional_tasks", "lifting"), 
                "lifting should be valid for functional_tasks");
    TEST_ASSERT(!ActivitiesOfDailyLiving::isValidActivityName("functional_tasks", "invalid_activity"), 
                "invalid_activity should not be valid");
    
    vector<string> functionalActivities = ActivitiesOfDailyLiving::getStandardActivitiesForCategory("functional_tasks");
    TEST_ASSERT(functionalActivities.size() > 0, "Should have functional activities");
    TEST_ASSERT(find(functionalActivities.begin(), functionalActivities.end(), "lifting") != functionalActivities.end(),
                "Should contain 'lifting'");
    
    return true;
}

bool test_ADLActivityManipulation() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Test setting individual activity difficulty
    adl.setActivityDifficulty("functional_tasks", "lifting", true);
    adl.setActivityDifficulty("functional_tasks", "sitting", false);
    adl.setActivityDifficulty("personal_care", "dressing", true);
    
    TEST_ASSERT(adl.hasCategoryData("functional_tasks"), "Should have functional_tasks data");
    TEST_ASSERT(adl.hasCategoryData("personal_care"), "Should have personal_care data");
    TEST_ASSERT(!adl.hasCategoryData("household_chores"), "Should not have household_chores data");
    
    ActivityCategory funcCategory = adl.getCategoryData("functional_tasks");
    TEST_ASSERT(funcCategory.activities["lifting"] == true, "Lifting should be difficult");
    TEST_ASSERT(funcCategory.activities["sitting"] == false, "Sitting should not be difficult");
    
    return true;
}

bool test_ADLCategoryComments() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    adl.setCategoryComments("functional_tasks", "Patient has back pain affecting lifting");
    
    ActivityCategory category = adl.getCategoryData("functional_tasks");
    TEST_ASSERT(category.comments == "Patient has back pain affecting lifting", 
                "Comments should be set correctly");
    
    // Test long comments validation
    string longComment(1001, 'x'); // 1001 characters
    try {
        adl.setCategoryComments("functional_tasks", longComment);
        TEST_ASSERT(false, "Should throw exception for comments too long");
    } catch (const invalid_argument& e) {
        TEST_ASSERT(true, "Should throw invalid_argument for comments too long");
    }
    
    return true;
}

bool test_ADLCategoryDataManipulation() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    ActivityCategory category("personal_care");
    category.activities["dressing"] = true;
    category.activities["bathing"] = true;
    category.activities["shaving"] = false;
    category.comments = "Needs assistance with upper body tasks";
    
    adl.setCategoryData(category);
    
    TEST_ASSERT(adl.hasCategoryData("personal_care"), "Should have personal care data");
    
    ActivityCategory retrieved = adl.getCategoryData("personal_care");
    TEST_ASSERT(retrieved.activities["dressing"] == true, "Dressing should be difficult");
    TEST_ASSERT(retrieved.activities["bathing"] == true, "Bathing should be difficult");
    TEST_ASSERT(retrieved.activities["shaving"] == false, "Shaving should not be difficult");
    TEST_ASSERT(retrieved.comments == "Needs assistance with upper body tasks", "Comments should match");
    
    // Test removal
    adl.removeCategoryData("personal_care");
    TEST_ASSERT(!adl.hasCategoryData("personal_care"), "Should not have personal care data after removal");
    
    return true;
}

bool test_ADLAnalysisMethods() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Set up test data
    adl.setActivityDifficulty("functional_tasks", "lifting", true);
    adl.setActivityDifficulty("functional_tasks", "walking_t", true);
    adl.setActivityDifficulty("personal_care", "dressing", true);
    adl.setActivityDifficulty("household_chores", "cooking", true);
    adl.setActivityDifficulty("child_care", "tk_children", true);
    
    TEST_ASSERT(adl.getTotalDifficulties() == 5, "Should have 5 total difficulties");
    TEST_ASSERT(adl.getDifficultiesInCategory("functional_tasks") == 2, 
                "Should have 2 difficulties in functional_tasks");
    TEST_ASSERT(adl.getDifficultiesInCategory("personal_care") == 1, 
                "Should have 1 difficulty in personal_care");
    
    vector<string> allDifficult = adl.getAllDifficultActivities();
    TEST_ASSERT(allDifficult.size() == 5, "Should have 5 difficult activities total");
    
    vector<string> functionalDifficult = adl.getDifficultActivitiesInCategory("functional_tasks");
    TEST_ASSERT(functionalDifficult.size() == 2, "Should have 2 difficult functional activities");
    
    return true;
}

bool test_ADLClinicalAnalysis() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Test independence assessment
    TEST_ASSERT(!adl.needsAssistanceWithPersonalCare(), "Should not need personal care assistance initially");
    TEST_ASSERT(!adl.needsAssistanceWithHousework(), "Should not need housework assistance initially");
    TEST_ASSERT(!adl.needsAssistanceWithChildCare(), "Should not need child care assistance initially");
    TEST_ASSERT(adl.canLiveIndependently(), "Should be able to live independently initially");
    
    // Add some difficulties
    adl.setActivityDifficulty("personal_care", "dressing", true);
    adl.setActivityDifficulty("household_chores", "cooking", true);
    adl.setActivityDifficulty("child_care", "tk_children", true);
    
    TEST_ASSERT(adl.needsAssistanceWithPersonalCare(), "Should need personal care assistance");
    TEST_ASSERT(adl.needsAssistanceWithHousework(), "Should need housework assistance");
    TEST_ASSERT(adl.needsAssistanceWithChildCare(), "Should need child care assistance");
    
    double independence = adl.getIndependencePercentage();
    TEST_ASSERT(independence >= 0.0 && independence <= 100.0, "Independence percentage should be 0-100");
    
    string level = adl.getIndependenceLevel();
    TEST_ASSERT(!level.empty(), "Independence level should not be empty");
    
    return true;
}

bool test_ADLMostProblematicCategories() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Set up different levels of difficulties
    adl.setActivityDifficulty("functional_tasks", "lifting", true);
    adl.setActivityDifficulty("functional_tasks", "walking_t", true);
    adl.setActivityDifficulty("functional_tasks", "stairs", true); // 3 difficulties
    
    adl.setActivityDifficulty("personal_care", "dressing", true);
    adl.setActivityDifficulty("personal_care", "bathing", true); // 2 difficulties
    
    adl.setActivityDifficulty("household_chores", "cooking", true); // 1 difficulty
    
    vector<string> problematic = adl.getMostProblematicCategories();
    TEST_ASSERT(problematic.size() == 3, "Should have 3 problematic categories");
    TEST_ASSERT(problematic[0] == "functional_tasks", "Most problematic should be functional_tasks");
    TEST_ASSERT(problematic[1] == "personal_care", "Second most problematic should be personal_care");
    TEST_ASSERT(problematic[2] == "household_chores", "Third most problematic should be household_chores");
    
    return true;
}

bool test_ADLJsonPersistence() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Set up test data
    adl.setActivityDifficulty("functional_tasks", "lifting", true);
    adl.setActivityDifficulty("personal_care", "dressing", true);
    adl.setCategoryComments("functional_tasks", "Back pain issues");
    
    string json = adl.getActivitiesDataJson();
    TEST_ASSERT(!json.empty(), "JSON should not be empty");
    TEST_ASSERT(json != "{}", "JSON should contain data");
    
    // Create new ADL and load from JSON
    ActivitiesOfDailyLiving newAdl(400002);
    newAdl.setActivitiesDataJson(json);
    
    TEST_ASSERT(newAdl.hasCategoryData("functional_tasks"), "New ADL should have functional_tasks data");
    TEST_ASSERT(newAdl.hasCategoryData("personal_care"), "New ADL should have personal_care data");
    
    ActivityCategory funcCategory = newAdl.getCategoryData("functional_tasks");
    TEST_ASSERT(funcCategory.activities["lifting"] == true, "Lifting should be difficult in new ADL");
    TEST_ASSERT(funcCategory.comments == "Back pain issues", "Comments should be preserved");
    
    ActivityCategory personalCategory = newAdl.getCategoryData("personal_care");
    TEST_ASSERT(personalCategory.activities["dressing"] == true, "Dressing should be difficult in new ADL");
    
    return true;
}

bool test_ADLValidation() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    TEST_ASSERT(adl.isValidData(), "ADL with valid case profile should be valid");
    
    // Test invalid category
    try {
        adl.setActivityDifficulty("invalid_category", "lifting", true);
        TEST_ASSERT(false, "Should throw exception for invalid category");
    } catch (const invalid_argument& e) {
        TEST_ASSERT(true, "Should throw invalid_argument for invalid category");
    }
    
    // Test invalid activity
    try {
        adl.setActivityDifficulty("functional_tasks", "invalid_activity", true);
        TEST_ASSERT(false, "Should throw exception for invalid activity");
    } catch (const invalid_argument& e) {
        TEST_ASSERT(true, "Should throw invalid_argument for invalid activity");
    }
    
    return true;
}

bool test_ADLClearAllData() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Add some data
    adl.setActivityDifficulty("functional_tasks", "lifting", true);
    adl.setActivityDifficulty("personal_care", "dressing", true);
    adl.setCategoryComments("functional_tasks", "Test comments");
    
    TEST_ASSERT(adl.getTotalDifficulties() > 0, "Should have difficulties before clearing");
    
    adl.clearAllActivityData();
    
    TEST_ASSERT(adl.getTotalDifficulties() == 0, "Should have no difficulties after clearing");
    TEST_ASSERT(adl.getActivitiesDataJson() == "{}", "JSON should be empty after clearing");
    TEST_ASSERT(!adl.hasCategoryData("functional_tasks"), "Should not have functional_tasks data after clearing");
    TEST_ASSERT(!adl.hasCategoryData("personal_care"), "Should not have personal_care data after clearing");
    
    return true;
}

bool test_ADLDisplayMethods() {
    ActivitiesOfDailyLiving::resetIdCounter();
    ActivitiesOfDailyLiving adl(400001);
    
    // Add some test data
    adl.setActivityDifficulty("functional_tasks", "lifting", true);
    adl.setActivityDifficulty("personal_care", "dressing", true);
    
    cout << "\n=== Testing Display Methods ===" << endl;
    
    // Test toString
    string str = adl.toString();
    TEST_ASSERT(!str.empty(), "toString should not be empty");
    TEST_ASSERT(str.find("ActivitiesOfDailyLiving") != string::npos, "toString should contain class name");
    
    cout << "\nTesting displayInfo():" << endl;
    adl.displayInfo();
    
    cout << "\nTesting displaySummary():" << endl;
    adl.displaySummary();
    
    cout << "\nTesting displayCategoryAnalysis():" << endl;
    adl.displayCategoryAnalysis();
    
    cout << "\nTesting displayIndependenceAssessment():" << endl;
    adl.displayIndependenceAssessment();
    
    return true;
}

int main() {
    cout << "🧪 Starting ActivitiesOfDailyLiving Tests..." << endl;
    cout << "================================================" << endl;
    
    // ActivityCategory tests
    RUN_TEST(test_ActivityCategoryConstructor);
    RUN_TEST(test_ActivityCategoryParameterizedConstructor);
    RUN_TEST(test_ActivityCategoryJsonSerialization);
    RUN_TEST(test_ActivityCategoryAnalysis);
    
    // ActivitiesOfDailyLiving basic tests
    RUN_TEST(test_ADLDefaultConstructor);
    RUN_TEST(test_ADLParameterizedConstructor);
    RUN_TEST(test_ADLInvalidCaseProfileId);
    RUN_TEST(test_ADLStaticMethods);
    
    // Data manipulation tests
    RUN_TEST(test_ADLActivityManipulation);
    RUN_TEST(test_ADLCategoryComments);
    RUN_TEST(test_ADLCategoryDataManipulation);
    
    // Analysis tests
    RUN_TEST(test_ADLAnalysisMethods);
    RUN_TEST(test_ADLClinicalAnalysis);
    RUN_TEST(test_ADLMostProblematicCategories);
    
    // Persistence and validation tests
    RUN_TEST(test_ADLJsonPersistence);
    RUN_TEST(test_ADLValidation);
    RUN_TEST(test_ADLClearAllData);
    
    // Display tests
    RUN_TEST(test_ADLDisplayMethods);
    
    cout << "\n🎉 All ActivitiesOfDailyLiving tests completed successfully!" << endl;
    cout << "================================================" << endl;
    
    return 0;
}
