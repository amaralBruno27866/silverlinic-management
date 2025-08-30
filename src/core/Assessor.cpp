#include "core/Assessor.h"
#include <iostream>
#include <regex>
#include <sstream>

using namespace std;
namespace SilverClinic {
    // Static variable for ID counter
    static int assessor_id_counter = 0;
    
    // Constructors
    Assessor::Assessor() 
        : m_assessor_id(getNextAssessorId()), m_first_name(""), m_last_name(""), 
          m_email(""), m_phone(""), m_address(), m_created_at(DateTime::now()), 
          m_updated_at(DateTime::now()) {
    }
    
    Assessor::Assessor(int assessor_id, const string& first_name, const string& last_name, 
                       const string& email, const string& phone, const Address& address, 
                       const DateTime& created_at, const DateTime& updated_at)
        : m_assessor_id(assessor_id), m_first_name(utils::normalizeName(first_name)), 
          m_last_name(utils::normalizeName(last_name)), m_email(utils::normalizeForDatabase(email)), 
          m_phone(utils::normalizePhoneNumber(phone)), m_address(address), 
          m_created_at(created_at), m_updated_at(updated_at) {
    }
    
    // Copy constructor
    Assessor::Assessor(const Assessor& other)
        : m_assessor_id(other.m_assessor_id), m_first_name(other.m_first_name),
          m_last_name(other.m_last_name), m_email(other.m_email), m_phone(other.m_phone),
          m_address(other.m_address), m_created_at(other.m_created_at), 
          m_updated_at(other.m_updated_at) {
    }
    
    // Assignment operator
    Assessor& Assessor::operator=(const Assessor& other) {
        if (this != &other) {
            m_assessor_id = other.m_assessor_id;
            m_first_name = other.m_first_name;
            m_last_name = other.m_last_name;
            m_email = other.m_email;
            m_phone = other.m_phone;
            m_address = other.m_address;
            m_created_at = other.m_created_at;
            m_updated_at = other.m_updated_at;
        }
        return *this;
    }
    
    // Destructor
    Assessor::~Assessor() {
        // Nothing to do for basic types and objects with proper destructors
    }
    
    // Utility methods
    string Assessor::getFullName() const {
        return m_first_name + " " + m_last_name;
    }
    
    bool Assessor::isValidEmail() const {
        // Simple email validation regex
        regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        return regex_match(m_email, email_pattern);
    }
    
    bool Assessor::isValidPhone() const {
        // Canadian phone number validation (various formats)
        regex phone_pattern(R"(^\+?1?[-.\s]?\(?[0-9]{3}\)?[-.\s]?[0-9]{3}[-.\s]?[0-9]{4}$)");
        return regex_match(m_phone, phone_pattern);
    }
    
    void Assessor::displayInfo() const {
        cout << "=================================================" << endl;
        cout << "              ASSESSOR INFORMATION              " << endl;
        cout << "=================================================" << endl;
        cout << "Assessor ID: " << m_assessor_id << endl;
        cout << "Name: " << getFullName() << endl;
        cout << "Email: " << m_email << endl;
        cout << "Phone: " << m_phone << endl;
        cout << "Email Valid: " << (isValidEmail() ? "Yes" : "No") << endl;
        cout << "Phone Valid: " << (isValidPhone() ? "Yes" : "No") << endl;
        cout << "Created: " << m_created_at.toString() << endl;
        cout << "Updated: " << m_updated_at.toString() << endl;
        cout << "------------------------------------------------" << endl;
        cout << "Address Information:" << endl;
        m_address.displayInfo();
        cout << "=================================================" << endl;
    }
    
    // Static methods for ID management
    int Assessor::getNextAssessorId() {
        return ID_PREFIX + (++assessor_id_counter);
    }
    
    void Assessor::resetIdCounter() {
        assessor_id_counter = 0;
    }
    
    // Stream operators
    ostream& operator<<(ostream& os, const Assessor& assessor) {
        os << "Assessor ID: " << assessor.m_assessor_id << endl;
        os << "Name: " << assessor.getFullName() << endl;
        os << "Email: " << assessor.m_email << endl;
        os << "Phone: " << assessor.m_phone << endl;
        os << "Created: " << assessor.m_created_at.toString() << endl;
        os << "Updated: " << assessor.m_updated_at.toString() << endl;
        os << "Address: " << endl << assessor.m_address;
        return os;
    }
    
    istream& operator>>(istream& is, Assessor& assessor) {
        cout << "Enter first name: ";
        is >> assessor.m_first_name;
        cout << "Enter last name: ";
        is >> assessor.m_last_name;
        cout << "Enter email: ";
        is >> assessor.m_email;
        cout << "Enter phone: ";
        is >> assessor.m_phone;
        cout << "Enter address information:" << endl;
        is >> assessor.m_address;
        assessor.updateTimestamp();
        return is;
    }
}
