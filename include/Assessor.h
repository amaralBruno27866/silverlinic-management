#ifndef ASSESSOR_H
#define ASSESSOR_H

#include <string>
#include "Address.h"
#include "DateTime.h"

using namespace std;
namespace SilverClinic {
  class Assessor {
    private:
    int m_assessor_id;
    string m_first_name;
    string m_last_name;
    string m_email;
    string m_phone;
    Address m_address;
    DateTime m_created_at;
    DateTime m_updated_at;

    public:
    // Constructors
    Assessor();
    Assessor(int assessor_id, const string& first_name, const string& last_name, const string& email, 
      const string& phone, const Address& address, const DateTime& created_at, const DateTime& updated_at);

    // Copy constructor
    Assessor(const Assessor& other);
    Assessor& operator=(const Assessor& other);

    // Destructor
    ~Assessor();

    // Getters
    int getAssessorId() const { return m_assessor_id; }
    string getFirstName() const { return m_first_name; }
    string getLastName() const { return m_last_name; }
    string getEmail() const { return m_email; }
    string getPhone() const { return m_phone; }
    Address getAddress() const { return m_address; }
    DateTime getCreatedAt() const { return m_created_at; }
    DateTime getUpdatedAt() const { return m_updated_at; }

    // Setters
    void setAssessorId(int assessor_id) { m_assessor_id = assessor_id; }
    void setFirstName(const string& first_name) { m_first_name = first_name; }
    void setLastName(const string& last_name) { m_last_name = last_name; }
    void setEmail(const string& email) { m_email = email; }
    void setPhone(const string& phone) { m_phone = phone; }
    void setAddress(const Address& address) { m_address = address; }
    void setCreatedAt(const DateTime& createdAt) { m_created_at = createdAt; }
    void setUpdatedAt(const DateTime& updatedAt) { m_updated_at = updatedAt; }

    // Timestamp management
    void updateTimestamp() { m_updated_at = DateTime::now(); }
    void setCreationTimestamp() { m_created_at = DateTime::now(); }
    void setTimestamps() { DateTime now = DateTime::now(); m_created_at = now; m_updated_at = now; }
    
    // Utility methods
    string getFullName() const;
    bool isValidEmail() const;
    bool isValidPhone() const;
    void displayInfo() const;
    
    // Static methods for ID management
    static int getNextAssessorId();
    static void resetIdCounter();
    static const int ID_PREFIX = 100000; // Assessor IDs start at 100001
    
    // Stream operators
    friend ostream& operator<<(ostream& os, const Assessor& assessor);
    friend istream& operator>>(istream& is, Assessor& assessor);
  };
  
  // Stream operators declarations
  ostream& operator<<(ostream& os, const Assessor& assessor);
  istream& operator>>(istream& is, Assessor& assessor);
}

#endif // ASSESSOR_H