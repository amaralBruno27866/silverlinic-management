#include "core/Utils.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace utils;

void testNewUtilityFunctions() {
    cout << "\n" << string(60, '=') << endl;
    cout << "        TESTING NEW UTILITY FUNCTIONS" << endl;
    cout << string(60, '=') << endl;
    
    // Testando validações canadenses
    cout << "\n1. Canadian Validations:" << endl;
    cout << "   Postal Code 'K1A 0A6': " << (isValidCanadianPostalCode("K1A 0A6") ? "Valid" : "Invalid") << endl;
    cout << "   Postal Code 'M5V3A8': " << (isValidCanadianPostalCode("M5V3A8") ? "Valid" : "Invalid") << endl;
    cout << "   Phone '416-555-0123': " << (isValidCanadianPhoneNumber("416-555-0123") ? "Valid" : "Invalid") << endl;
    cout << "   SIN '123-456-789': " << (isValidSIN("123-456-789") ? "Valid" : "Invalid") << endl;
    
    // Testando formatação
    cout << "\n2. Formatting:" << endl;
    cout << "   Postal Code formatted: " << formatCanadianPostalCode("k1a0a6") << endl;
    cout << "   Phone formatted: " << formatCanadianPhoneNumber("4165550123") << endl;
    cout << "   Currency: " << formatCurrency(1234.56) << endl;
    cout << "   SIN formatted: " << formatSIN("123456789") << endl;
    
    // Testando utilitários
    cout << "\n3. Utilities:" << endl;
    cout << "   Random ID: " << generateRandomId("PATIENT_", 6) << endl;
    cout << "   Masked SIN: " << maskSensitiveData("123456789", 3) << endl;
    cout << "   Is '12345' numeric: " << (isNumeric("12345") ? "Yes" : "No") << endl;
    cout << "   Is 'ABC123' alphanumeric: " << (isAlphaNumeric("ABC123") ? "Yes" : "No") << endl;
    
    // Testando conversões seguras
    cout << "\n4. Safe Conversions:" << endl;
    cout << "   Safe int conversion '123': " << safeStringToInt("123") << endl;
    cout << "   Safe int conversion 'abc': " << safeStringToInt("abc", -1) << endl;
    cout << "   Safe double conversion '123.45': " << safeStringToDouble("123.45") << endl;
    
    // Testando templates
    cout << "\n5. Templates:" << endl;
    vector<int> numbers = {1, 2, 3, 4, 5};
    cout << "   Vector contains 3: " << (contains(numbers, 3) ? "Yes" : "No") << endl;
    cout << "   Vector is empty: " << (isEmpty(numbers) ? "Yes" : "No") << endl;
    cout << "   Clamp 15 between 1-10: " << clampValue(15, 1, 10) << endl;
    cout << "   Double equality (1.0 vs 1.001): " << (isEqual(1.0, 1.001, 0.01) ? "Equal" : "Different") << endl;
    
    // Testando data/hora
    cout << "\n6. Date/Time Utilities:" << endl;
    cout << "   Current timestamp: " << getCurrentTimestamp() << endl;
    cout << "   Format 3665 seconds: " << formatDuration(3665) << endl;
    cout << "   Is Saturday weekend: " << (isWeekend(6) ? "Yes" : "No") << endl;
    cout << "   Is 14:30 business hours: " << (isBusinessHours(14, 30) ? "Yes" : "No") << endl;
    
    // Testando logging
    cout << "\n7. Logging:" << endl;
    logMessage("INFO", "Sistema iniciado com sucesso");
    logMessage("WARNING", "Baixo espaço em disco");
    logMessage("ERROR", "Falha na conexão com banco de dados");
}

int main() {
    testNewUtilityFunctions();
    return 0;
}
