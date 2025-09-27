#include "forms/EmergencyContact.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

namespace SilverClinic {
namespace Forms {

    // Sta        return emergency_contact_id_counter + 1; // Sequential IDs starting from 1ic variable initialization
    int EmergencyContact::emergency_contact_id_counter = 0;
    const string EmergencyContact::FORM_TYPE = "ECT";

    // === CONSTRUCTORS ===

    // Default constructor
    EmergencyContact::EmergencyContact() {
        m_ec_id = getNextEmergencyContactId();
        m_client_id = 0;
        m_type = FORM_TYPE;
        m_ec_full_name = "";
        m_ec_relationship = "";
        m_ec_phone = "";
        m_ec_alt_phone = "";
        setTimestamps();
    }

    // Parameterized constructor
    EmergencyContact::EmergencyContact(int client_id, const string& ec_full_name, 
                                     const string& ec_relationship, const string& ec_phone, 
                                     const string& ec_alt_phone) {
        m_ec_id = getNextEmergencyContactId();
        m_client_id = client_id;
        m_type = FORM_TYPE;
        m_ec_full_name = utils::normalizeForDatabase(ec_full_name);
        m_ec_relationship = utils::normalizeForDatabase(ec_relationship);
        m_ec_phone = utils::normalizePhoneNumber(ec_phone);
        m_ec_alt_phone = utils::normalizePhoneNumber(ec_alt_phone);
        setTimestamps();
    }

    // Full constructor
    EmergencyContact::EmergencyContact(int ec_id, int client_id, const string& ec_full_name,
                                     const string& ec_relationship, const string& ec_phone,
                                     const string& ec_alt_phone, const DateTime& ec_createdAt,
                                     const DateTime& ec_updatedAt) {
        m_ec_id = ec_id;
        m_client_id = client_id;
        m_type = FORM_TYPE;
        m_ec_full_name = utils::normalizeForDatabase(ec_full_name);
        m_ec_relationship = utils::normalizeForDatabase(ec_relationship);
        m_ec_phone = utils::normalizePhoneNumber(ec_phone);
        m_ec_alt_phone = utils::normalizePhoneNumber(ec_alt_phone);
        m_ec_createdAt = ec_createdAt;
        m_ec_updatedAt = ec_updatedAt;
    }

    // === SETTER METHODS ===

    void EmergencyContact::setEcFullName(const string& ec_full_name) {
        string normalizedName = utils::normalizeForDatabase(ec_full_name);
        if (isValidEcFullName(normalizedName)) {
            m_ec_full_name = normalizedName;
            updateTimestamp();
        } else if (normalizedName.length() > MAX_FULL_NAME_LENGTH) {
            utils::logStructured(utils::LogLevel::WARN, {"FORM","truncate_fullname","EmergencyContact","",""}, "Full name truncated");
            m_ec_full_name = normalizedName.substr(0, MAX_FULL_NAME_LENGTH);
            updateTimestamp();
        }
    }

    void EmergencyContact::setEcRelationship(const string& ec_relationship) {
        string normalizedRelationship = utils::normalizeForDatabase(ec_relationship);
        if (isValidEcRelationship(normalizedRelationship)) {
            m_ec_relationship = normalizedRelationship;
            updateTimestamp();
        } else if (normalizedRelationship.length() > MAX_RELATIONSHIP_LENGTH) {
            utils::logStructured(utils::LogLevel::WARN, {"FORM","truncate_relationship","EmergencyContact","",""}, "Relationship truncated");
            m_ec_relationship = normalizedRelationship.substr(0, MAX_RELATIONSHIP_LENGTH);
            updateTimestamp();
        }
    }

    void EmergencyContact::setEcPhone(const string& ec_phone) {
        string normalizedPhone = utils::normalizePhoneNumber(ec_phone);
        if (utils::isValidPhoneNumber(normalizedPhone) || ec_phone.empty()) {
            m_ec_phone = normalizedPhone;
            updateTimestamp();
        } else {
            utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_phone","EmergencyContact","",""}, "Invalid phone: "+ ec_phone);
            m_ec_phone = normalizedPhone; // Store anyway but log warning
            updateTimestamp();
        }
    }

    void EmergencyContact::setEcAltPhone(const string& ec_alt_phone) {
        string normalizedPhone = utils::normalizePhoneNumber(ec_alt_phone);
        if (utils::isValidPhoneNumber(normalizedPhone) || ec_alt_phone.empty()) {
            m_ec_alt_phone = normalizedPhone;
            updateTimestamp();
        } else {
            utils::logStructured(utils::LogLevel::WARN, {"FORM","invalid_alt_phone","EmergencyContact","",""}, "Invalid alternative phone: "+ ec_alt_phone);
            m_ec_alt_phone = normalizedPhone; // Store anyway but log warning
            updateTimestamp();
        }
    }

