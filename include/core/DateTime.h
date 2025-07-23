#ifndef DATETIME_H
#define DATETIME_H

#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace chrono;

namespace SilverClinic {
  class DateTime {
    private:
    system_clock::time_point m_time_point;

    public:
    // Constructos
    DateTime();
    DateTime(int year, int month, int day, int hour = 0,int minute = 0, int second = 0);
    DateTime(const string& date_string);

    // static methods
    static DateTime now();
    static DateTime fromString(const string& date_string);

    // Getters
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHour() const;
    int getMinute() const;
    int getSecond() const;

    // Formatters
    string toString() const;
    string toDateString() const;
    string toTimeString() const;
    string toCanadianFormat() const;

    // Operators
    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;

    // Age calculation
    int getAgeInYears() const;
    int getDaysFromNow() const;

    // Validation
    bool isValid() const;
    static bool isValidDate(int year, int month, int day);
  };
}

#endif // DATETIME_H