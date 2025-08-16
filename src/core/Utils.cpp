#include "core/Utils.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <regex>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;
namespace utils {

	std::string toUpper(const std::string& str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
		return result;
	}

	std::string removeAccents(const std::string& str)
	{
		std::string result = str;
		
		// Map of accented characters to their non-accented equivalents
		const std::vector<std::pair<std::string, char>> accentMap = {
			{"À", 'A'}, {"Á", 'A'}, {"Â", 'A'}, {"Ã", 'A'}, {"Ä", 'A'}, {"Å", 'A'},
			{"à", 'A'}, {"á", 'A'}, {"â", 'A'}, {"ã", 'A'}, {"ä", 'A'}, {"å", 'A'},
			{"È", 'E'}, {"É", 'E'}, {"Ê", 'E'}, {"Ë", 'E'},
			{"è", 'E'}, {"é", 'E'}, {"ê", 'E'}, {"ë", 'E'},
			{"Ì", 'I'}, {"Í", 'I'}, {"Î", 'I'}, {"Ï", 'I'},
			{"ì", 'I'}, {"í", 'I'}, {"î", 'I'}, {"ï", 'I'},
			{"Ò", 'O'}, {"Ó", 'O'}, {"Ô", 'O'}, {"Õ", 'O'}, {"Ö", 'O'},
			{"ò", 'O'}, {"ó", 'O'}, {"ô", 'O'}, {"õ", 'O'}, {"ö", 'O'},
			{"Ù", 'U'}, {"Ú", 'U'}, {"Û", 'U'}, {"Ü", 'U'},
			{"ù", 'U'}, {"ú", 'U'}, {"û", 'U'}, {"ü", 'U'},
			{"Ç", 'C'}, {"ç", 'C'},
			{"Ñ", 'N'}, {"ñ", 'N'}
		};
		
		for (const auto& pair : accentMap) {
			size_t pos = 0;
			while ((pos = result.find(pair.first, pos)) != std::string::npos) {
				result.replace(pos, pair.first.length(), 1, pair.second);
				pos += 1;
			}
		}
		
		return result;
	}

