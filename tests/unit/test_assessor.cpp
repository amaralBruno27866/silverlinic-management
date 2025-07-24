#include "core/Assessor.h"
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
    Assessor assessor;
    
    TEST_ASSERT(assessor.getAssessorId() > 0, "Default constructor should generate valid ID");
    TEST_ASSERT(assessor.getAssessorId() >= Assessor::ID_PREFIX, "ID should have correct prefix");
    TEST_ASSERT(assessor.getFirstName().empty(), "Default first name should be empty");
    TEST_ASSERT(assessor.getLastName().empty(), "Default last name should be empty");
    TEST_ASSERT(assessor.getEmail().empty(), "Default email should be empty");
    TEST_ASSERT(assessor.getPhone().empty(), "Default phone should be empty");
    
    return true;
}

// Test parameterized constructor
bool test_parameterized_constructor() {
    Address addr(1, 1, "123 Main St", "Toronto", "ON", "M5V 3A8");
    Assessor assessor(12345, "John", "Doe", "john.doe@clinic.com", "(416) 555-0123", 
                     addr, DateTime::now(), DateTime::now());
    
    TEST_ASSERT(assessor.getAssessorId() == 12345, "Assessor ID should match constructor parameter");
    TEST_ASSERT(assessor.getFirstName() == "JOHN", "First name should match constructor parameter (normalized)");
    TEST_ASSERT(assessor.getLastName() == "DOE", "Last name should match constructor parameter (normalized)");
    TEST_ASSERT(assessor.getEmail() == "JOHN.DOE@CLINIC.COM", "Email should match constructor parameter (normalized)");
    TEST_ASSERT(assessor.getPhone() == "4165550123", "Phone should match constructor parameter (normalized)");
    
    return true;
}

// Test ID generation
bool test_id_generation() {
    int initial_id = Assessor::getNextAssessorId();
    TEST_ASSERT(initial_id >= Assessor::ID_PREFIX, "Next ID should have correct prefix");
    
    Assessor assessor1;
    Assessor assessor2;
    
    TEST_ASSERT(assessor1.getAssessorId() >= Assessor::ID_PREFIX, "First assessor should have correct prefix");
    TEST_ASSERT(assessor2.getAssessorId() >= Assessor::ID_PREFIX, "Second assessor should have correct prefix");
    TEST_ASSERT(assessor2.getAssessorId() > assessor1.getAssessorId(), "IDs should be incremental");
    
    return true;
}

// Test setters
bool test_setters() {
    Assessor assessor;
    
    assessor.setFirstName("Jane");
    TEST_ASSERT(assessor.getFirstName() == "JANE", "First name setter should work (normalized)");
    
    assessor.setLastName("Smith");
    TEST_ASSERT(assessor.getLastName() == "SMITH", "Last name setter should work (normalized)");
    
    assessor.setEmail("jane.smith@clinic.com");
    TEST_ASSERT(assessor.getEmail() == "JANE.SMITH@CLINIC.COM", "Email setter should work (normalized)");
    
    assessor.setPhone("(647) 555-9876");
    TEST_ASSERT(assessor.getPhone() == "6475559876", "Phone setter should work (normalized)");
    
    return true;
}

// Test full name functionality
bool test_full_name() {
    Assessor assessor;
    
    assessor.setFirstName("Maria");
    assessor.setLastName("Rodriguez");
    
    string full_name = assessor.getFullName();
    TEST_ASSERT(full_name == "MARIA RODRIGUEZ", "Full name should combine first and last name (normalized)");
    
    // Test with empty names
    Assessor empty_assessor;
    string empty_full_name = empty_assessor.getFullName();
    TEST_ASSERT(empty_full_name == " ", "Empty names should produce single space");
    
    // Test with only first name
    Assessor first_only;
    first_only.setFirstName("John");
    string first_only_name = first_only.getFullName();
    TEST_ASSERT(first_only_name == "JOHN ", "Only first name should have trailing space (normalized)");
    
    // Test with only last name
    Assessor last_only;
    last_only.setLastName("Doe");
    string last_only_name = last_only.getFullName();
    TEST_ASSERT(last_only_name == " DOE", "Only last name should have leading space (normalized)");
    
    return true;
}

// Test address integration
bool test_address_integration() {
    Address addr(1, 1, "456 Oak Ave", "Vancouver", "BC", "V6B 1A1");
    Assessor assessor;
    
    assessor.setAddress(addr);
    Address retrieved_addr = assessor.getAddress();
    
    TEST_ASSERT(retrieved_addr.getStreet() == "456 OAK AVE", "Address street should be preserved (normalized)");
    TEST_ASSERT(retrieved_addr.getCity() == "VANCOUVER", "Address city should be preserved (normalized)");
    TEST_ASSERT(retrieved_addr.getProvince() == "BC", "Address province should be preserved (normalized)");
    TEST_ASSERT(retrieved_addr.getPostalCode() == "V6B1A1", "Address postal code should be preserved (normalized)");
    
    return true;
}

