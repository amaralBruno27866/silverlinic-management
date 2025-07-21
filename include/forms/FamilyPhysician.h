#ifndef FAMILYPHYSICIAN_H
#define FAMILYPHYSICIAN_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"
#include "core/Address.h"

using namespace std;

namespace SilverClinic {
    namespace Forms {

        class FamilyPhysician {
        private:
            static const int MAX_FULL_NAME_LENGTH = 100;
            static const string FORM_TYPE;     // Form type identifier: "FPH"
            
            // Primary attributes
            int m_fp_id;                  // Family Physician ID (500000+)
            int m_client_id;             // Foreign key to client
            string m_type;               // Form type (default: "FPH")
            string m_fp_full_name;       // Nome completo do médico
            string m_fp_phone;           // Telefone
            string m_fp_email;           // Email
            Address m_fp_address;        // Endereço completo
            DateTime m_fp_createdAt;     // Data de criação
            DateTime m_fp_updatedAt;     // Data de atualização
            
            static int family_physician_id_counter;
            
            void setTimestamps();
            void updateTimestamp();
            int getNextFamilyPhysicianId();

        public:
            // Constructors
            FamilyPhysician();
            FamilyPhysician(int client_id, const string& fp_full_name, const string& fp_phone = "");
            FamilyPhysician(int fp_id, int client_id, const string& fp_full_name, 
                        const string& fp_phone, const string& fp_email,
                        const Address& fp_address, const DateTime& fp_createdAt, 
                        const DateTime& fp_updatedAt);
            
            // Destructor
            ~FamilyPhysician() = default;
            
            // Getters
            int getFamilyPhysicianId() const { return m_fp_id; }
            int getClientId() const { return m_client_id; }
            string getType() const { return m_type; }
            string getFpFullName() const { return m_fp_full_name; }
            string getFpPhone() const { return m_fp_phone; }
            string getFpEmail() const { return m_fp_email; }
            Address getFpAddress() const { return m_fp_address; }
            DateTime getFpCreatedAt() const { return m_fp_createdAt; }
            DateTime getFpUpdatedAt() const { return m_fp_updatedAt; }
            
            // Setters with validation
            void setClientId(int client_id) { m_client_id = client_id; updateTimestamp(); }
            void setFpFullName(const string& fp_full_name);
            void setFpPhone(const string& fp_phone);
            void setFpEmail(const string& fp_email);
            void setFpAddress(const Address& fp_address) { m_fp_address = fp_address; updateTimestamp(); }
            void setFpCreatedAt(const DateTime& fp_createdAt) { m_fp_createdAt = fp_createdAt; }
            void setFpUpdatedAt(const DateTime& fp_updatedAt) { m_fp_updatedAt = fp_updatedAt; }
            
            // Validation methods
            bool isValidClientId(int client_id) const;
            bool isValidFpFullName(const string& fp_full_name) const;
            bool isValidData() const;
            
            // Display methods
            void displayInfo() const;
            void displaySummary() const;
            string toString() const;
            
        // Static methods for ID management
        static void resetIdCounter();
        static int getNextId();
        
        // Stream operators for serialization and debugging
        friend std::ostream& operator<<(std::ostream& os, const FamilyPhysician& fp);
        friend std::istream& operator>>(std::istream& is, FamilyPhysician& fp);
    };

    } // namespace Forms
} // namespace SilverClinic

#endif // FAMILYPHYSICIAN_H
