#include "Address.h"
#include "DateTime.h"
#include "Assessor.h"
#include "Client.h"
#include "CaseProfile.h"
#include "Utils.h"
#include "forms/FamilyPhysician.h"
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <iomanip>
#include <limits>
#include <vector>

using namespace std;
using namespace SilverClinic;
using namespace utils;

// Função para executar SQL e verificar erros
bool executeSQLCommand(sqlite3* db, const string& sql, const string& description) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    
    if (result != SQLITE_OK) {
        cerr << "Error executing " << description << ": " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    
    logMessage("INFO", description + " executed successfully");
    return true;
}

// Função para criar tabelas do banco de dados
bool createDatabaseTables(sqlite3* db) {
    logMessage("INFO", "Creating database tables...");
    
    // Ativar foreign keys no SQLite
    if (!executeSQLCommand(db, "PRAGMA foreign_keys = ON;", "Foreign keys activation")) {
        return false;
    }
    
    // Tabela assessor (avaliador/médico)
    string createAssessorTable = R"(
        CREATE TABLE IF NOT EXISTS assessor(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    
    if (!executeSQLCommand(db, createAssessorTable, "Assessor table creation")) {
        return false;
    }
    
    // Tabela client (paciente)
    string createClientTable = R"(
        CREATE TABLE IF NOT EXISTS client(
            id INTEGER PRIMARY KEY,
            firstname TEXT NOT NULL,
            lastname TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            date_of_birth TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    
    if (!executeSQLCommand(db, createClientTable, "Client table creation")) {
        return false;
    }
    
    // Tabela case_profile
    string createCaseProfileTable = R"(
        CREATE TABLE IF NOT EXISTS case_profile(
            id INTEGER PRIMARY KEY,
            client_id INTEGER NOT NULL,
            assessor_id INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT 'Pending',
            notes TEXT CHECK(length(notes) <= 1500),
            created_at TEXT NOT NULL,
            closed_at TEXT,
            modified_at TEXT NOT NULL
        )
    )";
    
    if (!executeSQLCommand(db, createCaseProfileTable, "CaseProfile table creation")) {
        return false;
    }
    
    // Tabela address
    string createAddressTable = R"(
        CREATE TABLE IF NOT EXISTS address(
            id INTEGER PRIMARY KEY,
            user_key INTEGER NOT NULL,
            street TEXT,
            city TEXT,
            province TEXT,
            postal_code TEXT,
            created_at TEXT NOT NULL,
            modified_at TEXT NOT NULL
        )
    )";
    
    if (!executeSQLCommand(db, createAddressTable, "Address table creation")) {
        return false;
    }
    
    logMessage("INFO", "All database tables created successfully");
    return true;
}

