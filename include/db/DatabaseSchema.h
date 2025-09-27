#ifndef DB_DATABASE_SCHEMA_H
#define DB_DATABASE_SCHEMA_H

#include <string>
#include <vector>

namespace SilverClinic {
namespace db {

/**
 * @brief Centralized database schema definitions
 * 
 * This class contains all CREATE TABLE statements and schema-related
 * operations to ensure consistency across the application.
 */
class DatabaseSchema {
public:
    // Core entity tables
    static std::string getAssessorTableSQL();
    static std::string getClientTableSQL();
    static std::string getCaseProfileTableSQL();
    static std::string getAddressTableSQL();
    
    // Form tables
    static std::string getFamilyPhysicianTableSQL();
    static std::string getEmergencyContactTableSQL();
    static std::string getLegalRepresentativeTableSQL();
    static std::string getInsuranceCompanyTableSQL();
    static std::string getAutomobileAnxietyInventoryTableSQL();
    static std::string getBeckDepressionInventoryTableSQL();
    static std::string getBeckAnxietyInventoryTableSQL();
    static std::string getPainBodyMapTableSQL();
    static std::string getActivitiesOfDailyLivingTableSQL();
    static std::string getSCL90RTableSQL();
    
    // FormManager table
    static std::string getFormGuidsTableSQL();
    
    // Index creation
    static std::string getAssessorEmailIndexSQL();
    static std::string getAssessorNamePhoneIndexSQL();
    static std::string getPopulateNormalizedEmailSQL();
    
    // Get all table creation statements in correct order
    static std::vector<std::pair<std::string, std::string>> getAllTableDefinitions();
    
    // Get all index creation statements
    static std::vector<std::pair<std::string, std::string>> getAllIndexDefinitions();
    
    // Schema version management
    static int getCurrentSchemaVersion();
    static std::string getSchemaVersionTableSQL();
    
private:
    DatabaseSchema() = delete; // Static-only class
};

} // namespace db
} // namespace SilverClinic

#endif // DB_DATABASE_SCHEMA_H