// Test display functionality
bool test_display_methods() {
    Address addr(1, 1, "789 Test St", "Ottawa", "ON", "K1A 0A6");
    Assessor assessor(12345, "Dr. Sarah", "Johnson", "sarah.johnson@clinic.com", 
                     "(613) 555-0147", addr, DateTime::now(), DateTime::now());
    
    // Test that display methods don't crash
    cout << "   Testing displayInfo:" << endl;
    assessor.displayInfo();
    
    // Test basic display information is accessible
    string full_name = assessor.getFullName();
    TEST_ASSERT(!full_name.empty(), "Full name should not be empty for complete assessor");
    TEST_ASSERT(full_name.find("SARAH") != string::npos, "Full name should contain first name (normalized)");
    TEST_ASSERT(full_name.find("JOHNSON") != string::npos, "Full name should contain last name (normalized)");
    
    return true;
}

// Test email validation scenarios
bool test_email_scenarios() {
    Assessor assessor;
    
    // Test various email formats
    assessor.setEmail("simple@example.com");
    TEST_ASSERT(assessor.getEmail() == "SIMPLE@EXAMPLE.COM", "Simple email should be stored (normalized)");
    
    assessor.setEmail("first.last@domain.co.uk");
    TEST_ASSERT(assessor.getEmail() == "FIRST.LAST@DOMAIN.CO.UK", "Complex email should be stored (normalized)");
    
    assessor.setEmail("user+tag@example.org");
    TEST_ASSERT(assessor.getEmail() == "USER+TAG@EXAMPLE.ORG", "Email with plus should be stored (normalized)");
    
    assessor.setEmail("");
    TEST_ASSERT(assessor.getEmail().empty(), "Empty email should be allowed");
    
    return true;
}

// Test phone number scenarios
bool test_phone_scenarios() {
    Assessor assessor;
    
    // Test various phone formats
    assessor.setPhone("(416) 555-0123");
    TEST_ASSERT(assessor.getPhone() == "4165550123", "Formatted phone should be normalized to digits only");
    
    assessor.setPhone("416-555-0123");
    TEST_ASSERT(assessor.getPhone() == "4165550123", "Dashed phone should be normalized to digits only");
    
    assessor.setPhone("4165550123");
    TEST_ASSERT(assessor.getPhone() == "4165550123", "Plain phone should remain unchanged");
    
    assessor.setPhone("1-800-555-1234");
    TEST_ASSERT(assessor.getPhone() == "18005551234", "Long distance phone should be normalized to digits only");
    
    return true;
}

// Test data normalization scenarios
bool test_normalization_scenarios() {
    Assessor assessor;
    
    // Test name normalization
    assessor.setFirstName("  john  ");
    TEST_ASSERT(assessor.getFirstName() == "JOHN", "Name should be trimmed and uppercase");
    
    assessor.setLastName("  smith  ");
    TEST_ASSERT(assessor.getLastName() == "SMITH", "Last name should be trimmed and uppercase");
    
    // Test email normalization
    assessor.setEmail("  John.Smith@Email.COM  ");
    TEST_ASSERT(assessor.getEmail() == "JOHN.SMITH@EMAIL.COM", "Email should be trimmed and uppercase");
    
    // Test phone normalization
    assessor.setPhone("(613) 555-1234");
    TEST_ASSERT(assessor.getPhone() == "6135551234", "Phone should be digits only");
    
    return true;
}

// Test timestamps
bool test_timestamps() {
    Assessor assessor;
    
    // Check that timestamps are set
    string created = assessor.getCreatedAt().toString();
    string updated = assessor.getUpdatedAt().toString();
    
    TEST_ASSERT(!created.empty(), "Created timestamp should not be empty");
    TEST_ASSERT(!updated.empty(), "Updated timestamp should not be empty");
    TEST_ASSERT(created == updated, "Initially, created and updated should be the same");
    
    // Modify the assessor and check timestamps
    string original_updated = assessor.getUpdatedAt().toString();
    assessor.setFirstName("Modified Name");
    string new_updated = assessor.getUpdatedAt().toString();
    
    TEST_ASSERT(!new_updated.empty(), "Updated timestamp should still be valid after modification");
    
    return true;
}

int main() {
    cout << "🚀 Assessor Unit Tests" << endl;
    cout << "======================" << endl;
    
    // Run all tests
    RUN_TEST(test_default_constructor);
    RUN_TEST(test_parameterized_constructor);
    RUN_TEST(test_id_generation);
    RUN_TEST(test_setters);
    RUN_TEST(test_full_name);
    RUN_TEST(test_address_integration);
    RUN_TEST(test_display_methods);
    RUN_TEST(test_email_scenarios);
    RUN_TEST(test_phone_scenarios);
    RUN_TEST(test_normalization_scenarios);
    RUN_TEST(test_timestamps);
    
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
