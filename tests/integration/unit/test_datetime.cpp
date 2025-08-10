#include "core/DateTime.h"
#include <iostream>
#include <cassert>

using namespace std;
using namespace SilverClinic;

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
        passed_tests++; \
    } else { \
        cout << "❌ " << #test_function << " failed" << endl; \
        failed_tests++; \
    } \
    total_tests++;

// Global test counters
int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

// Test default constructor
bool test_default_constructor() {
    DateTime dt;
    TEST_ASSERT(!dt.toString().empty(), "Default constructor should create valid DateTime");
    TEST_ASSERT(dt.toString().length() > 15, "DateTime string should have reasonable length");
    return true;
}

// Test parameterized constructor
bool test_parameterized_constructor() {
    DateTime dt(2025, 7, 21, 14, 30, 45);
    string str = dt.toString();
    
    TEST_ASSERT(str.find("2025") != string::npos, "Should contain year 2025");
    TEST_ASSERT(str.find("07") != string::npos, "Should contain month 07");
    TEST_ASSERT(str.find("21") != string::npos, "Should contain day 21");
    TEST_ASSERT(str.find("30") != string::npos, "Should contain minute 30");
    TEST_ASSERT(str.find("45") != string::npos, "Should contain second 45");
    
    return true;
}

// Test now() static method
bool test_now_method() {
    DateTime dt1 = DateTime::now();
    DateTime dt2 = DateTime::now();
    
    TEST_ASSERT(!dt1.toString().empty(), "now() should create valid DateTime");
    TEST_ASSERT(!dt2.toString().empty(), "Second now() should create valid DateTime");
    TEST_ASSERT(dt1.toString().length() == dt2.toString().length(), "Both now() results should have same format");
    
    return true;
}

// Test age calculation
bool test_age_calculation() {
    // Birth date: January 1, 1990
    DateTime birth(1990, 1, 1, 0, 0, 0);
    int age = birth.getAgeInYears();
    
    TEST_ASSERT(age >= 35, "Age should be at least 35 years (born 1990)");
    TEST_ASSERT(age <= 40, "Age should be reasonable (not more than 40)");
    
    // Test with more recent birth date
    DateTime recent_birth(2020, 6, 15, 12, 0, 0);
    int recent_age = recent_birth.getAgeInYears();
    
    TEST_ASSERT(recent_age >= 4, "Recent birth should be at least 4 years old");
    TEST_ASSERT(recent_age <= 6, "Recent birth should not be more than 6 years old");
    
    return true;
}

// Test edge cases
bool test_edge_cases() {
    // Test leap year
    DateTime leap_day(2024, 2, 29, 12, 0, 0);
    string leap_str = leap_day.toString();
    TEST_ASSERT(leap_str.find("2024") != string::npos, "Should handle leap year");
    TEST_ASSERT(leap_str.find("02") != string::npos, "Should handle February");
    TEST_ASSERT(leap_str.find("29") != string::npos, "Should handle 29th day");
    
    // Test end of year
    DateTime end_year(2025, 12, 31, 23, 59, 59);
    string end_str = end_year.toString();
    TEST_ASSERT(end_str.find("2025") != string::npos, "Should handle end of year");
    TEST_ASSERT(end_str.find("12") != string::npos, "Should handle December");
    TEST_ASSERT(end_str.find("31") != string::npos, "Should handle 31st day");
    TEST_ASSERT(end_str.find("23") != string::npos, "Should handle 23rd hour");
    TEST_ASSERT(end_str.find("59") != string::npos, "Should handle 59th minute/second");
    
    return true;
}

// Test string format
bool test_string_format() {
    DateTime dt(2025, 7, 21, 9, 5, 3);
    string str = dt.toString();
    
    // Should be in format: YYYY-MM-DD HH:MM:SS
    TEST_ASSERT(str.length() == 19, "String should be exactly 19 characters (YYYY-MM-DD HH:MM:SS)");
    TEST_ASSERT(str[4] == '-', "Character at position 4 should be hyphen");
    TEST_ASSERT(str[7] == '-', "Character at position 7 should be hyphen");
    TEST_ASSERT(str[10] == ' ', "Character at position 10 should be space");
    TEST_ASSERT(str[13] == ':', "Character at position 13 should be colon");
    TEST_ASSERT(str[16] == ':', "Character at position 16 should be colon");
    
    return true;
}

// Test comparison and consistency
bool test_consistency() {
    DateTime dt1(2025, 7, 21, 14, 30, 45);
    DateTime dt2(2025, 7, 21, 14, 30, 45);
    
    TEST_ASSERT(dt1.toString() == dt2.toString(), "Same parameters should produce same string");
    
    DateTime dt3(2025, 7, 21, 14, 30, 46); // One second later
    TEST_ASSERT(dt1.toString() != dt3.toString(), "Different parameters should produce different strings");
    
    return true;
}

int main() {
    cout << "🚀 DateTime Unit Tests" << endl;
    cout << "======================" << endl;
    
    // Run all tests
    RUN_TEST(test_default_constructor);
    RUN_TEST(test_parameterized_constructor);
    RUN_TEST(test_now_method);
    RUN_TEST(test_age_calculation);
    RUN_TEST(test_edge_cases);
    RUN_TEST(test_string_format);
    RUN_TEST(test_consistency);
    
    // Print summary
    cout << "\n📊 Test Summary" << endl;
    cout << "===============" << endl;
    cout << "Total tests: " << total_tests << endl;
    cout << "Passed: " << passed_tests << " ✅" << endl;
    cout << "Failed: " << failed_tests << " ❌" << endl;
    cout << "Success rate: " << (total_tests > 0 ? (passed_tests * 100 / total_tests) : 0) << "%" << endl;
    
    if (failed_tests == 0) {
        cout << "\n🎉 All tests passed!" << endl;
        return 0;
    } else {
        cout << "\n⚠️  Some tests failed!" << endl;
        return 1;
    }
}
