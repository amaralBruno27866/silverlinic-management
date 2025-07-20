#include "Client.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

using namespace utils;

namespace SilverClinic {

    // Static variable for ID management
    static int client_id_counter = 1;

    // Constructors
    Client::Client() {
        m_client_id = getNextClientId();
        m_first_name = "";
        m_last_name = "";
        m_email = "";
        m_phone = "";
        m_date_of_birth = "";
        m_address = Address();
        setTimestamps();
    }

    Client::Client(int client_id, const string& first_name, const string& last_name, const string& email, 
                   const string& phone, const string& date_of_birth, const Address& address, 
                   const DateTime& created_at, const DateTime& updated_at) {
        m_client_id = client_id;
        m_first_name = first_name;
        m_last_name = last_name;
        m_email = email;
        m_phone = phone;
        m_date_of_birth = date_of_birth;
        m_address = address;
        m_created_at = created_at;
        m_updated_at = updated_at;
    }

    // Copy constructor
    Client::Client(const Client& other) {
        m_client_id = other.m_client_id;
        m_first_name = other.m_first_name;
        m_last_name = other.m_last_name;
        m_email = other.m_email;
        m_phone = other.m_phone;
        m_date_of_birth = other.m_date_of_birth;
        m_address = other.m_address;
        m_created_at = other.m_created_at;
        m_updated_at = other.m_updated_at;
    }

    Client& Client::operator=(const Client& other) {
        if (this != &other) {
            m_client_id = other.m_client_id;
            m_first_name = other.m_first_name;
            m_last_name = other.m_last_name;
            m_email = other.m_email;
            m_phone = other.m_phone;
            m_date_of_birth = other.m_date_of_birth;
            m_address = other.m_address;
            m_created_at = other.m_created_at;
            m_updated_at = other.m_updated_at;
        }
        return *this;
    }

    // Destructor
    Client::~Client() {
        // Nothing specific to destroy
    }

    // Utility methods
    string Client::getFullName() const {
        return m_first_name + " " + m_last_name;
    }

    bool Client::isValidEmail() const {
        return utils::isValidEmail(m_email);
    }

    bool Client::isValidPhone() const {
        return utils::isValidCanadianPhoneNumber(m_phone);
    }

    int Client::getAge() const {
        if (m_date_of_birth.empty()) return 0;
        
        try {
            DateTime birth = DateTime::fromString(m_date_of_birth + " 00:00:00");
            return birth.getAgeInYears();
        } catch (...) {
            return 0;
        }
    }

    void Client::displayInfo() const {
        cout << string(50, '=') << endl;
        cout << "              CLIENT INFORMATION              " << endl;
        cout << string(50, '=') << endl;
        cout << "Client ID: " << m_client_id << endl;
        cout << "Name: " << getFullName() << endl;
        cout << "Email: " << m_email << endl;
        cout << "Phone: " << formatCanadianPhoneNumber(m_phone) << endl;
        cout << "Date of Birth: " << m_date_of_birth << endl;
        cout << "Age: " << getAge() << " years" << endl;
        cout << "Email Valid: " << (isValidEmail() ? "Yes" : "No") << endl;
        cout << "Phone Valid: " << (isValidPhone() ? "Yes" : "No") << endl;
        cout << "Created: " << m_created_at.toString() << endl;
        cout << "Updated: " << m_updated_at.toString() << endl;
        cout << "------------------------------------------------" << endl;
        cout << "Address Information:" << endl;
        cout << endl;
        m_address.displayInfo();
        cout << string(50, '=') << endl;
    }

    // Static methods for ID management
    int Client::getNextClientId() {
        return ID_PREFIX + client_id_counter++;
    }

    void Client::resetIdCounter() {
        client_id_counter = 1;
    }

    // Stream operators
    ostream& operator<<(ostream& os, const Client& client) {
        os << "Client ID: " << client.m_client_id << endl;
        os << "Name: " << client.getFullName() << endl;
        os << "Email: " << client.m_email << endl;
        os << "Phone: " << client.m_phone << endl;
        os << "Date of Birth: " << client.m_date_of_birth << endl;
        os << "Created: " << client.m_created_at.toString() << endl;
        os << "Updated: " << client.m_updated_at.toString() << endl;
        return os;
    }

    istream& operator>>(istream& is, Client& client) {
        cout << "Enter first name: ";
        is >> client.m_first_name;
        cout << "Enter last name: ";
        is >> client.m_last_name;
        cout << "Enter email: ";
        is >> client.m_email;
        cout << "Enter phone: ";
        is >> client.m_phone;
        cout << "Enter date of birth (YYYY-MM-DD): ";
        is >> client.m_date_of_birth;
        
        client.updateTimestamp();
        return is;
    }
}