	std::string trim(const std::string& str)
	{
		auto start = str.begin();
		while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) ++start;
		auto end = str.end();
		do {
			--end;
		} while (end != start && std::isspace(static_cast<unsigned char>(*end)));
		return (start <= end) ? std::string(start, end + 1) : std::string();
	}

	std::string removeSpaces(const std::string& str)
	{
		std::string result;
		for (char c : str) {
			if (!std::isspace(static_cast<unsigned char>(c)))
				result += c;
		}
		return result;
	}

	bool isValidEmail(const std::string& email)
	{
		static const std::regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
		return std::regex_match(email, pattern);
	}

	bool isValidPhoneNumber(const std::string& phone)
	{
		std::string digits;
		for (char c : phone) {
			if (std::isdigit(static_cast<unsigned char>(c)))
				digits += c;
		}
		return digits.length() == 10 || digits.length() == 11;
	}

	std::string capitalizeWords(const std::string& str)
	{
		std::string result = str;
		bool newWord = true;
		for (size_t i = 0; i < result.length(); ++i) {
			if (std::isspace(static_cast<unsigned char>(result[i]))) {
				newWord = true;
			} else if (newWord) {
				result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
				newWord = false;
			} else {
				result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
			}
		}
		return result;
	}

	std::string replaceAll(std::string str, const std::string& from, const std::string& to)
	{
		if (from.empty()) return str;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return str;
	}

	// === IMPLEMENTAÇÃO DAS NOVAS FUNÇÕES ===

	bool isValidCanadianPostalCode(const std::string& postalCode)
	{
		// Formato canadense: A1A 1A1 ou A1A1A1
		// Letras excluídas: D, F, I, O, Q, U
		static const std::regex pattern(R"(^[ABCEGHJKLMNPRSTVWXYZ]\d[ABCEGHJKLMNPRSTVWXYZ]\s?\d[ABCEGHJKLMNPRSTVWXYZ]\d$)");
		return std::regex_match(postalCode, pattern);
	}

	bool isValidCanadianPhoneNumber(const std::string& phone)
	{
		std::string digits;
		for (char c : phone) {
			if (std::isdigit(static_cast<unsigned char>(c))) {
				digits += c;
			}
		}
		
		// Aceita 10 dígitos (local) ou 11 dígitos (com código do país 1)
		if (digits.length() == 10) {
			return true;
		} else if (digits.length() == 11 && digits[0] == '1') {
			return true;
		}
		return false;
	}

	bool isValidSIN(const std::string& sin)
	{
		std::string digits = removeSpaces(sin);
		std::replace(digits.begin(), digits.end(), '-', ' ');
		digits = removeSpaces(digits);
		
		if (digits.length() != 9) return false;
		
		// Algoritmo de validação do SIN (Luhn-like)
		int sum = 0;
		for (int i = 0; i < 9; i++) {
			if (!std::isdigit(digits[i])) return false;
			int digit = digits[i] - '0';
			
			if (i % 2 == 1) { // Posições pares (1, 3, 5, 7)
				digit *= 2;
				if (digit > 9) digit = digit / 10 + digit % 10;
			}
			sum += digit;
		}
		
		return sum % 10 == 0;
	}

	bool isValidHealthCard(const std::string& healthCard)
	{
		std::string digits = removeSpaces(healthCard);
		return digits.length() >= 9 && digits.length() <= 12 && isNumeric(digits);
	}

	std::string formatCanadianPostalCode(const std::string& postalCode)
	{
		std::string clean = toUpper(removeSpaces(postalCode));
		if (clean.length() == 6) {
			return clean.substr(0, 3) + " " + clean.substr(3, 3);
		}
		return postalCode;
	}

	std::string formatCanadianPhoneNumber(const std::string& phone)
	{
		std::string digits;
		for (char c : phone) {
			if (std::isdigit(static_cast<unsigned char>(c))) {
				digits += c;
			}
		}
		
		if (digits.length() == 10) {
			return "(" + digits.substr(0, 3) + ") " + digits.substr(3, 3) + "-" + digits.substr(6, 4);
		} else if (digits.length() == 11 && digits[0] == '1') {
			return "+1 (" + digits.substr(1, 3) + ") " + digits.substr(4, 3) + "-" + digits.substr(7, 4);
		}
		
		return phone;
	}

	std::string formatSIN(const std::string& sin)
	{
		std::string digits = removeSpaces(sin);
		std::replace(digits.begin(), digits.end(), '-', ' ');
		digits = removeSpaces(digits);
		
		if (digits.length() == 9) {
			return digits.substr(0, 3) + "-" + digits.substr(3, 3) + "-" + digits.substr(6, 3);
		}
		return sin;
	}

	std::string formatCurrency(double amount)
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << "$" << amount;
		return oss.str();
	}

	std::string generateRandomId(const std::string& prefix, int length)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<> dis(0, 35);
		
		const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::string result = prefix;
		
		for (int i = 0; i < length; ++i) {
			result += chars[dis(gen)];
		}
		
		return result;
	}

	std::string maskSensitiveData(const std::string& data, int visibleChars)
	{
		if (data.length() <= static_cast<size_t>(visibleChars)) {
			return std::string(data.length(), '*');
		}

		std::string result = data.substr(0, static_cast<size_t>(visibleChars));
		result += std::string(data.length() - static_cast<size_t>(visibleChars), '*');
		return result;
	}

	bool isNumeric(const std::string& str)
	{
		if (str.empty()) return false;
		for (char c : str) {
			if (!std::isdigit(static_cast<unsigned char>(c))) {
				return false;
			}
		}
		return true;
	}

	bool isAlpha(const std::string& str)
	{
		if (str.empty()) return false;
		for (char c : str) {
			if (!std::isalpha(static_cast<unsigned char>(c))) {
				return false;
			}
		}
		return true;
	}

	bool isAlphaNumeric(const std::string& str)
	{
		if (str.empty()) return false;
		for (char c : str) {
			if (!std::isalnum(static_cast<unsigned char>(c))) {
				return false;
			}
		}
		return true;
	}

	int safeStringToInt(const std::string& str, int defaultValue)
	{
		try {
			return std::stoi(str);
		} catch (const std::exception&) {
			return defaultValue;
		}
	}

	double safeStringToDouble(const std::string& str, double defaultValue)
	{
		try {
			return std::stod(str);
		} catch (const std::exception&) {
			return defaultValue;
		}
	}

	std::string getCurrentTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto tm = *std::localtime(&time_t);
		
		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		return oss.str();
	}

	std::string formatDuration(int seconds)
	{
		int hours = seconds / 3600;
		int minutes = (seconds % 3600) / 60;
		int secs = seconds % 60;
		
		std::ostringstream oss;
		if (hours > 0) {
			oss << hours << "h ";
		}
		if (minutes > 0 || hours > 0) {
			oss << minutes << "m ";
		}
		oss << secs << "s";
		
		return oss.str();
	}

	bool isWeekend(int dayOfWeek)
	{
		// 0 = Domingo, 6 = Sábado
		return dayOfWeek == 0 || dayOfWeek == 6;
	}

	bool isBusinessHours(int hour, int minute)
	{
		// Horário comercial: 8:00 às 18:00
		if (hour < 8 || hour > 18) return false;
		if (hour == 18 && minute > 0) return false;
		return true;
	}

	// === IMPLEMENTAÇÕES DE NORMALIZAÇÃO PARA BANCO DE DADOS ===

	string normalizePostalCode(const string& postalCode) {
		string normalized = removeSpaces(postalCode);
		normalized = toUpper(normalized);
		return normalized;
	}

	string normalizeProvince(const string& province) {
		string normalized = trim(province);
		normalized = toUpper(normalized);
		return normalized;
	}

	string normalizeSIN(const string& sin) {
		string normalized = removeSpaces(sin);
		// Remove hífens e espaços, mantém apenas números
		string result;
		for (char c : normalized) {
			if (isdigit(c)) {
				result += c;
			}
		}
		return result;
	}

	string normalizeHealthCard(const string& healthCard) {
		string normalized = removeSpaces(healthCard);
		normalized = toUpper(normalized);
		return normalized;
	}

	string normalizePhoneNumber(const string& phone) {
		string normalized;
		// Mantém apenas dígitos
		for (char c : phone) {
			if (isdigit(c)) {
				normalized += c;
			}
		}
		return normalized;
	}

	string normalizeName(const string& name) {
		string normalized = trim(name);
		normalized = removeAccents(normalized);
		normalized = toUpper(normalized);
		return normalized;
	}

	string normalizeCity(const string& city) {
		string normalized = trim(city);
		normalized = removeAccents(normalized);
		normalized = toUpper(normalized);
		return normalized;
	}

	string normalizeAddress(const string& address) {
		string normalized = trim(address);
		normalized = removeAccents(normalized);
		normalized = toUpper(normalized);
		return normalized;
	}

	// === IMPLEMENTAÇÕES DE ESPECIALIZAÇÕES DE TEMPLATES ===

	template<>
	string toString<string>(const string& value) {
		return value;
	}

	template<>
	string toString<const char*>(const char* const& value) {
		return string(value);
	}

	template<>
	string normalizeForDatabase<string>(const string& value) {
		string str = trim(value);
		str = toUpper(str);
		return str;
	}

	template<>
	string normalizeForDatabase<const char*>(const char* const& value) {
		string str = trim(string(value));
		str = toUpper(str);
		return str;
	}

	string normalizeForDatabase(const char* value) {
		string str = trim(string(value));
		str = toUpper(str);
		return str;
	}
}