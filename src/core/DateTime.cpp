#include "DateTime.h"
#include <iomanip>
#include <ctime>

using namespace std;
namespace SilverClinic {
    
    // Default constructor - current time
    DateTime::DateTime() : m_time_point(system_clock::now()) {}

    // Constructor with date/time components
    DateTime::DateTime(int year, int month, int day, int hour, int minute, int second) {
        tm timeStruct = {};
        timeStruct.tm_year = year - 1900;
        timeStruct.tm_mon = month - 1;
        timeStruct.tm_mday = day;
        timeStruct.tm_hour = hour;
        timeStruct.tm_min = minute;
        timeStruct.tm_sec = second;
        
        time_t time = mktime(&timeStruct);
        m_time_point = system_clock::from_time_t(time);
    }

    // Constructor from string
    DateTime::DateTime(const string& date_string) {
        *this = fromString(date_string);
    }

    // Static method to get current time
    DateTime DateTime::now() {
        return DateTime();
    }

    // Static method to parse string
    DateTime DateTime::fromString(const string& date_string) {
        istringstream ss(date_string);
        tm timeStruct = {};
        
        // Try different formats
        if (ss >> get_time(&timeStruct, "%Y-%m-%d %H:%M:%S")) {
            // Full datetime format
        } else {
            ss.clear();
            ss.str(date_string);
            if (ss >> get_time(&timeStruct, "%Y-%m-%d")) {
                // Date only format
            } else {
                ss.clear();
                ss.str(date_string);
                if (ss >> get_time(&timeStruct, "%d/%m/%Y")) {
                    // Canadian format
                }
            }
        }
        
        time_t time = mktime(&timeStruct);
        DateTime result;
        result.m_time_point = system_clock::from_time_t(time);
        return result;
    }

    // Getters
    int DateTime::getYear() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        return timeStruct->tm_year + 1900;
    }

    int DateTime::getMonth() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        return timeStruct->tm_mon + 1;
    }

    int DateTime::getDay() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        return timeStruct->tm_mday;
    }

    int DateTime::getHour() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        return timeStruct->tm_hour;
    }

    int DateTime::getMinute() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        return timeStruct->tm_min;
    }

    int DateTime::getSecond() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        return timeStruct->tm_sec;
    }

    // Formatters
    string DateTime::toString() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        
        ostringstream oss;
        oss << put_time(timeStruct, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    string DateTime::toDateString() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        
        ostringstream oss;
        oss << put_time(timeStruct, "%Y-%m-%d");
        return oss.str();
    }

    string DateTime::toTimeString() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        
        ostringstream oss;
        oss << put_time(timeStruct, "%H:%M:%S");
        return oss.str();
    }

    string DateTime::toCanadianFormat() const {
        time_t time = system_clock::to_time_t(m_time_point);
        tm* timeStruct = localtime(&time);
        
        ostringstream oss;
        oss << put_time(timeStruct, "%d/%m/%Y");
        return oss.str();
    }

    // Operators
    bool DateTime::operator==(const DateTime& other) const {
        return m_time_point == other.m_time_point;
    }

    bool DateTime::operator!=(const DateTime& other) const {
        return !(*this == other);
    }

    bool DateTime::operator<(const DateTime& other) const {
        return m_time_point < other.m_time_point;
    }

    bool DateTime::operator<=(const DateTime& other) const {
        return m_time_point <= other.m_time_point;
    }

    // Age calculation
    int DateTime::getAgeInYears() const {
        auto now = system_clock::now();
        auto duration = now - m_time_point;
        auto hours = duration_cast<chrono::hours>(duration).count();
        auto days = hours / 24;
        return static_cast<int>(days / 365.25); // Approximate age in years
    }

    int DateTime::getDaysFromNow() const {
        auto now = system_clock::now();
        auto duration = now - m_time_point;
        auto hours = duration_cast<chrono::hours>(duration).count();
        return static_cast<int>(hours / 24);
    }

    // Validation
    bool DateTime::isValid() const {
        return m_time_point.time_since_epoch().count() > 0;
    }

    bool DateTime::isValidDate(int year, int month, int day) {
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        
        // Check days in month
        int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        
        // Check for leap year
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (month == 2 && isLeapYear) {
            return day <= 29;
        }
        
        return day <= daysInMonth[month - 1];
    }

}