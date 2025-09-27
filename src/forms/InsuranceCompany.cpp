#include "forms/InsuranceCompany.h"
#include "core/Utils.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace SilverClinic {
    namespace Forms {

        // Static members initialization
        const string InsuranceCompany::FORM_TYPE = "ICO";
        int InsuranceCompany::insurance_company_id_counter = 1; // Sequential IDs starting from 1

        // Private helper methods
        void InsuranceCompany::setTimestamps() {
            m_ic_createdAt = DateTime::now();
            m_ic_updatedAt = DateTime::now();
        }

        void InsuranceCompany::updateTimestamp() {
            m_ic_updatedAt = DateTime::now();
        }

        int InsuranceCompany::getNextInsuranceCompanyId() {
                        return insurance_company_id_counter + 1; // Sequential IDs starting from 1
        }

        // Constructors
        InsuranceCompany::InsuranceCompany() 
            : m_ic_id(getNextInsuranceCompanyId()), m_client_id(0), m_type(FORM_TYPE),
              m_ic_company_name(""), m_ic_adjuster_name(""), m_ic_phone(""), m_ic_email(""),
              m_ic_claim_number(""), m_ic_policy_number(""), m_ic_date_accident(DateTime()) {
            setTimestamps();
        }

        InsuranceCompany::InsuranceCompany(int client_id, const string& ic_company_name, 
                                         const string& ic_adjuster_name, const string& ic_phone, 
                                         const string& ic_email, const string& ic_claim_number,
                                         const string& ic_policy_number, const DateTime& ic_date_accident)
            : m_ic_id(getNextInsuranceCompanyId()), m_client_id(client_id), m_type(FORM_TYPE),
              m_ic_company_name(utils::normalizeForDatabase(ic_company_name)), 
              m_ic_adjuster_name(utils::normalizeForDatabase(ic_adjuster_name)),
              m_ic_phone(utils::normalizePhoneNumber(ic_phone)), 
              m_ic_email(utils::normalizeForDatabase(ic_email)),
              m_ic_claim_number(utils::normalizeForDatabase(ic_claim_number)),
              m_ic_policy_number(utils::normalizeForDatabase(ic_policy_number)),
              m_ic_date_accident(ic_date_accident) {
            setTimestamps();
        }

        InsuranceCompany::InsuranceCompany(int ic_id, int client_id, const string& ic_company_name, 
                                         const string& ic_adjuster_name, const string& ic_phone, 
                                         const string& ic_email, const string& ic_claim_number,
                                         const string& ic_policy_number, const DateTime& ic_date_accident,
                                         const DateTime& ic_createdAt, const DateTime& ic_updatedAt)
            : m_ic_id(ic_id), m_client_id(client_id), m_type(FORM_TYPE),
              m_ic_company_name(utils::normalizeForDatabase(ic_company_name)), 
              m_ic_adjuster_name(utils::normalizeForDatabase(ic_adjuster_name)),
              m_ic_phone(utils::normalizePhoneNumber(ic_phone)), 
              m_ic_email(utils::normalizeForDatabase(ic_email)),
              m_ic_claim_number(utils::normalizeForDatabase(ic_claim_number)),
              m_ic_policy_number(utils::normalizeForDatabase(ic_policy_number)),
              m_ic_date_accident(ic_date_accident),
              m_ic_createdAt(ic_createdAt), m_ic_updatedAt(ic_updatedAt) {
        }

        // Setters with validation and normalization
        void InsuranceCompany::setIcCompanyName(const string& ic_company_name) {
            if (isValidIcCompanyName(ic_company_name)) {
                m_ic_company_name = utils::normalizeForDatabase(ic_company_name);
                updateTimestamp();
            }
        }

        void InsuranceCompany::setIcAdjusterName(const string& ic_adjuster_name) {
            if (isValidIcAdjusterName(ic_adjuster_name)) {
                m_ic_adjuster_name = utils::normalizeForDatabase(ic_adjuster_name);
                updateTimestamp();
            }
        }

        void InsuranceCompany::setIcPhone(const string& ic_phone) {
            if (isValidIcPhone(ic_phone)) {
                m_ic_phone = utils::normalizePhoneNumber(ic_phone);
                updateTimestamp();
            }
        }

        void InsuranceCompany::setIcEmail(const string& ic_email) {
            if (isValidIcEmail(ic_email)) {
                m_ic_email = utils::normalizeForDatabase(ic_email);
                updateTimestamp();
            }
        }

        void InsuranceCompany::setIcClaimNumber(const string& ic_claim_number) {
            if (isValidIcClaimNumber(ic_claim_number)) {
                m_ic_claim_number = utils::normalizeForDatabase(ic_claim_number);
                updateTimestamp();
            }
        }

        void InsuranceCompany::setIcPolicyNumber(const string& ic_policy_number) {
            if (isValidIcPolicyNumber(ic_policy_number)) {
                m_ic_policy_number = utils::normalizeForDatabase(ic_policy_number);
                updateTimestamp();
            }
        }

        void InsuranceCompany::setIcDateAccident(const DateTime& ic_date_accident) {
            if (isValidIcDateAccident(ic_date_accident)) {
                m_ic_date_accident = ic_date_accident;
                updateTimestamp();
            }
        }

        // Validation methods
        bool InsuranceCompany::isValidClientId(int client_id) const {
            return client_id > 0;
        }

        bool InsuranceCompany::isValidIcCompanyName(const string& ic_company_name) const {
            string trimmed = utils::trim(ic_company_name);
            return !trimmed.empty() && trimmed.length() <= MAX_COMPANY_NAME_LENGTH;
        }

        bool InsuranceCompany::isValidIcAdjusterName(const string& ic_adjuster_name) const {
            string trimmed = utils::trim(ic_adjuster_name);
            return !trimmed.empty() && trimmed.length() <= MAX_ADJUSTER_NAME_LENGTH;
        }

        bool InsuranceCompany::isValidIcPhone(const string& ic_phone) const {
            return utils::isValidPhoneNumber(ic_phone);
        }

        bool InsuranceCompany::isValidIcEmail(const string& ic_email) const {
            return utils::isValidEmail(ic_email);
        }

        bool InsuranceCompany::isValidIcClaimNumber(const string& ic_claim_number) const {
            string trimmed = utils::trim(ic_claim_number);
            return !trimmed.empty() && trimmed.length() <= MAX_CLAIM_NUMBER_LENGTH;
        }

        bool InsuranceCompany::isValidIcPolicyNumber(const string& ic_policy_number) const {
            string trimmed = utils::trim(ic_policy_number);
            return !trimmed.empty() && trimmed.length() <= MAX_POLICY_NUMBER_LENGTH;
        }

        bool InsuranceCompany::isValidIcDateAccident(const DateTime& ic_date_accident) const {
            // Check if the DateTime is valid and not in the future
            if (!ic_date_accident.isValid()) return false;
            
            DateTime now = DateTime::now();
            if (ic_date_accident > now) return false; // Accident can't be in the future
            
            // Check if the date is not too far in the past (e.g., more than 100 years)
            int daysFromNow = ic_date_accident.getDaysFromNow();
            if (daysFromNow > 36500) return false; // ~100 years
            
            return true;
        }

        bool InsuranceCompany::isValidData() const {
            return isValidClientId(m_client_id) && 
                   isValidIcCompanyName(m_ic_company_name) && 
                   isValidIcAdjusterName(m_ic_adjuster_name) &&
                   isValidIcPhone(m_ic_phone) &&
                   isValidIcEmail(m_ic_email) &&
                   isValidIcClaimNumber(m_ic_claim_number) &&
                   isValidIcPolicyNumber(m_ic_policy_number) &&
                   isValidIcDateAccident(m_ic_date_accident);
        }

        // Display methods
        void InsuranceCompany::displayInfo() const {
            cout << "\n=== Insurance Company Information ===" << endl;
            cout << "ID: " << m_ic_id << endl;
            cout << "Type: " << m_type << endl;
            cout << "Client ID: " << m_client_id << endl;
            cout << "Company Name: " << m_ic_company_name << endl;
            cout << "Adjuster Name: " << m_ic_adjuster_name << endl;
            cout << "Phone: " << m_ic_phone << endl;
            cout << "Email: " << m_ic_email << endl;
            cout << "Claim Number: " << m_ic_claim_number << endl;
            cout << "Policy Number: " << m_ic_policy_number << endl;
            cout << "Date of Accident: " << m_ic_date_accident.toDateString() << endl;
            cout << "Created: " << m_ic_createdAt.toString() << endl;
            cout << "Updated: " << m_ic_updatedAt.toString() << endl;
            cout << "=====================================" << endl;
        }

        void InsuranceCompany::displaySummary() const {
            cout << "Insurance Company [" << m_ic_id << "] (" << m_type << "): " 
                 << m_ic_company_name << " - " << m_ic_adjuster_name 
                 << " | Claim: " << m_ic_claim_number << endl;
        }

        string InsuranceCompany::toString() const {
            stringstream ss;
            ss << "InsuranceCompany{id=" << m_ic_id 
               << ", type=" << m_type
               << ", clientId=" << m_client_id 
               << ", companyName='" << m_ic_company_name << "'"
               << ", adjusterName='" << m_ic_adjuster_name << "'"
               << ", phone='" << m_ic_phone << "'"
               << ", email='" << m_ic_email << "'"
               << ", claimNumber='" << m_ic_claim_number << "'"
               << ", policyNumber='" << m_ic_policy_number << "'"
               << ", dateAccident='" << m_ic_date_accident.toDateString() << "'"
               << ", created=" << m_ic_createdAt.toString()
               << ", updated=" << m_ic_updatedAt.toString() << "}";
            return ss.str();
        }

        // Static methods for ID management
        void InsuranceCompany::resetIdCounter() {
            insurance_company_id_counter = 1; // Sequential IDs starting from 1
        }

        int InsuranceCompany::getNextId() {
            return insurance_company_id_counter + 1;
        }

        // Stream operators
        std::ostream& operator<<(std::ostream& os, const InsuranceCompany& ic) {
            os << "InsuranceCompany[" << ic.m_ic_id << "] (" << ic.m_type << "): "
               << ic.m_ic_company_name << " - " << ic.m_ic_adjuster_name 
               << " | Phone: " << ic.m_ic_phone << " | Email: " << ic.m_ic_email
               << " | Claim: " << ic.m_ic_claim_number;
            return os;
        }

        std::istream& operator>>(std::istream& is, InsuranceCompany& ic) {
            // Manual creation for testing purposes
            InsuranceCompany temp;
            temp.m_ic_id = 800001;
            temp.m_client_id = 300001;
            temp.m_type = InsuranceCompany::FORM_TYPE;
            temp.m_ic_company_name = "TEST INSURANCE CORP";
            temp.m_ic_adjuster_name = "JOHN ADJUSTER";
            temp.m_ic_phone = "4165551234";
            temp.m_ic_email = "JOHN@INSURANCE.COM";
            temp.m_ic_claim_number = "CLM12345";
            temp.m_ic_policy_number = "POL67890";
            temp.m_ic_date_accident = DateTime(2024, 1, 15, 0, 0, 0);
            temp.setTimestamps();
            
            ic = temp;
            return is;
        }

    } // namespace Forms
} // namespace SilverClinic
