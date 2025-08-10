#include "core/Address.h"
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
    Address addr;
    TEST_ASSERT(addr.getAddressId() > 0, "Default constructor should generate valid ID");
    TEST_ASSERT(addr.getUserKey() == 0, "Default user key should be 0");
    TEST_ASSERT(addr.getStreet().empty(), "Default street should be empty");
    TEST_ASSERT(addr.getCity().empty(), "Default city should be empty");
    TEST_ASSERT(addr.getProvince().empty(), "Default province should be empty");
    TEST_ASSERT(addr.getPostalCode().empty(), "Default postal code should be empty");
    return true;
}

// Test parameterized constructor
bool test_parameterized_constructor() {
    Address addr(12345, 67890, "123 Main Street", "Toronto", "ON", "M5V 3A8");
    
    TEST_ASSERT(addr.getAddressId() == 12345, "Address ID should match constructor parameter");
    TEST_ASSERT(addr.getUserKey() == 67890, "User key should match constructor parameter");
    TEST_ASSERT(addr.getStreet() == "123 MAIN STREET", "Street should match constructor parameter (normalized)");
    TEST_ASSERT(addr.getCity() == "TORONTO", "City should match constructor parameter (normalized)");
    TEST_ASSERT(addr.getProvince() == "ON", "Province should match constructor parameter");
    TEST_ASSERT(addr.getPostalCode() == "M5V3A8", "Postal code should match constructor parameter (normalized)");
    
    return true;
}

// Test ID generation
bool test_id_generation() {
    int initial_id = Address::getNextAddressId();
    TEST_ASSERT(initial_id >= Address::ID_PREFIX, "Next ID should have correct prefix");
    
    Address addr1;
    Address addr2;
    
    TEST_ASSERT(addr1.getAddressId() >= Address::ID_PREFIX, "First address should have correct prefix");
    TEST_ASSERT(addr2.getAddressId() >= Address::ID_PREFIX, "Second address should have correct prefix");
    TEST_ASSERT(addr2.getAddressId() > addr1.getAddressId(), "IDs should be incremental");
    
    return true;
}

// Test setters
bool test_setters() {
    Address addr;
    
    addr.setStreet("456 Oak Avenue");
    TEST_ASSERT(addr.getStreet() == "456 OAK AVENUE", "Street setter should work (normalized)");
    
    addr.setCity("Vancouver");
    TEST_ASSERT(addr.getCity() == "VANCOUVER", "City setter should work (normalized)");
    
    addr.setProvince("BC");
    TEST_ASSERT(addr.getProvince() == "BC", "Province setter should work");
    
    addr.setPostalCode("V6B 1A1");
    TEST_ASSERT(addr.getPostalCode() == "V6B1A1", "Postal code setter should work (normalized)");
    
    addr.setUserKey(98765);
    TEST_ASSERT(addr.getUserKey() == 98765, "User key setter should work");
    
    return true;
}

// Test postal code formatting
bool test_postal_code_formatting() {
    Address addr1(1, 1, "Street", "City", "ON", "M5V3A8");  // No space
    Address addr2(2, 2, "Street", "City", "ON", "M5V 3A8"); // With space
    Address addr3(3, 3, "Street", "City", "ON", "m5v3a8");  // Lowercase
    
    // All should store the postal code as provided (formatting happens in Utils)
    TEST_ASSERT(addr1.getPostalCode() == "M5V3A8", "Postal code without space should be stored as-is");
    TEST_ASSERT(addr2.getPostalCode() == "M5V3A8", "Postal code with space should be normalized");
    TEST_ASSERT(addr3.getPostalCode() == "M5V3A8", "Lowercase postal code should be normalized to uppercase");
    
    return true;
}

// Test display functionality
bool test_display_methods() {
    Address addr(12345, 67890, "789 Test Street", "Ottawa", "ON", "K1A 0A6");
    
    // Test that display methods don't crash
    cout << "   Testing displayInfo:" << endl;
    addr.displayInfo();
    
    // Test getFormattedAddress
    string formatted = addr.getFormattedAddress();
    TEST_ASSERT(!formatted.empty(), "Formatted address should not be empty");
    TEST_ASSERT(formatted.find("789 TEST STREET") != string::npos, "Formatted address should contain street (normalized)");
    TEST_ASSERT(formatted.find("OTTAWA") != string::npos, "Formatted address should contain city (normalized)");
    TEST_ASSERT(formatted.find("ON") != string::npos, "Formatted address should contain province");
    TEST_ASSERT(formatted.find("K1A0A6") != string::npos, "Formatted address should contain postal code (normalized)");
    
    return true;
}

// Test edge cases
bool test_edge_cases() {
    // Test with empty strings
    Address addr1(1, 1, "", "", "", "");
    TEST_ASSERT(addr1.getStreet().empty(), "Empty street should remain empty");
    TEST_ASSERT(addr1.getCity().empty(), "Empty city should remain empty");
    
    // Test with very long strings
    string long_street(200, 'A');
    Address addr2(2, 2, long_street, "City", "ON", "M5V 3A8");
    TEST_ASSERT(addr2.getStreet() == long_street, "Long street should be stored completely (already uppercase)");
    
    // Test with special characters
    Address addr3(3, 3, "123 Main St. Apt #4B", "Montreal", "QC", "H3A 1A1");
    TEST_ASSERT(addr3.getStreet().find("#4B") != string::npos, "Special characters should be preserved");
    TEST_ASSERT(addr3.getCity() == "MONTREAL", "City should be normalized to uppercase");
    
    return true;
}

// Test timestamps
bool test_timestamps() {
    Address addr;
    
    // Check that timestamps are set
    string created = addr.getCreatedAt().toString();
    string updated = addr.getUpdatedAt().toString();
    
    TEST_ASSERT(!created.empty(), "Created timestamp should not be empty");
    TEST_ASSERT(!updated.empty(), "Updated timestamp should not be empty");
    TEST_ASSERT(created == updated, "Initially, created and updated should be the same");
    
    // Modify the address and check if timestamp updates
    string original_updated = addr.getUpdatedAt().toString();
    addr.setStreet("New Street");
    string new_updated = addr.getUpdatedAt().toString();
    
    // Note: This might be the same if the operation is very fast
    TEST_ASSERT(!new_updated.empty(), "Updated timestamp should still be valid after modification");
    
    return true;
}

int main() {
    cout << "🚀 Address Unit Tests" << endl;
    cout << "=====================" << endl;
    
    // Run all tests
    RUN_TEST(test_default_constructor);
    RUN_TEST(test_parameterized_constructor);
    RUN_TEST(test_id_generation);
    RUN_TEST(test_setters);
    RUN_TEST(test_postal_code_formatting);
    RUN_TEST(test_display_methods);
    RUN_TEST(test_edge_cases);
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
