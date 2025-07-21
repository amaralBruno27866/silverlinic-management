#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Address.h"
#include "DateTime.h"

using namespace std;
namespace SilverClinic {
  class Client {
    private:
    int m_client_id;
    string m_first_name;
    string m_last_name;
    string m_email;
    string m_phone;
    string m_date_of_birth;
    Address m_address;
    DateTime m_created_at;
    DateTime m_updated_at;

    public:
    // Constructors
    Client();
    Client(int client_id, const string& first_name, const string& last_name, const string& email, 
           const string& phone, const string& date_of_birth, const Address& address, 
           const DateTime& created_at, const DateTime& updated_at);

    // Copy constructor
    Client(const Client& other);
    Client& operator=(const Client& other);

    // Destructor
    ~Client();

    // Getters
    int getClientId() const { return m_client_id; }
    string getFirstName() const { return m_first_name; }
    string getLastName() const { return m_last_name; }
    string getEmail() const { return m_email; }
    string getPhone() const { return m_phone; }
    string getDateOfBirth() const { return m_date_of_birth; }
    Address getAddress() const { return m_address; }
    DateTime getCreatedAt() const { return m_created_at; }
    DateTime getUpdatedAt() const { return m_updated_at; }

    // Setters
    void setClientId(int client_id) { m_client_id = client_id; }
    void setFirstName(const string& first_name) { m_first_name = first_name; }
    void setLastName(const string& last_name) { m_last_name = last_name; }
    void setEmail(const string& email) { m_email = email; }
    void setPhone(const string& phone) { m_phone = phone; }
    void setDateOfBirth(const string& date_of_birth) { m_date_of_birth = date_of_birth; }
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
    int getAge() const;
    void displayInfo() const;
    
    // Static methods for ID management
    static int getNextClientId();
    static void resetIdCounter();
    static const int ID_PREFIX = 300000; // Client IDs start at 300001
    
    // Stream operators
    friend ostream& operator<<(ostream& os, const Client& client);
    friend istream& operator>>(istream& is, Client& client);
  };
  
  // Stream operators declarations
  ostream& operator<<(ostream& os, const Client& client);
  istream& operator>>(istream& is, Client& client);
}

#endif // CLIENT_H
