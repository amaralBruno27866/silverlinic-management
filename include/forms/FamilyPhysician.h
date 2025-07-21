#ifndef FAMILYPHYSICIAN_H
#define FAMILYPHYSICIAN_H

#include <string>
#include "core/DateTime.h"
#include "core/Utils.h"

using namespace std;

namespace SilverClinic {
namespace Forms {

class FamilyPhysician {
private:
    static const int MAX_DOCTOR_NAME_LENGTH = 100;
    static const int MAX_CLINIC_NAME_LENGTH = 150;
    static const int MAX_NOTES_LENGTH = 500;
    
    int m_family_physician_id;
    int m_client_id;              // Foreign key to client
    string m_doctor_name;         // Nome do médico
    string m_clinic_name;         // Nome da clínica
    string m_phone;              // Telefone
    string m_email;              // Email
    string m_address;            // Endereço
    string m_postal_code;        // Código postal
    string m_city;               // Cidade
    string m_province;           // Província
    string m_specialization;     // Especialização
    string m_notes;              // Notas adicionais
    DateTime m_created_at;       // Data de criação
    DateTime m_updated_at;       // Data de atualização
    
    static int family_physician_id_counter;
    
    void setTimestamps();
    void updateTimestamp();
    int getNextFamilyPhysicianId();

public:
    // Constructors
    FamilyPhysician();
    FamilyPhysician(int client_id, const string& doctor_name, const string& clinic_name);
    FamilyPhysician(int family_physician_id, int client_id, const string& doctor_name, 
                   const string& clinic_name, const string& phone, const string& email,
                   const string& address, const string& postal_code, const string& city,
                   const string& province, const string& specialization, const string& notes,
                   const DateTime& created_at, const DateTime& updated_at);
    
    // Destructor
    ~FamilyPhysician() = default;
    
    // Getters
    int getFamilyPhysicianId() const { return m_family_physician_id; }
    int getClientId() const { return m_client_id; }
    string getDoctorName() const { return m_doctor_name; }
    string getClinicName() const { return m_clinic_name; }
    string getPhone() const { return m_phone; }
    string getEmail() const { return m_email; }
    string getAddress() const { return m_address; }
    string getPostalCode() const { return m_postal_code; }
    string getCity() const { return m_city; }
    string getProvince() const { return m_province; }
    string getSpecialization() const { return m_specialization; }
    string getNotes() const { return m_notes; }
    DateTime getCreatedAt() const { return m_created_at; }
    DateTime getUpdatedAt() const { return m_updated_at; }
    
    // Setters with validation
    void setClientId(int client_id) { m_client_id = client_id; updateTimestamp(); }
    void setDoctorName(const string& doctor_name);
    void setClinicName(const string& clinic_name);
    void setPhone(const string& phone);
    void setEmail(const string& email);
    void setAddress(const string& address) { m_address = address; updateTimestamp(); }
    void setPostalCode(const string& postal_code);
    void setCity(const string& city) { m_city = city; updateTimestamp(); }
    void setProvince(const string& province) { m_province = province; updateTimestamp(); }
    void setSpecialization(const string& specialization) { m_specialization = specialization; updateTimestamp(); }
    void setNotes(const string& notes);
    void setCreatedAt(const DateTime& created_at) { m_created_at = created_at; }
    void setUpdatedAt(const DateTime& updated_at) { m_updated_at = updated_at; }
    
    // Validation methods
    bool isValidClientId(int client_id) const;
    bool isValidDoctorName(const string& doctor_name) const;
    bool isValidClinicName(const string& clinic_name) const;
    bool isValidNotes(const string& notes) const;
    bool isValidData() const;
    
    // Display methods
    void displayInfo() const;
    void displaySummary() const;
    string toString() const;
    
    // Static methods for ID management
    static void resetIdCounter();
    static int getNextId();
};

} // namespace Forms
} // namespace SilverClinic

#endif // FAMILYPHYSICIAN_H
