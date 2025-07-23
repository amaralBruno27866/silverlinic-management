#ifndef EMERGENCYCONTACT_H
#define EMERGENCYCONTACT_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class EmergencyContact {
        private:
            static const int MAX_FULL_NAME_LENGTH = 100;
            static const int MAX_RELATIONSHIP_LENGTH = 50;
            static const string FORM_TYPE;     // Form type identifier: "ECT"
            
            // Primary attributes
            int m_ec_id;                    // Emergency Contact ID (600000+)
            int m_client_id;               // Foreign key to client
            string m_type;                 // Form type (default: "ECT")
            string m_ec_full_name;         // Nome completo do contato de emergência
            string m_ec_relationship;      // Relacionamento com o cliente
            string m_ec_phone;             // Telefone principal
            string m_ec_alt_phone;         // Telefone alternativo (opcional)
            DateTime m_ec_createdAt;       // Data de criação
            DateTime m_ec_updatedAt;       // Data de atualização
            
            static int emergency_contact_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextEmergencyContactId();

        public:
            // Constructors
            EmergencyContact();
            EmergencyContact(int client_id, const string& ec_full_name, const string& ec_relationship, 
                           const string& ec_phone, const string& ec_alt_phone = "");
            EmergencyContact(int ec_id, int client_id, const string& ec_full_name, 
                           const string& ec_relationship, const string& ec_phone, 
                           const string& ec_alt_phone, const DateTime& ec_createdAt, 
                           const DateTime& ec_updatedAt);
            
            // Destructor
            ~EmergencyContact() = default;
            
            // Getters
            int getEmergencyContactId() const { return m_ec_id; }
            int getClientId() const { return m_client_id; }
            string getType() const { return m_type; }
            string getEcFullName() const { return m_ec_full_name; }
            string getEcRelationship() const { return m_ec_relationship; }
            string getEcPhone() const { return m_ec_phone; }
            string getEcAltPhone() const { return m_ec_alt_phone; }
            DateTime getEcCreatedAt() const { return m_ec_createdAt; }
            DateTime getEcUpdatedAt() const { return m_ec_updatedAt; }
            
            // Setters with validation
            void setClientId(int client_id) { m_client_id = client_id; updateTimestamp(); }
            void setEcFullName(const string& ec_full_name);
            void setEcRelationship(const string& ec_relationship);
            void setEcPhone(const string& ec_phone);
            void setEcAltPhone(const string& ec_alt_phone);
            void setEcCreatedAt(const DateTime& ec_createdAt) { m_ec_createdAt = ec_createdAt; }
            void setEcUpdatedAt(const DateTime& ec_updatedAt) { m_ec_updatedAt = ec_updatedAt; }
            
            // Validation methods
            bool isValidClientId(int client_id) const;
            bool isValidEcFullName(const string& ec_full_name) const;
            bool isValidEcRelationship(const string& ec_relationship) const;
            bool isValidData() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            string toString() const;
            
            // Static methods for ID management
            static void resetIdCounter();
            static int getNextId();
            static const int ID_PREFIX = 600000; // Emergency Contact IDs start at 600001
            
            // Stream operators for serialization and debugging
            friend std::ostream& operator<<(std::ostream& os, const EmergencyContact& ec);
            friend std::istream& operator>>(std::istream& is, EmergencyContact& ec);
        };

    } // namespace Forms
} // namespace SilverClinic

#endif // EMERGENCYCONTACT_H