// Função para inserir dados de exemplo
bool insertSampleData(sqlite3* db) {
    logMessage("INFO", "Inserting sample data...");
    
    string currentTime = getCurrentTimestamp();
    
    // Gerar IDs com os prefixos corretos
    int assessorId = Assessor::ID_PREFIX + 1; // 100001
    int clientId = Client::ID_PREFIX + 1;     // 300001
    int addressId1 = Address::ID_PREFIX + 1;  // 200001
    int addressId2 = Address::ID_PREFIX + 2;  // 200002
    
    // Inserir assessor de exemplo
    string insertAssessor = R"(
        INSERT OR IGNORE INTO assessor (id, firstname, lastname, phone, email, created_at, modified_at)
        VALUES ()" + to_string(assessorId) + R"(, 'Dr. Maria', 'Silva', '416-555-0123', 'maria.silva@silverclinic.ca', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAssessor, "Sample assessor insertion")) {
        return false;
    }
    
    // Inserir cliente de exemplo
    string insertClient = R"(
        INSERT OR IGNORE INTO client (id, firstname, lastname, phone, email, date_of_birth, created_at, modified_at)
        VALUES ()" + to_string(clientId) + R"(, 'João', 'Santos', '416-555-9876', 'joao.santos@email.com', '1985-03-15', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertClient, "Sample client insertion")) {
        return false;
    }
    
    // Inserir case profile de exemplo
    int caseProfileId = CaseProfile::ID_PREFIX + 1;
    string insertCaseProfile = R"(
        INSERT OR IGNORE INTO case_profile (id, client_id, assessor_id, status, notes, created_at, modified_at)
        VALUES ()" + to_string(caseProfileId) + ", " + to_string(clientId) + ", " + to_string(assessorId) + R"(, 'Pending', 'Initial case profile for mental health assessment', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertCaseProfile, "Sample case profile insertion")) {
        return false;
    }
    
    // Inserir endereço do assessor (user_key = assessorId)
    string insertAddressAssessor = R"(
        INSERT OR IGNORE INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES ()" + to_string(addressId1) + ", " + to_string(assessorId) + R"(, '456 Healthcare St', 'Toronto', 'ON', 'M5H 2N2', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAddressAssessor, "Sample assessor address insertion")) {
        return false;
    }
    
    // Inserir endereço do cliente (user_key = clientId)
    string insertAddressClient = R"(
        INSERT OR IGNORE INTO address (id, user_key, street, city, province, postal_code, created_at, modified_at)
        VALUES ()" + to_string(addressId2) + ", " + to_string(clientId) + R"(, '123 Main Street', 'Toronto', 'ON', 'M5V 3A8', ')" + currentTime + "', '" + currentTime + R"(')
    )";
    
    if (!executeSQLCommand(db, insertAddressClient, "Sample client address insertion")) {
        return false;
    }
    
    logMessage("INFO", "Sample data inserted successfully");
    return true;
}

