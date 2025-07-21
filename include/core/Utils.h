#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;
namespace utils {

    constexpr int DEFAULT_WINDOW_WIDTH = 1280;
    constexpr int DEFAULT_WINDOW_HEIGHT = 720;
    constexpr int DEFAULT_FORM_WIDTH = 980;
    constexpr int DEFAULT_FORM_HEIGHT = 620;

    string toUpper(const string& str);
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
        cout << "[" << getCurrentTimestamp() << "] [" << level << "] " << message << endl;
    }
}

#endif // UTILS_H
