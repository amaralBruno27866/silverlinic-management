#include "forms/FamilyPhysician.h"
#include "core/Utils.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace SilverClinic;

namespace SilverClinic {
namespace Forms {

// Static variable for ID management
int FamilyPhysician::family_physician_id_counter = 1;

// Constants
const int MAX_FULL_NAME_LENGTH = 100;

// Private methods
void FamilyPhysician::setTimestamps() {
    m_fp_createdAt = DateTime();
    m_fp_updatedAt = m_fp_createdAt;
}

void FamilyPhysician::updateTimestamp() {
    m_fp_updatedAt = DateTime();
}

int FamilyPhysician::getNextFamilyPhysicianId() {
    return 500000 + family_physician_id_counter++;
}

// Constructors
FamilyPhysician::FamilyPhysician() {
    m_fp_id = getNextFamilyPhysicianId();
    m_client_id = 0;
    m_fp_full_name = "";
    m_fp_phone = "";
    m_fp_email = "";
    m_fp_address = Address();
    setTimestamps();
}

FamilyPhysician::FamilyPhysician(int client_id, const string& fp_full_name, const string& fp_phone) {
    m_fp_id = getNextFamilyPhysicianId();
    m_client_id = client_id;
    m_fp_full_name = fp_full_name;
    m_fp_phone = fp_phone;
    m_fp_email = "";
    m_fp_address = Address();
    setTimestamps();
}

FamilyPhysician::FamilyPhysician(int fp_id, int client_id, const string& fp_full_name,
                                 const string& fp_phone, const string& fp_email,
                                 const Address& fp_address,
                                 const DateTime& fp_createdAt, const DateTime& fp_updatedAt) {
    m_fp_id = fp_id;
    m_client_id = client_id;
    m_fp_full_name = fp_full_name;
    m_fp_phone = fp_phone;
    m_fp_email = fp_email;
    m_fp_address = fp_address;
    m_fp_createdAt = fp_createdAt;
    m_fp_updatedAt = fp_updatedAt;
}

// Setter methods
void FamilyPhysician::setFpFullName(const string& fp_full_name) {
    if (isValidFpFullName(fp_full_name)) {
        m_fp_full_name = fp_full_name;
        updateTimestamp();
    } else if (fp_full_name.length() > MAX_FULL_NAME_LENGTH) {
        utils::logMessage("WARNING", "Full name truncated to " + to_string(MAX_FULL_NAME_LENGTH) + " characters");
        m_fp_full_name = fp_full_name.substr(0, MAX_FULL_NAME_LENGTH);
        updateTimestamp();
    }
}

void FamilyPhysician::setFpPhone(const string& fp_phone) {
    if (utils::isValidPhoneNumber(fp_phone) || fp_phone.empty()) {
        m_fp_phone = fp_phone;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Invalid phone number format: " + fp_phone);
        m_fp_phone = fp_phone; // Store anyway but log warning
        updateTimestamp();
    }
}

void FamilyPhysician::setFpEmail(const string& fp_email) {
    if (utils::isValidEmail(fp_email) || fp_email.empty()) {
        m_fp_email = fp_email;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Invalid email format: " + fp_email);
        m_fp_email = fp_email; // Store anyway but log warning
        updateTimestamp();
    }
}

// Validation methods
bool FamilyPhysician::isValidClientId(int client_id) const {
    return client_id > 0;
}

bool FamilyPhysician::isValidFpFullName(const string& fp_full_name) const {
    return !fp_full_name.empty() && fp_full_name.length() <= MAX_FULL_NAME_LENGTH;
}

bool FamilyPhysician::isValidData() const {
    return isValidClientId(m_client_id) &&
           isValidFpFullName(m_fp_full_name);
}

// Display methods
void FamilyPhysician::displayInfo() const {
    cout << "\n=== Family Physician Information ===" << endl;
    cout << "ID: " << m_fp_id << endl;
    cout << "Client ID: " << m_client_id << endl;
    cout << "Full Name: " << m_fp_full_name << endl;
    cout << "Phone: " << (m_fp_phone.empty() ? "Not provided" : m_fp_phone) << endl;
    cout << "Email: " << (m_fp_email.empty() ? "Not provided" : m_fp_email) << endl;
    cout << "Address: " << endl;
    m_fp_address.displayInfo();
    cout << "Created: " << m_fp_createdAt.toString() << endl;
    cout << "Updated: " << m_fp_updatedAt.toString() << endl;
    cout << "=====================================" << endl;
}

void FamilyPhysician::displaySummary() const {
    cout << "Family Physician [" << m_fp_id << "]: " 
         << m_fp_full_name << endl;
}

string FamilyPhysician::toString() const {
    return "FamilyPhysician{ID=" + to_string(m_fp_id) + 
           ", Name=" + m_fp_full_name + "}";
}

// Stream operators implementation
ostream& operator<<(ostream& os, const FamilyPhysician& fp) {
    os << "FamilyPhysician[" << fp.m_fp_id << "] {"
       << "\n  Client ID: " << fp.m_client_id
       << "\n  Name: " << fp.m_fp_full_name
       << "\n  Phone: " << (fp.m_fp_phone.empty() ? "Not provided" : fp.m_fp_phone)
       << "\n  Email: " << (fp.m_fp_email.empty() ? "Not provided" : fp.m_fp_email)
       << "\n  Address: " << fp.m_fp_address
       << "\n  Created: " << fp.m_fp_createdAt.toString()
       << "\n  Updated: " << fp.m_fp_updatedAt.toString()
       << "\n}";
    return os;
}

istream& operator>>(istream& is, FamilyPhysician& fp) {
    string line;
    // Simple format: ID|CLIENT_ID|NAME|PHONE|EMAIL|ADDRESS_LINE|CITY|STATE|ZIP
    if (getline(is, line)) {
        istringstream iss(line);
        string id_str, client_id_str, name, phone, email;
        string address_line, city, state, zip_code;
        
        if (getline(iss, id_str, '|') &&
            getline(iss, client_id_str, '|') &&
            getline(iss, name, '|') &&
            getline(iss, phone, '|') &&
            getline(iss, email, '|') &&
            getline(iss, address_line, '|') &&
            getline(iss, city, '|') &&
            getline(iss, state, '|') &&
            getline(iss, zip_code, '|')) {
            
            // Direct access to private members (friend function)
            fp.m_fp_id = stoi(id_str);
            fp.m_client_id = stoi(client_id_str);
            fp.m_fp_full_name = name;
            fp.m_fp_phone = phone;
            fp.m_fp_email = email;
            
            // Create Address object using the default constructor
            Address addr;
            fp.m_fp_address = addr;
            fp.updateTimestamp();
        }
    }
    return is;
}

// Static methods
void FamilyPhysician::resetIdCounter() {
    family_physician_id_counter = 1;
}

int FamilyPhysician::getNextId() {
    return 500000 + family_physician_id_counter;
}

} // namespace Forms
} // namespace SilverClinic