// Função para testar as classes existentes
void runClassTests() {
    logMessage("INFO", "Running class tests...");
    
    cout << "\n" << string(60, '=') << endl;
    cout << "           SILVER CLINIC CLASS TESTS" << endl;
    cout << string(60, '=') << endl;
    
    // Test DateTime class
    cout << "\n1. DateTime Tests:" << endl;
    DateTime now = DateTime::now();
    cout << "   Current time: " << now.toString() << endl;
    
    DateTime birth_date(1990, 5, 15, 14, 30, 45);
    cout << "   Birth date: " << birth_date.toString() << endl;
    cout << "   Age: " << birth_date.getAgeInYears() << " years" << endl;
    
    // Test Address class
    cout << "\n2. Address Tests:" << endl;
    Address addr(1, 100, "123 Main Street", "Toronto", "ON", "M5V 3A8");
    cout << "   Address created successfully" << endl;
    cout << "   Formatted postal code: " << formatCanadianPostalCode(addr.getPostalCode()) << endl;
    
    // Test Assessor class
    cout << "\n3. Assessor Tests:" << endl;
    Address assessor_address(Address::getNextAddressId(), Assessor::ID_PREFIX + 1, "456 Healthcare St", "Toronto", "ON", "M5H 2N2");
    Assessor dr_silva(Assessor::getNextAssessorId(), "Dr. Maria", "Silva", "maria.silva@silverclinic.ca", "416-555-0123", 
                      assessor_address, DateTime::now(), DateTime::now());
    cout << "   Assessor created: " << dr_silva.getFullName() << endl;
    cout << "   Assessor ID: " << dr_silva.getAssessorId() << " (should start with " << Assessor::ID_PREFIX << ")" << endl;
    cout << "   Email valid: " << (dr_silva.isValidEmail() ? "Yes" : "No") << endl;
    cout << "   Phone valid: " << (dr_silva.isValidPhone() ? "Yes" : "No") << endl;
    
    // Test Client class
    cout << "\n4. Client Tests:" << endl;
    Address client_address(Address::getNextAddressId(), Client::ID_PREFIX + 1, "123 Main Street", "Toronto", "ON", "M5V 3A8");
    Client joao_santos(Client::getNextClientId(), "João", "Santos", "joao.santos@email.com", "416-555-9876", 
                       "1985-03-15", client_address, DateTime::now(), DateTime::now());
    cout << "   Client created: " << joao_santos.getFullName() << endl;
    cout << "   Client ID: " << joao_santos.getClientId() << " (should start with " << Client::ID_PREFIX << ")" << endl;
    cout << "   Age: " << joao_santos.getAge() << " years" << endl;
    cout << "   Email valid: " << (joao_santos.isValidEmail() ? "Yes" : "No") << endl;
    cout << "   Phone valid: " << (joao_santos.isValidPhone() ? "Yes" : "No") << endl;
    
    // Test CaseProfile class
    cout << "\n5. CaseProfile Tests:" << endl;
    CaseProfile case_profile(joao_santos.getClientId(), dr_silva.getAssessorId(), "Initial mental health assessment case");
    cout << "   Case Profile created successfully" << endl;
    cout << "   Case Profile ID: " << case_profile.getCaseProfileId() << " (should start with " << CaseProfile::ID_PREFIX << ")" << endl;
    cout << "   Client ID: " << case_profile.getClientId() << " (valid: " << (case_profile.isValidClientId() ? "Yes" : "No") << ")" << endl;
    cout << "   Assessor ID: " << case_profile.getAssessorId() << " (valid: " << (case_profile.isValidAssessorId() ? "Yes" : "No") << ")" << endl;
    cout << "   Status: " << case_profile.getStatus() << " (should be 'Pending')" << endl;
    cout << "   Is Pending: " << (case_profile.isPending() ? "Yes" : "No") << endl;
    
    // Test status change
    cout << "   Testing status change..." << endl;
    case_profile.activate();
    cout << "   After activation - Status: " << case_profile.getStatus() << " (active: " << (case_profile.isActive() ? "Yes" : "No") << ")" << endl;
    
    // Test new utility functions
    cout << "\n6. Utility Function Tests:" << endl;
    cout << "   Phone formatted: " << formatCanadianPhoneNumber("4165550123") << endl;
    cout << "   Random patient ID: " << generateRandomId("PAT_", 6) << endl;
    cout << "   Current timestamp: " << getCurrentTimestamp() << endl;
    cout << "   Is business hours (14:30): " << (isBusinessHours(14, 30) ? "Yes" : "No") << endl;
    
    // Test ID system
    cout << "\n7. ID System Tests:" << endl;
    cout << "   Next Assessor ID: " << Assessor::getNextAssessorId() << " (should be >= " << (Assessor::ID_PREFIX + 1) << ")" << endl;
    cout << "   Next Client ID: " << Client::getNextClientId() << " (should be >= " << (Client::ID_PREFIX + 1) << ")" << endl;
    cout << "   Next CaseProfile ID: " << CaseProfile::getNextCaseProfileId() << " (should be >= " << (CaseProfile::ID_PREFIX + 1) << ")" << endl;
    cout << "   Next Address ID: " << Address::getNextAddressId() << " (should be >= " << (Address::ID_PREFIX + 1) << ")" << endl;
    
    // Teste do sistema de validação de notas
    cout << "\n8. Notes Validation Tests:" << endl;
    
    // Criar um texto muito longo (mais de 1500 caracteres)
    string longNotes = "";
    for (int i = 0; i < 30; i++) {
        longNotes += "Esta é uma linha de teste para validar o limite de caracteres das notas do sistema. ";
    }
    longNotes += "Texto adicional para garantir que exceda 1500 caracteres.";
    
    cout << "   Original notes length: " << longNotes.length() << " characters" << endl;
    
    // Criar um case profile de teste
    CaseProfile testCase(300001, 100001);
    testCase.setNotes(longNotes);
    
    cout << "   Final notes length: " << testCase.getNotes().length() << " characters" << endl;
    cout << "   Notes validation working: " << (testCase.getNotes().length() <= 1500 ? "Yes" : "No") << endl;
    
    // Testar isValidNotes
    cout << "   isValidNotes(short text): " << (testCase.isValidNotes("Short text") ? "Yes" : "No") << endl;
    cout << "   isValidNotes(long text): " << (testCase.isValidNotes(longNotes) ? "Yes" : "No") << endl;

    // Teste da classe FamilyPhysician
    cout << "\n9. FamilyPhysician Form Tests:" << endl;
    
    SilverClinic::Forms::FamilyPhysician familyDoc(300001, "Dr. João Oliveira", "Toronto Medical Center");
    familyDoc.setPhone("(416) 555-9876");
    familyDoc.setEmail("joao.oliveira@torontomedical.ca");
    familyDoc.setAddress("456 University Ave");
    familyDoc.setPostalCode("M5G1X5");
    familyDoc.setCity("Toronto");
    familyDoc.setProvince("ON");
    familyDoc.setSpecialization("Family Medicine");
    familyDoc.setNotes("Médico da família responsável pelo acompanhamento médico geral do paciente.");
    
    cout << "   Family Physician created: " << familyDoc.getDoctorName() << endl;
    cout << "   Family Physician ID: " << familyDoc.getFamilyPhysicianId() << " (should start with 500000)" << endl;
    cout << "   Client ID: " << familyDoc.getClientId() << " (valid: " << (familyDoc.isValidClientId(familyDoc.getClientId()) ? "Yes" : "No") << ")" << endl;
    cout << "   Clinic: " << familyDoc.getClinicName() << endl;
    cout << "   Phone: " << familyDoc.getPhone() << endl;
    cout << "   Postal Code: " << familyDoc.getPostalCode() << endl;
    cout << "   Notes length: " << familyDoc.getNotes().length() << " characters" << endl;
    cout << "   All data valid: " << (familyDoc.isValidData() ? "Yes" : "No") << endl;

    logMessage("SUCCESS", "All class tests completed successfully");
}

