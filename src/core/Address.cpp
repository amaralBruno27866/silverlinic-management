#include "core/Address.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <iomanip>

using namespace std;

namespace SilverClinic {
    // Static variable for ID counter
    static int address_id_counter = 0;
    
    // Default constructor
    Address::Address() : m_address_id(getNextAddressId()), m_user_key(0), m_street(""), m_city(""), 
                         m_province(""), m_postal_code(""), m_created_at(DateTime::now()), m_updated_at(DateTime::now()) {}

    // Parameterized constructor
    Address::Address(int address_id, int user_key, const string& street, const string& city,
                     const string& province, const string& postal_code)
        : m_address_id(address_id), m_user_key(user_key), 
          m_street(utils::normalizeAddress(street)), m_city(utils::normalizeCity(city)),
          m_province(utils::normalizeProvince(province)), m_postal_code(utils::normalizePostalCode(postal_code)), 
          m_created_at(DateTime::now()), m_updated_at(DateTime::now()) {}

    // Parameterized constructor with timestamps
    Address::Address(int address_id, int user_key, const string& street, const string& city,
                     const string& province, const string& postal_code, 
                     const DateTime& created_at, const DateTime& updated_at)
        : m_address_id(address_id), m_user_key(user_key), 
          m_street(utils::normalizeAddress(street)), m_city(utils::normalizeCity(city)),
          m_province(utils::normalizeProvince(province)), m_postal_code(utils::normalizePostalCode(postal_code)), 
          m_created_at(created_at), m_updated_at(updated_at) {}

    // Destructor
    Address::~Address() {}

    // Copy constructor
    Address::Address(const Address& other) 
        : m_address_id(other.m_address_id), m_user_key(other.m_user_key), 
          m_street(other.m_street), m_city(other.m_city), 
          m_province(other.m_province), m_postal_code(other.m_postal_code),
          m_created_at(other.m_created_at), m_updated_at(other.m_updated_at) {}

    // Assignment operator
    Address& Address::operator=(const Address& other) {
        if (this != &other) { // Self-assignment check
            m_address_id = other.m_address_id;
            m_user_key = other.m_user_key;
            m_street = other.m_street;
            m_city = other.m_city;
            m_province = other.m_province;
            m_postal_code = other.m_postal_code;
            m_created_at = other.m_created_at;
            m_updated_at = other.m_updated_at;
        }
        return *this;
    }

    // Validate Canadian postal code format (A1A 1A1)
    bool Address::isValidCanadianPostalCode() const {
        return isValidPostalCodeFormat(m_postal_code);
    }

    // Validate if province is Ontario
    bool Address::isValidOntarioProvince() const {
        return (m_province == "ON" || m_province == "Ontario");
    }

    // Get formatted address string
    string Address::getFormattedAddress() const {
        string formatted = m_street;
        if (!m_city.empty()) {
            formatted += "\n" + m_city;
        }
        if (!m_province.empty()) {
            formatted += ", " + m_province;
        }
        if (!m_postal_code.empty()) {
            formatted += " " + m_postal_code;
        }
        formatted += "\nCanada";
        return formatted;
    }

    // Display address information
    void Address::displayInfo() const {
        cout << "\n" << string(50, '=') << endl;
        cout << "ADDRESS INFORMATION" << endl;
        cout << string(50, '=') << endl;
        cout << "Address ID: " << m_address_id << endl;
        cout << "User Key: " << m_user_key << endl;
        cout << "Street: " << m_street << endl;
        cout << "City: " << m_city << endl;
        cout << "Province: " << m_province << endl;
        cout << "Postal Code: " << m_postal_code << endl;
        
        cout << "Created: " << m_created_at.toString() << endl;
        cout << "Updated: " << m_updated_at.toString() << endl;
        
        cout << "\nFormatted Address:" << endl;
        cout << getFormattedAddress() << endl;
        cout << string(50, '=') << endl;
    }

    // Static method to validate postal code format
    bool Address::isValidPostalCodeFormat(const string& postal_code) {
        // Canadian postal code format: A1A 1A1
        // Remove spaces and convert to uppercase for validation
        string clean_code = postal_code;
        clean_code.erase(remove(clean_code.begin(), clean_code.end(), ' '), clean_code.end());
        transform(clean_code.begin(), clean_code.end(), clean_code.begin(), ::toupper);
        
        // Must be exactly 6 characters
        if (clean_code.length() != 6) {
            return false;
        }
        
        // Pattern: Letter-Digit-Letter-Digit-Letter-Digit
        for (int i = 0; i < 6; i++) {
            if (i % 2 == 0) { // Even positions (0, 2, 4) should be letters
                if (!isalpha(clean_code[i])) {
                    return false;
                }
            } else { // Odd positions (1, 3, 5) should be digits
                if (!isdigit(clean_code[i])) {
                    return false;
                }
            }
        }
        
        return true;
    }

    // Static method to validate Canadian province codes
    bool Address::isValidProvinceCode(const string& province) {
        // Canadian provinces and territories
        vector<string> valid_provinces = {
            "AB", "Alberta",
            "BC", "British Columbia", 
            "MB", "Manitoba",
            "NB", "New Brunswick",
            "NL", "Newfoundland and Labrador",
            "NS", "Nova Scotia",
            "ON", "Ontario",
            "PE", "Prince Edward Island",
            "QC", "Quebec",
            "SK", "Saskatchewan",
            "NT", "Northwest Territories",
            "NU", "Nunavut",
            "YT", "Yukon"
        };
        
        return find(valid_provinces.begin(), valid_provinces.end(), province) != valid_provinces.end();
    }

    // Stream output operator
    ostream& operator<<(ostream& os, const Address& addr) {
        os << "Address ID: " << addr.getAddressId() << "\n"
           << "User Key: " << addr.getUserKey() << "\n"
           << "Street: " << addr.getStreet() << "\n"
           << "City: " << addr.getCity() << "\n"
           << "Province: " << addr.getProvince() << "\n"
           << "Postal Code: " << addr.getPostalCode() << "\n"
           << "Created: " << addr.getCreatedAt().toString() << "\n"
           << "Updated: " << addr.getUpdatedAt().toString();
        
        return os;
    }

    // Stream input operator
    istream& operator>>(istream& is, Address& addr) {
        string street, city, province, postal_code;
        
        cout << "Enter street address: ";
        getline(is, street);
        addr.setStreet(street);
        
        cout << "Enter city: ";
        getline(is, city);
        addr.setCity(city);
        
        cout << "Enter province (ON for Ontario): ";
        getline(is, province);
        addr.setProvince(province);
        
        cout << "Enter postal code (A1A 1A1 format): ";
        getline(is, postal_code);
        addr.setPostalCode(postal_code);
        
        // Validate postal code format
        if (!addr.isValidCanadianPostalCode()) {
            cout << "Warning: Invalid Canadian postal code format!" << endl;
        }
        
        // Validate province
        if (!Address::isValidProvinceCode(province)) {
            cout << "Warning: Invalid Canadian province code!" << endl;
        }
        
        return is;
    }
    
    // Static methods for ID management
    int Address::getNextAddressId() {
        return ID_PREFIX + (++address_id_counter);
    }
    
    void Address::resetIdCounter() {
        address_id_counter = 0;
    }

} // namespace SilverClinic