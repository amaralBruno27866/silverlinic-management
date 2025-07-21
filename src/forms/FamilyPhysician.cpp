#include "FamilyPhysician.h"
#include "core/Utils.h"
#include <iostream>
#include <iomanip>

using namespace utils;

namespace SilverClinic {
namespace Forms {

// Static variable for ID management
int FamilyPhysician::family_physician_id_counter = 1;

// Private methods
void FamilyPhysician::setTimestamps() {
    m_created_at = DateTime();
    m_updated_at = m_created_at;
}

void FamilyPhysician::updateTimestamp() {
    m_updated_at = DateTime();
}

int FamilyPhysician::getNextFamilyPhysicianId() {
    return 500000 + family_physician_id_counter++;
}

// Constructors
FamilyPhysician::FamilyPhysician() {
    m_family_physician_id = getNextFamilyPhysicianId();
    m_client_id = 0;
    m_doctor_name = "";
    m_clinic_name = "";
    m_phone = "";
    m_email = "";
    m_address = "";
    m_postal_code = "";
    m_city = "";
    m_province = "";
    m_specialization = "";
    m_notes = "";
    setTimestamps();
    utils::logMessage("INFO", "New Family Physician record created with ID " + to_string(m_family_physician_id));
}

FamilyPhysician::FamilyPhysician(int client_id, const string& doctor_name, const string& clinic_name) {
    m_family_physician_id = getNextFamilyPhysicianId();
    m_client_id = client_id;
    m_doctor_name = doctor_name;
    m_clinic_name = clinic_name;
    m_phone = "";
    m_email = "";
    m_address = "";
    m_postal_code = "";
    m_city = "";
    m_province = "";
    m_specialization = "";
    m_notes = "";
    setTimestamps();
    utils::logMessage("INFO", "New Family Physician record created: " + doctor_name + " at " + clinic_name);
}

FamilyPhysician::FamilyPhysician(int family_physician_id, int client_id, const string& doctor_name, 
                               const string& clinic_name, const string& phone, const string& email,
                               const string& address, const string& postal_code, const string& city,
                               const string& province, const string& specialization, const string& notes,
                               const DateTime& created_at, const DateTime& updated_at) {
    m_family_physician_id = family_physician_id;
    m_client_id = client_id;
    m_doctor_name = doctor_name;
    m_clinic_name = clinic_name;
    m_phone = phone;
    m_email = email;
    m_address = address;
    m_postal_code = postal_code;
    m_city = city;
    m_province = province;
    m_specialization = specialization;
    m_notes = notes;
    m_created_at = created_at;
    m_updated_at = updated_at;
}

// Setters with validation
void FamilyPhysician::setDoctorName(const string& doctor_name) {
    if (isValidDoctorName(doctor_name)) {
        m_doctor_name = doctor_name;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Doctor name truncated to " + to_string(MAX_DOCTOR_NAME_LENGTH) + " characters");
        m_doctor_name = doctor_name.substr(0, MAX_DOCTOR_NAME_LENGTH);
        updateTimestamp();
    }
}

void FamilyPhysician::setClinicName(const string& clinic_name) {
    if (isValidClinicName(clinic_name)) {
        m_clinic_name = clinic_name;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Clinic name truncated to " + to_string(MAX_CLINIC_NAME_LENGTH) + " characters");
        m_clinic_name = clinic_name.substr(0, MAX_CLINIC_NAME_LENGTH);
        updateTimestamp();
    }
}

void FamilyPhysician::setPhone(const string& phone) {
    if (isValidPhoneNumber(phone)) {
        m_phone = phone;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Invalid phone number format: " + phone);
        m_phone = phone; // Store anyway but log warning
        updateTimestamp();
    }
}

void FamilyPhysician::setEmail(const string& email) {
    if (isValidEmail(email)) {
        m_email = email;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Invalid email format: " + email);
        m_email = email; // Store anyway but log warning
        updateTimestamp();
    }
}

void FamilyPhysician::setPostalCode(const string& postal_code) {
    if (isValidCanadianPostalCode(postal_code)) {
        m_postal_code = formatCanadianPostalCode(postal_code);
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Invalid Canadian postal code format: " + postal_code);
        m_postal_code = postal_code; // Store anyway but log warning
        updateTimestamp();
    }
}

void FamilyPhysician::setNotes(const string& notes) {
    if (isValidNotes(notes)) {
        m_notes = notes;
        updateTimestamp();
    } else {
        utils::logMessage("WARNING", "Notes truncated to " + to_string(MAX_NOTES_LENGTH) + " characters");
        m_notes = notes.substr(0, MAX_NOTES_LENGTH);
        updateTimestamp();
    }
}

// Validation methods
bool FamilyPhysician::isValidClientId(int client_id) const {
    return client_id >= 300000; // Client IDs start at 300000
}

bool FamilyPhysician::isValidDoctorName(const string& doctor_name) const {
    return !doctor_name.empty() && doctor_name.length() <= MAX_DOCTOR_NAME_LENGTH;
}

bool FamilyPhysician::isValidClinicName(const string& clinic_name) const {
    return clinic_name.length() <= MAX_CLINIC_NAME_LENGTH;
}

bool FamilyPhysician::isValidNotes(const string& notes) const {
    return notes.length() <= MAX_NOTES_LENGTH;
}

bool FamilyPhysician::isValidData() const {
    return isValidClientId(m_client_id) && 
           isValidDoctorName(m_doctor_name) &&
           isValidClinicName(m_clinic_name) &&
           isValidNotes(m_notes);
}

// Display methods
void FamilyPhysician::displayInfo() const {
    cout << "\n=== Family Physician Information ===" << endl;
    cout << "ID: " << m_family_physician_id << endl;
    cout << "Client ID: " << m_client_id << endl;
    cout << "Doctor Name: " << m_doctor_name << endl;
    cout << "Clinic Name: " << m_clinic_name << endl;
    cout << "Phone: " << (m_phone.empty() ? "Not provided" : m_phone) << endl;
    cout << "Email: " << (m_email.empty() ? "Not provided" : m_email) << endl;
    cout << "Address: " << (m_address.empty() ? "Not provided" : m_address) << endl;
    cout << "Postal Code: " << (m_postal_code.empty() ? "Not provided" : m_postal_code) << endl;
    cout << "City: " << (m_city.empty() ? "Not provided" : m_city) << endl;
    cout << "Province: " << (m_province.empty() ? "Not provided" : m_province) << endl;
    cout << "Specialization: " << (m_specialization.empty() ? "General Practice" : m_specialization) << endl;
    cout << "Notes (" << m_notes.length() << "/" << MAX_NOTES_LENGTH << " chars): " << 
            (m_notes.empty() ? "None" : m_notes) << endl;
    cout << "Created: " << m_created_at.toString() << endl;
    cout << "Updated: " << m_updated_at.toString() << endl;
    cout << "=====================================" << endl;
}

void FamilyPhysician::displaySummary() const {
    cout << "Family Physician [" << m_family_physician_id << "]: " 
         << m_doctor_name << " (" << m_clinic_name << ")" << endl;
}

string FamilyPhysician::toString() const {
    return "FamilyPhysician{ID=" + to_string(m_family_physician_id) + 
           ", Doctor=" + m_doctor_name + ", Clinic=" + m_clinic_name + "}";
}

// Static methods
void FamilyPhysician::resetIdCounter() {
    family_physician_id_counter = 1;
    utils::logMessage("INFO", "FamilyPhysician ID counter reset");
}

int FamilyPhysician::getNextId() {
    return 500000 + family_physician_id_counter;
}

} // namespace Forms
} // namespace SilverClinic
