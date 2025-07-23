#ifndef LEGALREPRESENTATIVE_H
#define LEGALREPRESENTATIVE_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class LegalRepresentative {
        private:
            static const int MAX_COMPANY_NAME_LENGTH = 150;
            static const int MAX_FULL_NAME_LENGTH = 100;
            static const string FORM_TYPE;     // Form type identifier: "LRT"
            
            // Primary attributes
            int m_lr_id;                    // Legal Representative ID (700000+)
            int m_client_id;               // Foreign key to client
            string m_type;                 // Form type (default: "LRT")
            string m_lr_company_name;      // Nome da empresa/escritório
            string m_lr_full_name;         // Nome completo do representante
            string m_lr_phone;             // Telefone
            string m_lr_email;             // Email
            DateTime m_lr_createdAt;       // Data de criação
            DateTime m_lr_updatedAt;       // Data de atualização
            
            static int legal_representative_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextLegalRepresentativeId();

        public:
            // Constructors
            LegalRepresentative();
            LegalRepresentative(int client_id, const string& lr_company_name, const string& lr_full_name, 
                              const string& lr_phone, const string& lr_email);
            LegalRepresentative(int lr_id, int client_id, const string& lr_company_name, 
                              const string& lr_full_name, const string& lr_phone, 
                              const string& lr_email, const DateTime& lr_createdAt, 
                              const DateTime& lr_updatedAt);
            
            // Destructor
            ~LegalRepresentative() = default;
            
            // Getters
            int getLegalRepresentativeId() const { return m_lr_id; }
            int getClientId() const { return m_client_id; }
            string getType() const { return m_type; }
            string getLrCompanyName() const { return m_lr_company_name; }
            string getLrFullName() const { return m_lr_full_name; }
            string getLrPhone() const { return m_lr_phone; }
            string getLrEmail() const { return m_lr_email; }
            DateTime getLrCreatedAt() const { return m_lr_createdAt; }
            DateTime getLrUpdatedAt() const { return m_lr_updatedAt; }
            
            // Setters with validation
            void setClientId(int client_id) { m_client_id = client_id; updateTimestamp(); }
            void setLrCompanyName(const string& lr_company_name);
            void setLrFullName(const string& lr_full_name);
            void setLrPhone(const string& lr_phone);
            void setLrEmail(const string& lr_email);
            void setLrCreatedAt(const DateTime& lr_createdAt) { m_lr_createdAt = lr_createdAt; }
            void setLrUpdatedAt(const DateTime& lr_updatedAt) { m_lr_updatedAt = lr_updatedAt; }
            
            // Validation methods
            bool isValidClientId(int client_id) const;
            bool isValidLrCompanyName(const string& lr_company_name) const;
            bool isValidLrFullName(const string& lr_full_name) const;
            bool isValidLrPhone(const string& lr_phone) const;
            bool isValidLrEmail(const string& lr_email) const;
            bool isValidData() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            static const int ID_PREFIX = 700000; // Legal Representative IDs start at 700001
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const LegalRepresentative& lr);
            friend std::istream& operator>>(std::istream& is, LegalRepresentative& lr);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // LEGALREPRESENTATIVE_H
