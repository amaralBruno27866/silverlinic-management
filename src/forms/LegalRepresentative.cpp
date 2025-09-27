#include "forms/LegalRepresentative.h"
#include "core/Utils.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static members initialization
        const string LegalRepresentative::FORM_TYPE = "LRT";
        int LegalRepresentative::legal_representative_id_counter = 1; // Sequential IDs starting from 1

        // Private helper methods
        void LegalRepresentative::setTimestamps() {
            m_lr_createdAt = DateTime::now();
            m_lr_updatedAt = DateTime::now();
        }

        void LegalRepresentative::updateTimestamp() {
            m_lr_updatedAt = DateTime::now();
        }

        int LegalRepresentative::getNextLegalRepresentativeId() {
            return ++legal_representative_id_counter;
        }

        // Constructors
        LegalRepresentative::LegalRepresentative() 
            : m_lr_id(getNextLegalRepresentativeId()), m_client_id(0), m_type(FORM_TYPE),
              m_lr_company_name(""), m_lr_full_name(""), m_lr_phone(""), m_lr_email("") {
            setTimestamps();
        }

        LegalRepresentative::LegalRepresentative(int client_id, const string& lr_company_name, 
                                               const string& lr_full_name, const string& lr_phone, 
                                               const string& lr_email)
            : m_lr_id(getNextLegalRepresentativeId()), m_client_id(client_id), m_type(FORM_TYPE),
              m_lr_company_name(utils::normalizeForDatabase(lr_company_name)), 
              m_lr_full_name(utils::normalizeForDatabase(lr_full_name)),
              m_lr_phone(utils::normalizePhoneNumber(lr_phone)), 
              m_lr_email(utils::normalizeForDatabase(lr_email)) {
            setTimestamps();
        }

        LegalRepresentative::LegalRepresentative(int lr_id, int client_id, const string& lr_company_name, 
                                               const string& lr_full_name, const string& lr_phone, 
                                               const string& lr_email, const DateTime& lr_createdAt, 
                                               const DateTime& lr_updatedAt)
            : m_lr_id(lr_id), m_client_id(client_id), m_type(FORM_TYPE),
              m_lr_company_name(utils::normalizeForDatabase(lr_company_name)), 
              m_lr_full_name(utils::normalizeForDatabase(lr_full_name)),
              m_lr_phone(utils::normalizePhoneNumber(lr_phone)), 
              m_lr_email(utils::normalizeForDatabase(lr_email)),
              m_lr_createdAt(lr_createdAt), m_lr_updatedAt(lr_updatedAt) {
        }

        // Setters with validation and normalization
        void LegalRepresentative::setLrCompanyName(const string& lr_company_name) {
            if (isValidLrCompanyName(lr_company_name)) {
                m_lr_company_name = utils::normalizeForDatabase(lr_company_name);
                updateTimestamp();
            }
        }

        void LegalRepresentative::setLrFullName(const string& lr_full_name) {
            if (isValidLrFullName(lr_full_name)) {
                m_lr_full_name = utils::normalizeForDatabase(lr_full_name);
                updateTimestamp();
            }
        }

        void LegalRepresentative::setLrPhone(const string& lr_phone) {
            if (isValidLrPhone(lr_phone)) {
                m_lr_phone = utils::normalizePhoneNumber(lr_phone);
                updateTimestamp();
            }
        }

        void LegalRepresentative::setLrEmail(const string& lr_email) {
            if (isValidLrEmail(lr_email)) {
                m_lr_email = utils::normalizeForDatabase(lr_email);
                updateTimestamp();
            }
        }

        // Validation methods
        bool LegalRepresentative::isValidClientId(int client_id) const {
            return client_id > 0;
        }

        bool LegalRepresentative::isValidLrCompanyName(const string& lr_company_name) const {
            string trimmed = utils::trim(lr_company_name);
            return !trimmed.empty() && trimmed.length() <= MAX_COMPANY_NAME_LENGTH;
        }

        bool LegalRepresentative::isValidLrFullName(const string& lr_full_name) const {
            string trimmed = utils::trim(lr_full_name);
            return !trimmed.empty() && trimmed.length() <= MAX_FULL_NAME_LENGTH;
        }

        bool LegalRepresentative::isValidLrPhone(const string& lr_phone) const {
            return utils::isValidPhoneNumber(lr_phone);
        }

        bool LegalRepresentative::isValidLrEmail(const string& lr_email) const {
            return utils::isValidEmail(lr_email);
        }

        bool LegalRepresentative::isValidData() const {
            return isValidClientId(m_client_id) && 
                   isValidLrCompanyName(m_lr_company_name) && 
                   isValidLrFullName(m_lr_full_name) &&
                   isValidLrPhone(m_lr_phone) &&
                   isValidLrEmail(m_lr_email);
        }

        // Display methods
        void LegalRepresentative::displayInfo() const {
            cout << "\n=== Legal Representative Information ===" << endl;
            cout << "ID: " << m_lr_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Client ID: " << m_client_id << endl;
            cout << "Company Name: " << m_lr_company_name << endl;
            cout << "Full Name: " << m_lr_full_name << endl;
            cout << "Phone: " << m_lr_phone << endl;
            cout << "Email: " << m_lr_email << endl;
            cout << "Created: " << m_lr_createdAt.toString() << endl;
            cout << "Updated: " << m_lr_updatedAt.toString() << endl;
            cout << "=======================================" << endl;
        }

        void LegalRepresentative::displaySummary() const {
            cout << "Legal Representative [" << m_lr_id << "] (" << m_type << "): " 
                 << m_lr_full_name << " - " << m_lr_company_name << endl;
        }

        string LegalRepresentative::toString() const {
            stringstream ss;
            ss << "LegalRepresentative{id=" << m_lr_id 
               << ", type=" << m_type
               << ", clientId=" << m_client_id 
               << ", companyName='" << m_lr_company_name << "'"
               << ", fullName='" << m_lr_full_name << "'"
               << ", phone='" << m_lr_phone << "'"
               << ", email='" << m_lr_email << "'"
               << ", created=" << m_lr_createdAt.toString()
               << ", updated=" << m_lr_updatedAt.toString() << "}";
            return ss.str();
        }

        // Static methods for ID management
        void LegalRepresentative::resetIdCounter() {
            legal_representative_id_counter = 1; // Sequential IDs starting from 1
        }

        int LegalRepresentative::getNextId() {
            return legal_representative_id_counter + 1;
        }

        // Stream operators
        std::ostream& operator<<(std::ostream& os, const LegalRepresentative& lr) {
            os << "LegalRepresentative[" << lr.m_lr_id << "] (" << lr.m_type << "): "
               << lr.m_lr_full_name << " - " << lr.m_lr_company_name 
               << " | Phone: " << lr.m_lr_phone << " | Email: " << lr.m_lr_email;
            return os;
        }

        std::istream& operator>>(std::istream& is, LegalRepresentative& lr) {
            // Manual creation for testing purposes
            LegalRepresentative temp;
            temp.m_lr_id = 700001;
            temp.m_client_id = 300001;
            temp.m_type = LegalRepresentative::FORM_TYPE;
            temp.m_lr_company_name = "TEST LEGAL CORP";
            temp.m_lr_full_name = "JOHN LAWYER";
            temp.m_lr_phone = "4165551234";
            temp.m_lr_email = "JOHN@LEGALCORP.COM";
            temp.setTimestamps();
            
            lr = temp;
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
