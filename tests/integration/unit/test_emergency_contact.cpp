#include "forms/EmergencyContact.h"
#include "core/DateTime.h"
#include <iostream>
#include <cassert>
#include <sstream>

using namespace std;
using namespace SilverClinic;
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
    EmergencyContact ec;
    
    TEST_ASSERT(ec.getEmergencyContactId() >= 600000, "ID should start with 600000+");
    TEST_ASSERT(ec.getClientId() == 0, "Default client ID should be 0");
    TEST_ASSERT(ec.getType() == "ECT", "Form type should be ECT");
    TEST_ASSERT(ec.getEcFullName() == "", "Default name should be empty");
    TEST_ASSERT(ec.getEcRelationship() == "", "Default relationship should be empty");
    TEST_ASSERT(ec.getEcPhone() == "", "Default phone should be empty");
    TEST_ASSERT(ec.getEcAltPhone() == "", "Default alt phone should be empty");
    
    return true;
}

// Test parameterized constructor
bool test_parameterized_constructor() {
    EmergencyContact ec(300001, "Maria Silva", "Mother", "(416) 555-0123", "(647) 555-9876");
    
    TEST_ASSERT(ec.getEmergencyContactId() >= 600000, "ID should start with 600000+");
    TEST_ASSERT(ec.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(ec.getType() == "ECT", "Form type should be ECT");
    TEST_ASSERT(ec.getEcFullName() == "MARIA SILVA", "Name should match (normalized)");
    TEST_ASSERT(ec.getEcRelationship() == "MOTHER", "Relationship should match (normalized)");
    TEST_ASSERT(ec.getEcPhone() == "4165550123", "Phone should match (normalized to digits only)");
    TEST_ASSERT(ec.getEcAltPhone() == "6475559876", "Alt phone should match (normalized to digits only)");
    
    return true;
}

// Test full constructor
bool test_full_constructor() {
    DateTime created = DateTime::now();
    DateTime updated = DateTime::now();
    
    EmergencyContact ec(600001, 300001, "John Smith", "Father", 
                       "(416) 555-9876", "(647) 555-0123", 
                       created, updated);
    
    TEST_ASSERT(ec.getEmergencyContactId() == 600001, "ID should match");
    TEST_ASSERT(ec.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(ec.getType() == "ECT", "Form type should be ECT");
    TEST_ASSERT(ec.getEcFullName() == "JOHN SMITH", "Name should match (normalized)");
    TEST_ASSERT(ec.getEcRelationship() == "FATHER", "Relationship should match (normalized)");
    TEST_ASSERT(ec.getEcPhone() == "4165559876", "Phone should match (normalized to digits only)");
    TEST_ASSERT(ec.getEcAltPhone() == "6475550123", "Alt phone should match (normalized to digits only)");
    
    return true;
}

// Test validation methods
bool test_validation() {
    EmergencyContact ec;
    
    // Test client ID validation
    TEST_ASSERT(ec.isValidClientId(300001), "Valid client ID should pass");
    TEST_ASSERT(!ec.isValidClientId(-1), "Negative client ID should fail");
    TEST_ASSERT(!ec.isValidClientId(0), "Zero client ID should fail");
    
    // Test name validation
    TEST_ASSERT(ec.isValidEcFullName("John Smith"), "Valid name should pass");
    TEST_ASSERT(!ec.isValidEcFullName(""), "Empty name should fail");
    
    // Test relationship validation
    TEST_ASSERT(ec.isValidEcRelationship("Father"), "Valid relationship should pass");
    TEST_ASSERT(!ec.isValidEcRelationship(""), "Empty relationship should fail");
    
    // Create a name that's too long (over 100 chars)
    string longName(101, 'A');
    TEST_ASSERT(!ec.isValidEcFullName(longName), "Long name should fail");
    
    // Create a relationship that's too long (over 50 chars)
    string longRelationship(51, 'B');
    TEST_ASSERT(!ec.isValidEcRelationship(longRelationship), "Long relationship should fail");
    
    return true;
}

// Test setters
bool test_setters() {
    EmergencyContact ec;
    
    // Test setting valid values
    ec.setClientId(300001);
    TEST_ASSERT(ec.getClientId() == 300001, "Client ID should be set");
    
    ec.setEcFullName("Test Contact");
    TEST_ASSERT(ec.getEcFullName() == "TEST CONTACT", "Name should be set (normalized)");
    
    ec.setEcRelationship("Sister");
    TEST_ASSERT(ec.getEcRelationship() == "SISTER", "Relationship should be set (normalized)");
    
    ec.setEcPhone("(416) 555-0123");
    TEST_ASSERT(ec.getEcPhone() == "4165550123", "Phone should be set (normalized to digits only)");
    
    ec.setEcAltPhone("(647) 555-9876");
    TEST_ASSERT(ec.getEcAltPhone() == "6475559876", "Alt phone should be set (normalized to digits only)");
    
    return true;
}

// Test stream operators
bool test_stream_operators() {
    EmergencyContact ec(300001, "Stream Test", "Brother", "(416) 555-7890", "");
    
    // Test output operator
    ostringstream oss;
    oss << ec;
    string output = oss.str();
    
    TEST_ASSERT(output.find("Emergency Contact ID:") != string::npos, "Output should contain class identifier");
    TEST_ASSERT(output.find("ECT") != string::npos, "Output should contain form type");
    TEST_ASSERT(output.find("STREAM TEST") != string::npos, "Output should contain name (normalized)");
    TEST_ASSERT(output.find("BROTHER") != string::npos, "Output should contain relationship (normalized)");
    
    // Test toString method
    string str = ec.toString();
    TEST_ASSERT(str.find("EmergencyContact[") != string::npos, "toString should contain class identifier");
    TEST_ASSERT(str.find("ECT") != string::npos, "toString should contain form type");
    
    return true;
}

// Test ID generation
bool test_id_generation() {
    int initial_id = EmergencyContact::getNextId();
    
    EmergencyContact ec1;
    EmergencyContact ec2;
    
    TEST_ASSERT(ec1.getEmergencyContactId() >= initial_id, "First ID should be greater than or equal to initial");
    TEST_ASSERT(ec2.getEmergencyContactId() > ec1.getEmergencyContactId(), "Second ID should be greater than first");
    TEST_ASSERT(ec1.getEmergencyContactId() >= 600000, "ID should have correct prefix");
    TEST_ASSERT(ec2.getEmergencyContactId() >= 600000, "ID should have correct prefix");
    
    return true;
}

// Test display methods
bool test_display_methods() {
    EmergencyContact ec(300001, "Display Test", "Uncle", "(416) 555-4444", "(647) 555-5555");
    
    // Test toString
    string str = ec.toString();
    TEST_ASSERT(str.find("EmergencyContact") != string::npos, "toString should contain class name");
    TEST_ASSERT(str.find("DISPLAY TEST") != string::npos, "toString should contain name (normalized)");
    TEST_ASSERT(str.find("ECT") != string::npos, "toString should contain type");
    
    // Test displaySummary and displayInfo (just check they don't crash)
    cout << "   Testing displaySummary:" << endl;
    ec.displaySummary();
    
    cout << "   Testing displayInfo:" << endl;
    ec.displayInfo();
    
    return true;
}

// Test form type consistency
bool test_form_type() {
    EmergencyContact ec1;
    EmergencyContact ec2(300001, "Test", "Friend", "1234567890");
    EmergencyContact ec3(600001, 300001, "Full", "Cousin", "phone", "alt", DateTime::now(), DateTime::now());
    
    TEST_ASSERT(ec1.getType() == "ECT", "Default constructor should set type to ECT");
    TEST_ASSERT(ec2.getType() == "ECT", "Parameterized constructor should set type to ECT");
    TEST_ASSERT(ec3.getType() == "ECT", "Full constructor should set type to ECT");
    
    return true;
}

int main() {
    cout << "🚀 EmergencyContact Unit Tests" << endl;
    cout << "==============================" << endl;
    
    // Run all tests
    RUN_TEST(test_default_constructor);
    RUN_TEST(test_parameterized_constructor);
    RUN_TEST(test_full_constructor);
    RUN_TEST(test_validation);
    RUN_TEST(test_setters);
    RUN_TEST(test_stream_operators);
    RUN_TEST(test_id_generation);
    RUN_TEST(test_display_methods);
    RUN_TEST(test_form_type);
    
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
