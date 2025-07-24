#include "forms/InsuranceCompany.h"
#include "core/Utils.h"
#include <iostream>
#include <cassert>
#include <sstream>

using namespace std;
using namespace SilverClinic;
using namespace SilverClinic::Forms;

// Test assertion macro
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            cout << "❌ FAIL: " << message << endl; \
            return false; \
        } else { \
            cout << "✅ PASS: " << message << endl; \
        } \
    } while(0)

// Test default constructor
bool test_default_constructor() {
    InsuranceCompany ic;
    
    TEST_ASSERT(ic.getInsuranceCompanyId() >= 800000, "ID should start with 800000+");
    TEST_ASSERT(ic.getClientId() == 0, "Default client ID should be 0");
    TEST_ASSERT(ic.getType() == "ICO", "Form type should be ICO");
    TEST_ASSERT(ic.getIcCompanyName().empty(), "Default company name should be empty");
    TEST_ASSERT(ic.getIcAdjusterName().empty(), "Default adjuster name should be empty");
    TEST_ASSERT(ic.getIcPhone().empty(), "Default phone should be empty");
    TEST_ASSERT(ic.getIcEmail().empty(), "Default email should be empty");
    TEST_ASSERT(ic.getIcClaimNumber().empty(), "Default claim number should be empty");
    TEST_ASSERT(ic.getIcPolicyNumber().empty(), "Default policy number should be empty");
    TEST_ASSERT(!ic.getIcDateAccident().isValid(), "Default date accident should be invalid");
    
    return true;
}