    // === VALIDATION METHODS ===

    bool EmergencyContact::isValidClientId(int client_id) const {
        return client_id > 0;
    }

    bool EmergencyContact::isValidEcFullName(const string& ec_full_name) const {
        return !ec_full_name.empty() && ec_full_name.length() <= MAX_FULL_NAME_LENGTH;
    }

    bool EmergencyContact::isValidEcRelationship(const string& ec_relationship) const {
        return !ec_relationship.empty() && ec_relationship.length() <= MAX_RELATIONSHIP_LENGTH;
    }

    bool EmergencyContact::isValidData() const {
        return isValidClientId(m_client_id) && 
               isValidEcFullName(m_ec_full_name) && 
               isValidEcRelationship(m_ec_relationship) &&
               !m_ec_phone.empty();
    }

    // === DISPLAY METHODS ===

    void EmergencyContact::displayInfo() const {
        cout << "\n=== Emergency Contact Information ===" << endl;
        cout << "ID: " << m_ec_id << endl;
        cout << "Type: " << m_type << endl;
        cout << "Client ID: " << m_client_id << endl;
        cout << "Full Name: " << m_ec_full_name << endl;
        cout << "Relationship: " << m_ec_relationship << endl;
        cout << "Phone: " << m_ec_phone << endl;
        cout << "Alternative Phone: " << m_ec_alt_phone << endl;
        cout << "Created: " << m_ec_createdAt.toString() << endl;
        cout << "Updated: " << m_ec_updatedAt.toString() << endl;
        cout << "=====================================" << endl;
    }

    void EmergencyContact::displaySummary() const {
        cout << "Emergency Contact [" << m_ec_id << "] (" << m_type << "): " 
             << m_ec_full_name << " - " << m_ec_relationship << endl;
    }

    string EmergencyContact::toString() const {
        ostringstream oss;
        oss << "EmergencyContact[" << m_ec_id << "," << m_type << "," << m_client_id 
            << "," << m_ec_full_name << "," << m_ec_relationship << "," << m_ec_phone 
            << "," << m_ec_alt_phone << "]";
        return oss.str();
    }

    // === UTILITY METHODS ===

    void EmergencyContact::setTimestamps() {
        DateTime now = DateTime::now();
        m_ec_createdAt = now;
        m_ec_updatedAt = now;
    }

    void EmergencyContact::updateTimestamp() {
        m_ec_updatedAt = DateTime::now();
    }

    int EmergencyContact::getNextEmergencyContactId() {
        return ++emergency_contact_id_counter; // Sequential IDs starting from 1
    }

    // === STATIC METHODS ===

    void EmergencyContact::resetIdCounter() {
        emergency_contact_id_counter = 0;
    }

    int EmergencyContact::getNextId() {
        return emergency_contact_id_counter + 1; // Sequential IDs starting from 1
    }

    // === STREAM OPERATORS ===

    ostream& operator<<(ostream& os, const EmergencyContact& ec) {
        os << "Emergency Contact ID: " << ec.getEmergencyContactId() << "\n"
           << "Type: " << ec.getType() << "\n"
           << "Client ID: " << ec.getClientId() << "\n"
           << "Full Name: " << ec.getEcFullName() << "\n"
           << "Relationship: " << ec.getEcRelationship() << "\n"
           << "Phone: " << ec.getEcPhone() << "\n"
           << "Alternative Phone: " << ec.getEcAltPhone() << "\n"
           << "Created: " << ec.getEcCreatedAt().toString() << "\n"
           << "Updated: " << ec.getEcUpdatedAt().toString();
        return os;
    }

    istream& operator>>(istream& is, EmergencyContact& ec) {
        string full_name, relationship, phone, alt_phone;
        
        cout << "Enter full name: ";
        getline(is, full_name);
        ec.setEcFullName(full_name);
        
        cout << "Enter relationship: ";
        getline(is, relationship);
        ec.setEcRelationship(relationship);
        
        cout << "Enter phone: ";
        getline(is, phone);
        ec.setEcPhone(phone);
        
        cout << "Enter alternative phone (optional): ";
        getline(is, alt_phone);
        ec.setEcAltPhone(alt_phone);
        
        return is;
    }

} // namespace Forms
} // namespace SilverClinic
