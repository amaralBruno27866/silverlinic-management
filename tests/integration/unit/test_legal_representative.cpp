#include "forms/LegalRepresentative.h"
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
    LegalRepresentative lr;
    
    TEST_ASSERT(lr.getLegalRepresentativeId() >= 700000, "ID should start with 700000+");
    TEST_ASSERT(lr.getClientId() == 0, "Default client ID should be 0");
    TEST_ASSERT(lr.getType() == "LRT", "Form type should be LRT");
    TEST_ASSERT(lr.getLrCompanyName().empty(), "Default company name should be empty");
    TEST_ASSERT(lr.getLrFullName().empty(), "Default full name should be empty");
    TEST_ASSERT(lr.getLrPhone().empty(), "Default phone should be empty");
    TEST_ASSERT(lr.getLrEmail().empty(), "Default email should be empty");
    
    return true;
}

// Test parameterized constructor
bool test_parameterized_constructor() {
    LegalRepresentative lr(300001, "smith & associates law firm", "john smith", 
                          "(416) 555-1234", "john.smith@lawfirm.com");
    
    TEST_ASSERT(lr.getLegalRepresentativeId() >= 700000, "ID should start with 700000+");
    TEST_ASSERT(lr.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(lr.getType() == "LRT", "Form type should be LRT");
    TEST_ASSERT(lr.getLrCompanyName() == "SMITH & ASSOCIATES LAW FIRM", "Company name should match (normalized)");
    TEST_ASSERT(lr.getLrFullName() == "JOHN SMITH", "Full name should match (normalized)");
    TEST_ASSERT(lr.getLrPhone() == "4165551234", "Phone should match (normalized to digits only)");
    TEST_ASSERT(lr.getLrEmail() == "JOHN.SMITH@LAWFIRM.COM", "Email should match (normalized)");
    
    return true;
}

// Test full constructor
bool test_full_constructor() {
    DateTime created = DateTime::now();
    DateTime updated = DateTime::now();
    
    LegalRepresentative lr(700123, 300001, "brown legal services", "jane brown", 
                          "647-555-9876", "jane@brownlegal.ca", created, updated);
    
    TEST_ASSERT(lr.getLegalRepresentativeId() == 700123, "ID should match");
    TEST_ASSERT(lr.getClientId() == 300001, "Client ID should match");
    TEST_ASSERT(lr.getType() == "LRT", "Form type should be LRT");
    TEST_ASSERT(lr.getLrCompanyName() == "BROWN LEGAL SERVICES", "Company name should match (normalized)");
    TEST_ASSERT(lr.getLrFullName() == "JANE BROWN", "Full name should match (normalized)");
    TEST_ASSERT(lr.getLrPhone() == "6475559876", "Phone should match (normalized to digits only)");
    TEST_ASSERT(lr.getLrEmail() == "JANE@BROWNLEGAL.CA", "Email should match (normalized)");
    
    return true;
}

// Test validation methods
bool test_validation() {
    LegalRepresentative lr;
    
    // Valid cases
    TEST_ASSERT(lr.isValidClientId(300001), "Valid client ID should pass");
    TEST_ASSERT(lr.isValidLrCompanyName("Legal Associates"), "Valid company name should pass");
    TEST_ASSERT(lr.isValidLrFullName("John Doe"), "Valid name should pass");
    TEST_ASSERT(lr.isValidLrPhone("(416) 555-1234"), "Valid phone should pass");
    TEST_ASSERT(lr.isValidLrEmail("test@example.com"), "Valid email should pass");
    
    // Invalid cases
    TEST_ASSERT(!lr.isValidClientId(-1), "Negative client ID should fail");
    TEST_ASSERT(!lr.isValidClientId(0), "Zero client ID should fail");
    TEST_ASSERT(!lr.isValidLrCompanyName(""), "Empty company name should fail");
    TEST_ASSERT(!lr.isValidLrFullName(""), "Empty name should fail");
    TEST_ASSERT(!lr.isValidLrPhone(""), "Empty phone should fail");
    TEST_ASSERT(!lr.isValidLrEmail("invalid-email"), "Invalid email should fail");
    
    // Length validation
    string long_company(200, 'A');
    string long_name(150, 'B');
    TEST_ASSERT(!lr.isValidLrCompanyName(long_company), "Long company name should fail");
    TEST_ASSERT(!lr.isValidLrFullName(long_name), "Long name should fail");
    
    return true;
}

// Test setters
bool test_setters() {
    LegalRepresentative lr;
    
    lr.setClientId(300001);
    lr.setLrCompanyName("abc law group");
    lr.setLrFullName("sarah johnson");
    lr.setLrPhone("(905) 555-7777");
    lr.setLrEmail("sarah@abclaw.com");
    
    TEST_ASSERT(lr.getClientId() == 300001, "Client ID should be set");
    TEST_ASSERT(lr.getLrCompanyName() == "ABC LAW GROUP", "Company name should be set (normalized)");
    TEST_ASSERT(lr.getLrFullName() == "SARAH JOHNSON", "Full name should be set (normalized)");
    TEST_ASSERT(lr.getLrPhone() == "9055557777", "Phone should be set (normalized to digits only)");
    TEST_ASSERT(lr.getLrEmail() == "SARAH@ABCLAW.COM", "Email should be set (normalized)");
    
    return true;
}

// Test stream operators
bool test_stream_operators() {
    LegalRepresentative lr(300001, "law offices inc", "mike wilson", 
                          "416-555-8888", "mike@lawoffices.com");
    
    stringstream ss;
    ss << lr;
    string output = ss.str();
    
    TEST_ASSERT(output.find("LegalRepresentative") != string::npos, "Output should contain class identifier");
    TEST_ASSERT(output.find("LRT") != string::npos, "Output should contain form type");
    TEST_ASSERT(output.find("MIKE WILSON") != string::npos, "Output should contain name (normalized)");
    TEST_ASSERT(output.find("LAW OFFICES INC") != string::npos, "Output should contain company (normalized)");
    
    // Test input operator
    stringstream input_stream;
    LegalRepresentative lr_input;
    input_stream >> lr_input;
    
    TEST_ASSERT(lr_input.getType() == "LRT", "toString should contain form type");
    TEST_ASSERT(!lr_input.toString().empty(), "toString should contain class identifier");
    
    return true;
}

// Test ID generation
bool test_id_generation() {
    int initial_id = LegalRepresentative::getNextId();
    
    LegalRepresentative lr1;
    LegalRepresentative lr2;
    
    TEST_ASSERT(lr1.getLegalRepresentativeId() >= initial_id, "First ID should be greater than or equal to initial");
    TEST_ASSERT(lr2.getLegalRepresentativeId() > lr1.getLegalRepresentativeId(), "Second ID should be greater than first");
    TEST_ASSERT(lr1.getLegalRepresentativeId() >= 700000, "ID should have correct prefix");
    TEST_ASSERT(lr2.getLegalRepresentativeId() >= 700000, "ID should have correct prefix");
    
    return true;
}

// Test display methods
bool test_display_methods() {
    LegalRepresentative lr(300001, "test law firm", "display test", 
                          "416-555-4444", "display@test.com");
    
    // Test toString
    string str = lr.toString();
    TEST_ASSERT(str.find("LegalRepresentative") != string::npos, "toString should contain class name");
    TEST_ASSERT(str.find("DISPLAY TEST") != string::npos, "toString should contain name (normalized)");
    TEST_ASSERT(str.find("LRT") != string::npos, "toString should contain type");
    
    // Test display methods (visual verification)
    cout << "   Testing displaySummary:" << endl;
    lr.displaySummary();
    cout << "   Testing displayInfo:" << endl;
    lr.displayInfo();
    
    return true;
}

// Test form type consistency
bool test_form_type() {
    LegalRepresentative lr1;
    LegalRepresentative lr2(300001, "law firm", "john doe", "416-555-1111", "john@law.com");
    LegalRepresentative lr3(700001, 300001, "law corp", "jane doe", "416-555-2222", 
                           "jane@law.com", DateTime::now(), DateTime::now());
    
    TEST_ASSERT(lr1.getType() == "LRT", "Default constructor should set type to LRT");
    TEST_ASSERT(lr2.getType() == "LRT", "Parameterized constructor should set type to LRT");
    TEST_ASSERT(lr3.getType() == "LRT", "Full constructor should set type to LRT");
    
    return true;
}

// Main test runner
int main() {
    cout << "🚀 LegalRepresentative Unit Tests" << endl;
    cout << "==================================" << endl;

    // Reset counter before tests
    LegalRepresentative::resetIdCounter();

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