int main() {
    cout << "=== Silver Clinic Management System ===" << endl;
    cout << "=== Database Setup and Application Initialization ===" << endl;
    
    try {
        // Configurar e abrir banco de dados SQLite
        sqlite3* db;
        int result = sqlite3_open("clinic.db", &db);
        
        if (result != SQLITE_OK) {
            cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
            return 1;
        }
        
        logMessage("INFO", "Database opened successfully: clinic.db");
        
        // Criar tabelas do banco de dados
        if (!createDatabaseTables(db)) {
            cerr << "Failed to create database tables" << endl;
            sqlite3_close(db);
            return 1;
        }
        
        // Inserir dados de exemplo (apenas se as tabelas estiverem vazias)
        if (!insertSampleData(db)) {
            cerr << "Failed to insert sample data" << endl;
            sqlite3_close(db);
            return 1;
        }
        
        // Executar testes das classes
        runClassTests();
        
        // Exibir informações do banco de dados
        cout << "\n" << string(60, '=') << endl;
        cout << "             DATABASE INFORMATION" << endl;
        cout << string(60, '=') << endl;
        
        // Consultar dados do banco
        sqlite3_stmt* stmt;
        
        // Contar assessores
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM assessor", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "\nAssessors in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Contar clientes
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM client", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Clients in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Contar endereços
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM address", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Addresses in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        // Contar case profiles
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM case_profile", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                cout << "Case Profiles in database: " << count << endl;
            }
            sqlite3_finalize(stmt);
        }
        
        cout << "\n" << string(60, '=') << endl;
        logMessage("SUCCESS", "Silver Clinic system initialized successfully!");
        cout << "Database file: clinic.db" << endl;
        cout << "Application ready for use." << endl;
        cout << string(60, '=') << endl;
        
        // Fechar banco de dados
        sqlite3_close(db);
        logMessage("INFO", "Database connection closed");
        
    } catch (const exception& e) {
        cerr << "Error occurred: " << e.what() << endl;
        logMessage("ERROR", "Application failed to initialize: " + string(e.what()));
        return 1;
    }
    
    return 0;
    
    return 0;
}
