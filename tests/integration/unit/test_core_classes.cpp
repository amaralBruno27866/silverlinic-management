#include "core/Address.h"
#include "core/DateTime.h"
#include "core/Assessor.h"
#include "core/Client.h"
#include "core/CaseProfile.h"
#include "core/Utils.h"
#include <iostream>
#include <cassert>

using namespace std;
using namespace SilverClinic;
using namespace utils;

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

bool test_datetime() {
    DateTime now = DateTime::now();
    TEST_ASSERT(!now.toString().empty(), "DateTime should generate valid string");
    
    DateTime birth(1990, 5, 15, 14, 30, 45);
    TEST_ASSERT(birth.getAgeInYears() > 30, "Age calculation should work");
    
    return true;
}

bool test_address() {
    Address addr(1, 100, "123 Main Street", "Toronto", "ON", "M5V 3A8");
    TEST_ASSERT(addr.getAddressId() == 1, "Address ID should match");
    TEST_ASSERT(addr.getUserKey() == 100, "User key should match");
    TEST_ASSERT(addr.getPostalCode() == "M5V3A8", "Postal code should match (normalized)");
    
    return true;
}

bool test_assessor() {
    Address addr(Address::getNextAddressId(), Assessor::ID_PREFIX + 1, "456 Healthcare St", "Toronto", "ON", "M5H 2N2");
    Assessor assessor(Assessor::getNextAssessorId(), "Dr. Maria", "Silva", "maria@email.com", "416-555-0123", 
                     addr, DateTime::now(), DateTime::now());
    
    TEST_ASSERT(assessor.getAssessorId() >= Assessor::ID_PREFIX, "Assessor ID should have correct prefix");
    TEST_ASSERT(assessor.getFullName() == "DR. MARIA SILVA", "Full name should be correct (normalized)");
    TEST_ASSERT(assessor.isValidEmail(), "Email should be valid");
    
    return true;
}

bool test_client() {
    Address addr(Address::getNextAddressId(), Client::ID_PREFIX + 1, "123 Main Street", "Toronto", "ON", "M5V 3A8");
    Client client(Client::getNextClientId(), "John", "Santos", "joao@email.com", "416-555-9876", 
                 "1985-03-15", addr, DateTime::now(), DateTime::now());
    
    TEST_ASSERT(client.getClientId() >= Client::ID_PREFIX, "Client ID should have correct prefix");
    TEST_ASSERT(client.getFullName() == "JOHN SANTOS", "Full name should be correct (normalized)");
    TEST_ASSERT(client.getAge() > 30, "Age should be calculated");
    
    return true;
}

bool test_case_profile() {
    CaseProfile cp(300001, 100001, "Test case profile");
    
    TEST_ASSERT(cp.getCaseProfileId() >= CaseProfile::ID_PREFIX, "CaseProfile ID should have correct prefix");
    TEST_ASSERT(cp.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(cp.getAssessorId() == 100001, "Assessor ID should match");
    {
        auto s = cp.getStatus();
        TEST_ASSERT(s == "Pending", "Initial status should be Pending (normalized)");
    }
    TEST_ASSERT(cp.isPending(), "Should be pending");
    
    cp.activate();
    TEST_ASSERT(cp.isActive(), "Should be active after activation");
    
    return true;
}

bool test_utilities() {
    string formatted_phone = formatCanadianPhoneNumber("4165550123");
    TEST_ASSERT(formatted_phone == "(416) 555-0123", "Phone formatting should work");
    
    string random_id = generateRandomId("PAT_", 6);
    TEST_ASSERT(random_id.length() == 10, "Random ID should have correct length"); // PAT_ + 6 chars
    TEST_ASSERT(random_id.substr(0, 4) == "PAT_", "Random ID should have correct prefix");
    
    string timestamp = getCurrentTimestamp();
    TEST_ASSERT(!timestamp.empty(), "Timestamp should not be empty");
    
    TEST_ASSERT(isBusinessHours(10, 0), "10:00 should be business hours");
    TEST_ASSERT(!isBusinessHours(22, 0), "22:00 should not be business hours");
    
    return true;
}

bool test_id_systems() {
    int assessor_id1 = Assessor::getNextAssessorId();
    int assessor_id2 = Assessor::getNextAssessorId();
    TEST_ASSERT(assessor_id2 > assessor_id1, "Assessor IDs should increment");
    TEST_ASSERT(assessor_id1 >= Assessor::ID_PREFIX, "Assessor ID should have correct prefix");
    
    int client_id1 = Client::getNextClientId();
    int client_id2 = Client::getNextClientId();
    TEST_ASSERT(client_id2 > client_id1, "Client IDs should increment");
    TEST_ASSERT(client_id1 >= Client::ID_PREFIX, "Client ID should have correct prefix");
    
    int case_id1 = CaseProfile::getNextCaseProfileId();
    int case_id2 = CaseProfile::getNextCaseProfileId();
    TEST_ASSERT(case_id2 > case_id1, "CaseProfile IDs should increment");
    TEST_ASSERT(case_id1 >= CaseProfile::ID_PREFIX, "CaseProfile ID should have correct prefix");
    
    int address_id1 = Address::getNextAddressId();
    int address_id2 = Address::getNextAddressId();
    TEST_ASSERT(address_id2 > address_id1, "Address IDs should increment");
    TEST_ASSERT(address_id1 >= Address::ID_PREFIX, "Address ID should have correct prefix");
    
    return true;
}

int main() {
    cout << "🚀 Core Classes Unit Tests" << endl;
    cout << "===========================" << endl;
    
    // Run all tests
    RUN_TEST(test_datetime);
    RUN_TEST(test_address);
    RUN_TEST(test_assessor);
    RUN_TEST(test_client);
    RUN_TEST(test_case_profile);
    RUN_TEST(test_utilities);
    RUN_TEST(test_id_systems);
    
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
