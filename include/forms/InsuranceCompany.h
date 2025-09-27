#ifndef INSURANCECOMPANY_H
#define INSURANCECOMPANY_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class InsuranceCompany {
        private:
            static const int MAX_COMPANY_NAME_LENGTH = 150;
            static const int MAX_ADJUSTER_NAME_LENGTH = 100;
            static const int MAX_CLAIM_NUMBER_LENGTH = 50;
            static const int MAX_POLICY_NUMBER_LENGTH = 50;
            static const string FORM_TYPE;     // Form type identifier: "ICO"
            
            // Primary attributes
            int m_ic_id;                    // Insurance Company ID (800000+)
            int m_client_id;               // Foreign key to client
            string m_type;                 // Form type (default: "ICO")
            string m_ic_company_name;      // Nome da empresa de seguro
            string m_ic_adjuster_name;     // Nome do ajustador
            string m_ic_phone;             // Telefone
            string m_ic_email;             // Email
            string m_ic_claim_number;      // Número da reclamação
            string m_ic_policy_number;     // Número da apólice
            DateTime m_ic_date_accident;   // Data do acidente
            DateTime m_ic_createdAt;       // Data de criação
            DateTime m_ic_updatedAt;       // Data de atualização
            
            static int insurance_company_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextInsuranceCompanyId();

        public:
            // Constructors
            InsuranceCompany();
            InsuranceCompany(int client_id, const string& ic_company_name, const string& ic_adjuster_name, 
                           const string& ic_phone, const string& ic_email, const string& ic_claim_number,
                           const string& ic_policy_number, const DateTime& ic_date_accident);
            InsuranceCompany(int ic_id, int client_id, const string& ic_company_name, 
                           const string& ic_adjuster_name, const string& ic_phone, 
                           const string& ic_email, const string& ic_claim_number,
                           const string& ic_policy_number, const DateTime& ic_date_accident,
                           const DateTime& ic_createdAt, const DateTime& ic_updatedAt);
            
            // Destructor
            ~InsuranceCompany() = default;
            
            // Getters
            int getInsuranceCompanyId() const { return m_ic_id; }
            int getClientId() const { return m_client_id; }
            string getType() const { return m_type; }
            string getIcCompanyName() const { return m_ic_company_name; }
            string getIcAdjusterName() const { return m_ic_adjuster_name; }
            string getIcPhone() const { return m_ic_phone; }
            string getIcEmail() const { return m_ic_email; }
            string getIcClaimNumber() const { return m_ic_claim_number; }
            string getIcPolicyNumber() const { return m_ic_policy_number; }
            DateTime getIcDateAccident() const { return m_ic_date_accident; }
            DateTime getIcCreatedAt() const { return m_ic_createdAt; }
            DateTime getIcUpdatedAt() const { return m_ic_updatedAt; }
            
            // Setters with validation
            void setClientId(int client_id) { m_client_id = client_id; updateTimestamp(); }
            void setIcCompanyName(const string& ic_company_name);
            void setIcAdjusterName(const string& ic_adjuster_name);
            void setIcPhone(const string& ic_phone);
            void setIcEmail(const string& ic_email);
            void setIcClaimNumber(const string& ic_claim_number);
            void setIcPolicyNumber(const string& ic_policy_number);
            void setIcDateAccident(const DateTime& ic_date_accident);
            void setIcCreatedAt(const DateTime& ic_createdAt) { m_ic_createdAt = ic_createdAt; }
            void setIcUpdatedAt(const DateTime& ic_updatedAt) { m_ic_updatedAt = ic_updatedAt; }
            
            // Validation methods
            bool isValidClientId(int client_id) const;
            bool isValidIcCompanyName(const string& ic_company_name) const;
            bool isValidIcAdjusterName(const string& ic_adjuster_name) const;
            bool isValidIcPhone(const string& ic_phone) const;
            bool isValidIcEmail(const string& ic_email) const;
            bool isValidIcClaimNumber(const string& ic_claim_number) const;
            bool isValidIcPolicyNumber(const string& ic_policy_number) const;
            bool isValidIcDateAccident(const DateTime& ic_date_accident) const;
            bool isValidData() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            // Sequential IDs starting from 1
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const InsuranceCompany& ic);
            friend std::istream& operator>>(std::istream& is, InsuranceCompany& ic);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // INSURANCECOMPANY_H