// Test parameterized constructor
bool test_parameterized_constructor() {
    DateTime accident_date(2024, 1, 15, 0, 0, 0);
    InsuranceCompany ic(300001, "state farm insurance", "jane adjuster", 
                       "(416) 555-1234", "jane.adjuster@statefarm.com", 
                       "clm123456", "pol987654", accident_date);
    
    TEST_ASSERT(ic.getInsuranceCompanyId() >= 800000, "ID should start with 800000+");
    TEST_ASSERT(ic.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(ic.getType() == "ICO", "Form type should be ICO");
    TEST_ASSERT(ic.getIcCompanyName() == "STATE FARM INSURANCE", "Company name should match (normalized)");
    TEST_ASSERT(ic.getIcAdjusterName() == "JANE ADJUSTER", "Adjuster name should match (normalized)");
    TEST_ASSERT(ic.getIcPhone() == "4165551234", "Phone should match (normalized to digits only)");
    TEST_ASSERT(ic.getIcEmail() == "JANE.ADJUSTER@STATEFARM.COM", "Email should match (normalized)");
    TEST_ASSERT(ic.getIcClaimNumber() == "CLM123456", "Claim number should match (normalized)");
    TEST_ASSERT(ic.getIcPolicyNumber() == "POL987654", "Policy number should match (normalized)");
    TEST_ASSERT(ic.getIcDateAccident().toDateString() == "2024-01-15", "Date accident should match");
    
    return true;
}

// Test full constructor
bool test_full_constructor() {
    DateTime created = DateTime::now();
    DateTime updated = DateTime::now();
    DateTime accident_date(2024, 2, 20, 0, 0, 0);
    
    InsuranceCompany ic(800123, 300001, "allstate insurance", "bob claims", 
                       "647-555-9876", "bob@allstate.ca", "clm789012", 
                       "pol345678", accident_date, created, updated);
    
    TEST_ASSERT(ic.getInsuranceCompanyId() == 800123, "ID should match");
    TEST_ASSERT(ic.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(ic.getType() == "ICO", "Form type should be ICO");
    TEST_ASSERT(ic.getIcCompanyName() == "ALLSTATE INSURANCE", "Company name should match (normalized)");
    TEST_ASSERT(ic.getIcAdjusterName() == "BOB CLAIMS", "Adjuster name should match (normalized)");
    TEST_ASSERT(ic.getIcPhone() == "6475559876", "Phone should match (normalized to digits only)");
    TEST_ASSERT(ic.getIcEmail() == "BOB@ALLSTATE.CA", "Email should match (normalized)");
    TEST_ASSERT(ic.getIcClaimNumber() == "CLM789012", "Claim number should match (normalized)");
    TEST_ASSERT(ic.getIcPolicyNumber() == "POL345678", "Policy number should match (normalized)");
    TEST_ASSERT(ic.getIcDateAccident().toDateString() == "2024-02-20", "Date accident should match");
    
    return true;
}

// Test validation methods
bool test_validation() {
    InsuranceCompany ic;
    
    // Valid cases
    TEST_ASSERT(ic.isValidClientId(300001), "Valid client ID should pass");
    TEST_ASSERT(ic.isValidIcCompanyName("Insurance Corp"), "Valid company name should pass");
    TEST_ASSERT(ic.isValidIcAdjusterName("John Doe"), "Valid adjuster name should pass");
    TEST_ASSERT(ic.isValidIcPhone("(416) 555-1234"), "Valid phone should pass");
    TEST_ASSERT(ic.isValidIcEmail("test@example.com"), "Valid email should pass");
    TEST_ASSERT(ic.isValidIcClaimNumber("CLM12345"), "Valid claim number should pass");
    TEST_ASSERT(ic.isValidIcPolicyNumber("POL67890"), "Valid policy number should pass");
    TEST_ASSERT(ic.isValidIcDateAccident(DateTime(2024, 1, 15, 0, 0, 0)), "Valid date should pass");
    
    // Invalid cases
    TEST_ASSERT(!ic.isValidClientId(-1), "Negative client ID should fail");
    TEST_ASSERT(!ic.isValidClientId(0), "Zero client ID should fail");
    TEST_ASSERT(!ic.isValidIcCompanyName(""), "Empty company name should fail");
    TEST_ASSERT(!ic.isValidIcAdjusterName(""), "Empty adjuster name should fail");
    TEST_ASSERT(!ic.isValidIcPhone(""), "Empty phone should fail");
    TEST_ASSERT(!ic.isValidIcEmail("invalid-email"), "Invalid email should fail");
    TEST_ASSERT(!ic.isValidIcClaimNumber(""), "Empty claim number should fail");
    TEST_ASSERT(!ic.isValidIcPolicyNumber(""), "Empty policy number should fail");
    TEST_ASSERT(!ic.isValidIcDateAccident(DateTime()), "Invalid date should fail");
    TEST_ASSERT(!ic.isValidIcDateAccident(DateTime(2030, 1, 1, 0, 0, 0)), "Future date should fail");
    
    // Length validation
    string long_company(200, 'A');
    string long_adjuster(150, 'B');
    string long_claim(60, 'C');
    string long_policy(60, 'P');
    TEST_ASSERT(!ic.isValidIcCompanyName(long_company), "Long company name should fail");
    TEST_ASSERT(!ic.isValidIcAdjusterName(long_adjuster), "Long adjuster name should fail");
    TEST_ASSERT(!ic.isValidIcClaimNumber(long_claim), "Long claim number should fail");
    TEST_ASSERT(!ic.isValidIcPolicyNumber(long_policy), "Long policy number should fail");
    
    return true;
}

// Test setters
bool test_setters() {
    InsuranceCompany ic;
    
    ic.setClientId(300001);
    ic.setIcCompanyName("intact insurance");
    ic.setIcAdjusterName("sarah claims");
    ic.setIcPhone("(905) 555-7777");
    ic.setIcEmail("sarah@intact.com");
    ic.setIcClaimNumber("clm555666");
    ic.setIcPolicyNumber("pol777888");
    ic.setIcDateAccident(DateTime(2024, 3, 10, 0, 0, 0));
    
    TEST_ASSERT(ic.getClientId() == 300001, "Client ID should be set");
    TEST_ASSERT(ic.getIcCompanyName() == "INTACT INSURANCE", "Company name should be set (normalized)");
    TEST_ASSERT(ic.getIcAdjusterName() == "SARAH CLAIMS", "Adjuster name should be set (normalized)");
    TEST_ASSERT(ic.getIcPhone() == "9055557777", "Phone should be set (normalized to digits only)");
    TEST_ASSERT(ic.getIcEmail() == "SARAH@INTACT.COM", "Email should be set (normalized)");
    TEST_ASSERT(ic.getIcClaimNumber() == "CLM555666", "Claim number should be set (normalized)");
    TEST_ASSERT(ic.getIcPolicyNumber() == "POL777888", "Policy number should be set (normalized)");
    TEST_ASSERT(ic.getIcDateAccident().toDateString() == "2024-03-10", "Date accident should be set");
    
    return true;
}

// Test stream operators
bool test_stream_operators() {
    InsuranceCompany ic(300001, "td insurance", "mike handler", 
                       "416-555-8888", "mike@tdinsurance.com", 
                       "clm999000", "pol111222", DateTime(2024, 4, 5, 0, 0, 0));
    
    stringstream ss;
    ss << ic;
    string output = ss.str();
    
    TEST_ASSERT(output.find("InsuranceCompany") != string::npos, "Output should contain class identifier");
    TEST_ASSERT(output.find("ICO") != string::npos, "Output should contain form type");
    TEST_ASSERT(output.find("TD INSURANCE") != string::npos, "Output should contain company name (normalized)");
    TEST_ASSERT(output.find("MIKE HANDLER") != string::npos, "Output should contain adjuster name (normalized)");
    TEST_ASSERT(output.find("CLM999000") != string::npos, "Output should contain claim number (normalized)");
    
    // Test input operator
    stringstream input_stream;
    InsuranceCompany ic_input;
    input_stream >> ic_input;
    
    TEST_ASSERT(ic_input.getType() == "ICO", "Input operator should set type to ICO");
    TEST_ASSERT(!ic_input.toString().empty(), "toString should not be empty");
    
    return true;
}

// Test ID generation
bool test_id_generation() {
    int initial_id = InsuranceCompany::getNextId();
    
    InsuranceCompany ic1;
    InsuranceCompany ic2;
    
    TEST_ASSERT(ic1.getInsuranceCompanyId() >= initial_id, "First ID should be greater than or equal to initial");
    TEST_ASSERT(ic2.getInsuranceCompanyId() > ic1.getInsuranceCompanyId(), "Second ID should be greater than first");
    TEST_ASSERT(ic1.getInsuranceCompanyId() >= 800000, "ID should have correct prefix");
    TEST_ASSERT(ic2.getInsuranceCompanyId() >= 800000, "ID should have correct prefix");
    
    return true;
}

// Test display methods
bool test_display_methods() {
    InsuranceCompany ic(300001, "display insurance corp", "display adjuster", 
                       "416-555-4444", "display@test.com", "clm123test", 
                       "pol456test", DateTime(2024, 5, 15, 0, 0, 0));
    
    // Test toString
    string str = ic.toString();
    TEST_ASSERT(str.find("InsuranceCompany") != string::npos, "toString should contain class name");
    TEST_ASSERT(str.find("DISPLAY ADJUSTER") != string::npos, "toString should contain adjuster name (normalized)");
    TEST_ASSERT(str.find("ICO") != string::npos, "toString should contain type");
    
    // Test display methods (visual verification)
    cout << "   Testing displaySummary:" << endl;
    ic.displaySummary();
    cout << "   Testing displayInfo:" << endl;
    ic.displayInfo();
    
    return true;
}

// Test form type consistency
bool test_form_type() {
    InsuranceCompany ic1;
    InsuranceCompany ic2(300001, "test insurance", "test adjuster", "416-555-1111", 
                        "test@insurance.com", "clm111", "pol222", DateTime(2024, 1, 1, 0, 0, 0));
    InsuranceCompany ic3(800001, 300001, "test corp", "test handler", "416-555-2222", 
                        "test@corp.com", "clm333", "pol444", DateTime(2024, 2, 2, 0, 0, 0), 
                        DateTime::now(), DateTime::now());
    
    TEST_ASSERT(ic1.getType() == "ICO", "Default constructor should set type to ICO");
    TEST_ASSERT(ic2.getType() == "ICO", "Parameterized constructor should set type to ICO");
    TEST_ASSERT(ic3.getType() == "ICO", "Full constructor should set type to ICO");
    
    return true;
}

// Main test runner
int main() {
    cout << "🚀 InsuranceCompany Unit Tests" << endl;
    cout << "===============================" << endl;

    // Reset counter before tests
    InsuranceCompany::resetIdCounter();

    bool all_passed = true;
    int test_count = 0;
    int passed_count = 0;

    // Test functions
    struct TestCase {
        bool (*func)();
        string name;
    };

    TestCase tests[] = {
        {test_default_constructor, "test_default_constructor"},
        {test_parameterized_constructor, "test_parameterized_constructor"},
        {test_full_constructor, "test_full_constructor"},
        {test_validation, "test_validation"},
        {test_setters, "test_setters"},
        {test_stream_operators, "test_stream_operators"},
        {test_id_generation, "test_id_generation"},
        {test_display_methods, "test_display_methods"},
        {test_form_type, "test_form_type"}
    };

    for (const auto& test : tests) {
        cout << "\n🧪 Running " << test.name << "..." << endl;
        test_count++;
        
        if (test.func()) {
            cout << "✅ " << test.name << " completed successfully" << endl;
            passed_count++;
        } else {
            cout << "❌ " << test.name << " failed" << endl;
            all_passed = false;
        }
    }

    // Summary
    cout << "\n📊 Test Summary" << endl;
    cout << "===============" << endl;
    cout << "Total tests: " << test_count << endl;
    cout << "Passed: " << passed_count << " ✅" << endl;
    cout << "Failed: " << (test_count - passed_count) << " ❌" << endl;
    cout << "Success rate: " << (passed_count * 100 / test_count) << "%" << endl;

    if (all_passed) {
        cout << "\n🎉 All tests passed!" << endl;
        return 0;
    } else {
        cout << "\n⚠️ Some tests failed!" << endl;
        return 1;
    }
}
