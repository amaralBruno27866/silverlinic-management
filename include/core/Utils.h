#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <algorithm>
#include <iostream>
#include "utils/StructuredLogger.h"
#include <cmath>

using namespace std;
namespace utils {

    constexpr int DEFAULT_WINDOW_WIDTH = 1280;
    constexpr int DEFAULT_WINDOW_HEIGHT = 720;
    constexpr int DEFAULT_FORM_WIDTH = 980;
    constexpr int DEFAULT_FORM_HEIGHT = 620;

    string toUpper(const string& str);
    string removeAccents(const string& str);
    string trim(const string& str);
    string removeSpaces(const string& str);
    bool isValidEmail(const string& email);
    bool isValidPhoneNumber(const string& phone);
    string capitalizeWords(const string& str);
    string replaceAll(string str, const string& from, const string& to);
    
    // === NOVAS FUNÇÕES PARA SISTEMA DE CLÍNICA ===
    
    // Validações específicas para sistema médico
    bool isValidCanadianPostalCode(const string& postalCode);
    bool isValidCanadianPhoneNumber(const string& phone);
    bool isValidSIN(const string& sin); // Social Insurance Number
    bool isValidHealthCard(const string& healthCard);
    
    // Formatação de dados
    string formatCanadianPostalCode(const string& postalCode);
    string formatCanadianPhoneNumber(const string& phone);
    string formatSIN(const string& sin);
    string formatCurrency(double amount);
    
    // Utilitários de string específicos
    string generateRandomId(const string& prefix, int length = 6);
    string maskSensitiveData(const string& data, int visibleChars = 4);
    bool isNumeric(const string& str);
    bool isAlpha(const string& str);
    bool isAlphaNumeric(const string& str);
    
    // Conversões seguras
    int safeStringToInt(const string& str, int defaultValue = 0);
    double safeStringToDouble(const string& str, double defaultValue = 0.0);
    
    // Utilitários de data/hora
    string getCurrentTimestamp();
    string formatDuration(int seconds);
    bool isWeekend(int dayOfWeek);
    bool isBusinessHours(int hour, int minute = 0);

    template<typename T>
    void reloadList(T* obj, void (T::* method)()) {
        if (obj && method) {
            (obj->*method)();
        }
    }

    template<typename T>
    void showForm(T* form, int width = utils::DEFAULT_FORM_WIDTH, int height = utils::DEFAULT_FORM_HEIGHT) {
        if (!form) return;
        form->setFixedSize(width, height);
        form->show();
    }
    
    // === NOVOS TEMPLATES ÚTEIS ===
    
    // Template para validação de containers
    template<typename Container>
    bool isEmpty(const Container& container) {
        return container.empty();
    }
    
    // Template para encontrar elemento em container
    template<typename Container, typename T>
    bool contains(const Container& container, const T& value) {
        return std::find(container.begin(), container.end(), value) != container.end();
    }
    
    // Template para conversão segura
    template<typename T>
    string toString(const T& value) {
        return std::to_string(value);
    }
    
    // Declarações de especializações (implementadas em Utils.cpp)
    template<>
    string toString<string>(const string& value);
    
    template<>
    string toString<const char*>(const char* const& value);
    
    // Template para limpeza de ponteiros
    template<typename T>
    void safeDelete(T*& ptr) {
        if (ptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
    
    // Template para comparação com tolerância (útil para doubles)
    template<typename T>
    bool isEqual(const T& a, const T& b, const T& tolerance = static_cast<T>(0.001)) {
        return std::abs(a - b) <= tolerance;
    }
    
    // Template para clamping de valores
    template<typename T>
    T clampValue(const T& value, const T& min, const T& max) {
        return (value < min) ? min : (value > max) ? max : value;
    }
    
    // Template para logging simples
    template<typename T>
    void logMessage(const string& level, const T& message) {
        // Legacy shim -> structured logging
        utils::LogLevel lvl = utils::LogLevel::INFO;
        if(level == "ERROR") lvl = utils::LogLevel::ERROR; else if(level == "WARNING" || level=="WARN") lvl = utils::LogLevel::WARN; else if(level=="DEBUG") lvl = utils::LogLevel::DEBUG; else if(level=="TRACE") lvl = utils::LogLevel::TRACE;
        utils::LogEventContext ctx{"LEGACY","log", std::nullopt, std::nullopt, std::nullopt};
        std::ostringstream oss; oss << message; // generic stringify
        utils::logStructured(lvl, ctx, oss.str());
    }
    
    // === NORMALIZAÇÃO PARA BANCO DE DADOS ===
    
    // Template principal para normalização de dados do banco
    template<typename T>
    string normalizeForDatabase(const T& value) {
        string str = toString(value);
        str = trim(str);
        str = toUpper(str);
        return str;
    }
    
    // Declarações de especializações (implementadas em Utils.cpp)
    template<>
    string normalizeForDatabase<string>(const string& value);
    
    template<>
    string normalizeForDatabase<const char*>(const char* const& value);
    
    // Função auxiliar para literais de string
    string normalizeForDatabase(const char* value);
    
    // Funções específicas para campos do banco
    string normalizePostalCode(const string& postalCode);
    string normalizeProvince(const string& province);
    string normalizeSIN(const string& sin);
    string normalizeHealthCard(const string& healthCard);
    string normalizePhoneNumber(const string& phone);
    string normalizeName(const string& name);
    string normalizeCity(const string& city);
    string normalizeAddress(const string& address);
}

#endif // UTILS_H
