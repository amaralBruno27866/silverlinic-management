#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include "DateTime.h"
#include "Utils.h"

using namespace std;
namespace SilverClinic {
  class Address {
    private:
    int m_address_id;
    int m_user_key;
    string m_street;
    string m_city;
    string m_province;
    string m_postal_code;
    DateTime m_created_at;
    DateTime m_updated_at;

    public:
    // Constructors
    Address();
    Address(int address_id, int user_key, const string& street, const string& city, 
            const string& province, const string& postal_code);
    Address(int address_id, int user_key, const string& street, const string& city, 
            const string& province, const string& postal_code, 
            const DateTime& created_at, const DateTime& updated_at);

    // Copy constructor
    Address (const Address&);
    Address& operator=(const Address&);
    
    // Destructor
    ~Address();
    
    // Getters
    int getAddressId() const { return m_address_id; }
    int getUserKey() const { return m_user_key; }
    string getStreet() const { return m_street; }
    string getCity() const { return m_city; }
    string getProvince() const { return m_province; }
    string getPostalCode() const { return m_postal_code; }
    DateTime getCreatedAt() const { return m_created_at; }
    DateTime getUpdatedAt() const { return m_updated_at; }
    
    // Setters
    void setAddressId(int address_id) { m_address_id = address_id; }
    void setUserKey(int user_key) { m_user_key = user_key; }
    void setStreet(const string& street) { m_street = utils::normalizeAddress(street); }
    void setCity(const string& city) { m_city = utils::normalizeCity(city); }
    void setProvince(const string& province) { m_province = utils::normalizeProvince(province); }
    void setPostalCode(const string& postal_code) { m_postal_code = utils::normalizePostalCode(postal_code); }
    void setCreatedAt(const DateTime& created_at) { m_created_at = created_at; }
    void setUpdatedAt(const DateTime& updated_at) { m_updated_at = updated_at; }
    
    // Timestamp management
    void updateTimestamp() { m_updated_at = DateTime::now(); }
    void setCreationTimestamp() { m_created_at = DateTime::now(); }
    void setTimestamps() { DateTime now = DateTime::now(); m_created_at = now; m_updated_at = now; }
    
    // Utility methods
    bool isValidCanadianPostalCode() const;
    bool isValidOntarioProvince() const;
    string getFormattedAddress() const;
    void displayInfo() const;
    
    // Static methods for validation
    static bool isValidPostalCodeFormat(const string& postal_code);
    static bool isValidProvinceCode(const string& province);
    
    // Static methods for ID management
    static int getNextAddressId();
    static void resetIdCounter();
  };

  ostream& operator<<(ostream& os, const Address& addr);
  istream& operator>>(istream& is, Address& addr);
}

#endif // ADDRESS_H