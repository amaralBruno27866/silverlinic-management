#include "core/Utils.h"
#include <iostream>
#include <cassert>
#include <regex>

using namespace std;

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

// Test phone number formatting
bool test_phone_formatting() {
    // Test basic Canadian phone number validation
    TEST_ASSERT(utils::isValidCanadianPhoneNumber("(416) 555-1234"), "Standard Canadian phone should be valid");
    TEST_ASSERT(utils::isValidCanadianPhoneNumber("416-555-1234"), "Dashed Canadian phone should be valid");
    TEST_ASSERT(utils::isValidCanadianPhoneNumber("4165551234"), "Plain Canadian phone should be valid");
    
    // Test invalid phone numbers
    TEST_ASSERT(!utils::isValidCanadianPhoneNumber(""), "Empty phone should be invalid");
    TEST_ASSERT(!utils::isValidCanadianPhoneNumber("123"), "Too short phone should be invalid");
    TEST_ASSERT(!utils::isValidCanadianPhoneNumber("abc-def-ghij"), "Alphabetic phone should be invalid");
    
    return true;
}

// Test email validation
bool test_email_validation() {
    // Valid email addresses
    TEST_ASSERT(utils::isValidEmail("user@example.com"), "Simple email should be valid");
    TEST_ASSERT(utils::isValidEmail("first.last@domain.co.uk"), "Complex email should be valid");
    TEST_ASSERT(utils::isValidEmail("user+tag@example.org"), "Email with plus should be valid");
    TEST_ASSERT(utils::isValidEmail("test123@test-domain.net"), "Email with numbers and hyphens should be valid");
    
    // Invalid email addresses
    TEST_ASSERT(!utils::isValidEmail(""), "Empty string should be invalid");
    TEST_ASSERT(!utils::isValidEmail("invalid-email"), "Email without @ should be invalid");
    TEST_ASSERT(!utils::isValidEmail("@domain.com"), "Email without user part should be invalid");
    TEST_ASSERT(!utils::isValidEmail("user@"), "Email without domain should be invalid");
    TEST_ASSERT(!utils::isValidEmail("user@domain"), "Email without TLD should be invalid");
    
    return true;
}

// Test postal code validation
bool test_postal_code_validation() {
    // Valid Canadian postal codes
    TEST_ASSERT(utils::isValidCanadianPostalCode("M5V 3A8"), "Standard Canadian postal code should be valid");
    TEST_ASSERT(utils::isValidCanadianPostalCode("K1A 0A6"), "Government postal code should be valid");
    TEST_ASSERT(utils::isValidCanadianPostalCode("M5V3A8"), "Postal code without space should be valid");
    
    // Invalid postal codes
    TEST_ASSERT(!utils::isValidCanadianPostalCode(""), "Empty postal code should be invalid");
    TEST_ASSERT(!utils::isValidCanadianPostalCode("12345"), "US ZIP code should be invalid for Canadian validation");
    TEST_ASSERT(!utils::isValidCanadianPostalCode("M5V 3A"), "Incomplete postal code should be invalid");
    TEST_ASSERT(!utils::isValidCanadianPostalCode("O5V 3A8"), "Postal code with O should be invalid");
    
    return true;
}

// Test postal code formatting - removed since formatPostalCode doesn't exist
// Test string trimming
bool test_string_trimming() {
    // Test leading whitespace
    string trimmed1 = utils::trim("   hello");
    TEST_ASSERT(trimmed1 == "hello", "Leading whitespace should be removed");
    
    // Test trailing whitespace
    string trimmed2 = utils::trim("hello   ");
    TEST_ASSERT(trimmed2 == "hello", "Trailing whitespace should be removed");
    
    // Test both leading and trailing
    string trimmed3 = utils::trim("   hello   ");
    TEST_ASSERT(trimmed3 == "hello", "Both leading and trailing whitespace should be removed");
    
    // Test with tabs and newlines
    string trimmed4 = utils::trim("\t\nhello\n\t");
    TEST_ASSERT(trimmed4 == "hello", "Tabs and newlines should be removed");
    
    // Test empty string
    string trimmed5 = utils::trim("");
    TEST_ASSERT(trimmed5 == "", "Empty string should remain empty");
    
    // Test whitespace-only string
    string trimmed6 = utils::trim("   ");
    TEST_ASSERT(trimmed6 == "", "Whitespace-only string should become empty");
    
    return true;
}

// Test timestamp generation  
bool test_timestamp_generation() {
    string timestamp1 = utils::getCurrentTimestamp();
    TEST_ASSERT(!timestamp1.empty(), "Timestamp should not be empty");
    
    // Check basic format (should contain date and time elements)
    TEST_ASSERT(timestamp1.length() >= 19, "Timestamp should be at least 19 characters (YYYY-MM-DD HH:MM:SS)");
    
    // Generate another timestamp and verify they're different (unless executed in same second)
    string timestamp2 = utils::getCurrentTimestamp();
    TEST_ASSERT(!timestamp2.empty(), "Second timestamp should not be empty");
    
    // Test that timestamps are consistently formatted
    regex timestamp_pattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})");
    TEST_ASSERT(regex_match(timestamp1, timestamp_pattern), "First timestamp should match expected format");
    TEST_ASSERT(regex_match(timestamp2, timestamp_pattern), "Second timestamp should match expected format");
    
    return true;
}

// Test string case conversion
bool test_case_conversion() {
    // Test uppercase conversion
    string upper1 = utils::toUpper("hello world");
    TEST_ASSERT(upper1 == "HELLO WORLD", "Lowercase should convert to uppercase");
    
    string upper2 = utils::toUpper("Hello World");
    TEST_ASSERT(upper2 == "HELLO WORLD", "Mixed case should convert to uppercase");
    
    string upper3 = utils::toUpper("HELLO WORLD");
    TEST_ASSERT(upper3 == "HELLO WORLD", "Already uppercase should remain unchanged");
    
    return true;
}

// Test specialized validation functions
bool test_specialized_validation() {
    // Test SIN validation (usando um SIN que passa na validação Luhn)
    TEST_ASSERT(utils::isValidSIN("046454286"), "Valid SIN format should pass");
    TEST_ASSERT(!utils::isValidSIN(""), "Empty SIN should be invalid");
    TEST_ASSERT(!utils::isValidSIN("123"), "Too short SIN should be invalid");
    
    // Test Health Card validation
    TEST_ASSERT(utils::isValidHealthCard("1234567890"), "Valid health card should pass");
    TEST_ASSERT(!utils::isValidHealthCard(""), "Empty health card should be invalid");
    TEST_ASSERT(!utils::isValidHealthCard("abc"), "Non-numeric health card should be invalid");
    
    return true;
}

int main() {
    cout << "🚀 Utils Unit Tests" << endl;
    cout << "===================" << endl;
    
    // Run all tests
    RUN_TEST(test_phone_formatting);
    RUN_TEST(test_email_validation);
    RUN_TEST(test_postal_code_validation);
    RUN_TEST(test_string_trimming);
    RUN_TEST(test_timestamp_generation);
    RUN_TEST(test_case_conversion);
    RUN_TEST(test_specialized_validation);
    
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
