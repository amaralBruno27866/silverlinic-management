#ifndef CSV_IMPORTER_H
#define CSV_IMPORTER_H

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <sqlite3.h>
#include "utils/CSVUtils.h"
#include "core/Utils.h"
#include "utils/StructuredLogger.h"

namespace SilverClinic {
namespace utils {

    /**
     * @brief Centralized CSV import template for all managers
     * 
     * This template class eliminates code duplication across managers
     * by providing a standardized CSV import process with:
     * - Robust transaction handling
     * - Consistent error reporting
     * - Flexible validation and parsing
     * - Standardized logging
     */
    template<typename T>
    class CSVImporter {
    public:
        /**
         * @brief Detailed import result with comprehensive reporting
         */
        struct ImportResult {
            int success = 0;
            int failed = 0;
            std::vector<std::string> errors;
            std::vector<std::pair<int, std::string>> duplicates;
            
            // Helper method to get total processed
            int getTotal() const { return success + failed; }
            
            // Helper method to check if import was successful
            bool hasErrors() const { return failed > 0 || !errors.empty(); }
        };

        /**
         * @brief Function types for customizable import behavior
         */
        using ParseFunction = std::function<std::optional<T>(const std::unordered_map<std::string, std::string>&)>;
        using ValidateFunction = std::function<bool(const T&)>;
        using CreateFunction = std::function<bool(const T&)>;
        using DuplicateCheckFunction = std::function<std::optional<int>(const T&)>;

        /**
         * @brief Constructor
         * @param db Database connection
         * @param entityName Name for logging (e.g., "Assessor", "Client")
         */
        explicit CSVImporter(sqlite3* db, const std::string& entityName)
            : m_db(db), m_entityName(entityName) {}

        /**
         * @brief Import data from CSV file with full customization
         * 
         * @param filePath Path to CSV file
         * @param requiredHeaders List of mandatory column headers
         * @param parser Function to parse CSV row into object T
         * @param validator Function to validate object before insertion
         * @param creator Function to create object in database
         * @param duplicateChecker Optional function to check for existing duplicates
         * @return Detailed import result
         */
        ImportResult importFromCSV(
            const std::string& filePath,
            const std::vector<std::string>& requiredHeaders,
            ParseFunction parser,
            ValidateFunction validator,
            CreateFunction creator,
            DuplicateCheckFunction duplicateChecker = nullptr
        );

        /**
         * @brief Simplified import method that returns only success count
         * @return Number of successfully imported records
         */
        int importFromCSVSimple(
            const std::string& filePath,
            const std::vector<std::string>& requiredHeaders,
            ParseFunction parser,
            ValidateFunction validator,
            CreateFunction creator,
            DuplicateCheckFunction duplicateChecker = nullptr
        ) {
            ImportResult result = importFromCSV(filePath, requiredHeaders, parser, validator, creator, duplicateChecker);
            return result.success;
        }

    private:
        sqlite3* m_db;
        std::string m_entityName;

        /**
         * @brief Validate that all required headers are present in CSV
         */
        bool validateHeaders(const csv::CSVTable& table, const std::vector<std::string>& requiredHeaders);

        /**
         * @brief Begin database transaction with proper error handling
         */
        bool beginTransaction();

        /**
         * @brief Commit transaction with error handling and rollback on failure
         */
        bool commitTransaction();

        /**
         * @brief Rollback transaction
         */
        void rollbackTransaction();

        /**
         * @brief Log structured message for import operations
         */
        void logImportMessage(::utils::LogLevel level, const std::string& operation, const std::string& message);
    };

    // Template implementation must be in header file
    template<typename T>
    typename CSVImporter<T>::ImportResult CSVImporter<T>::importFromCSV(
        const std::string& filePath,
        const std::vector<std::string>& requiredHeaders,
        ParseFunction parser,
        ValidateFunction validator,
        CreateFunction creator,
        DuplicateCheckFunction duplicateChecker
    ) {
        ImportResult result;
        bool inTransaction = false;

        try {
            logImportMessage(::utils::LogLevel::INFO, "start", "Starting CSV import from: " + filePath);

            // 1. Read CSV file
            csv::CSVTable table = csv::CSVReader::readFile(filePath);
            logImportMessage(::utils::LogLevel::DEBUG, "read", "CSV file read successfully, rows: " + std::to_string(table.rows.size()));

            // 2. Validate required headers
            if (!validateHeaders(table, requiredHeaders)) {
                logImportMessage(::utils::LogLevel::ERROR, "validate", "Missing required headers");
                return result; // Return with success=0, failed=0
            }

            // 3. Begin transaction for performance and atomicity
            inTransaction = beginTransaction();
            if (inTransaction) {
                logImportMessage(::utils::LogLevel::DEBUG, "transaction", "Transaction started successfully");
            } else {
                logImportMessage(::utils::LogLevel::WARN, "transaction", "Failed to start transaction - continuing without atomicity");
            }

            // 4. Process each row
            int rowIndex = 0;
            for (const auto& row : table.rows) {
                rowIndex++;
                try {
                    // Parse CSV row into object
                    std::optional<T> parsedObject = parser(row);
                    if (!parsedObject.has_value()) {
                        result.failed++;
                        result.errors.push_back("Failed to parse row " + std::to_string(rowIndex));
                        continue;
                    }

                    // Validate object
                    if (!validator(parsedObject.value())) {
                        result.failed++;
                        result.errors.push_back("Validation failed for row " + std::to_string(rowIndex));
                        continue;
                    }

                    // Check for duplicates if checker provided
                    if (duplicateChecker) {
                        std::optional<int> existingId = duplicateChecker(parsedObject.value());
                        if (existingId.has_value()) {
                            result.failed++;
                            result.duplicates.emplace_back(existingId.value(), "Duplicate found for row " + std::to_string(rowIndex));
                            continue;
                        }
                    }

                    // Create object in database
                    if (creator(parsedObject.value())) {
                        result.success++;
                    } else {
                        result.failed++;
                        result.errors.push_back("Database insertion failed for row " + std::to_string(rowIndex));
                    }

                } catch (const std::exception& e) {
                    result.failed++;
                    result.errors.push_back("Exception processing row " + std::to_string(rowIndex) + ": " + e.what());
                }
            }

            // 5. Commit transaction
            if (inTransaction) {
                if (commitTransaction()) {
                    logImportMessage(::utils::LogLevel::DEBUG, "transaction", "Transaction committed successfully");
                } else {
                    logImportMessage(::utils::LogLevel::ERROR, "transaction", "Failed to commit transaction");
                }
            }

        } catch (const std::exception& e) {
            if (inTransaction) {
                rollbackTransaction();
                logImportMessage(::utils::LogLevel::ERROR, "transaction", "Transaction rolled back due to exception");
            }
            result.errors.push_back("CSV import exception: " + std::string(e.what()));
            logImportMessage(::utils::LogLevel::ERROR, "exception", "CSV import failed: " + std::string(e.what()));
        }

        // Log final summary
        std::string summary = "CSV import completed - Success: " + std::to_string(result.success) + 
                             ", Failed: " + std::to_string(result.failed) + 
                             ", Duplicates: " + std::to_string(result.duplicates.size());
        logImportMessage(::utils::LogLevel::INFO, "summary", summary);

        return result;
    }

    template<typename T>
    bool CSVImporter<T>::validateHeaders(const csv::CSVTable& table, const std::vector<std::string>& requiredHeaders) {
        for (const auto& header : requiredHeaders) {
            if (std::find(table.headers.begin(), table.headers.end(), header) == table.headers.end()) {
                logImportMessage(::utils::LogLevel::ERROR, "header_missing", "Missing required header: " + header);
                return false;
            }
        }
        return true;
    }

    template<typename T>
    bool CSVImporter<T>::beginTransaction() {
        char* errorMsg = nullptr;
        int result = sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, &errorMsg);
        if (result != SQLITE_OK) {
            if (errorMsg) {
                logImportMessage(::utils::LogLevel::WARN, "transaction_begin_fail", std::string("SQLite error: ") + errorMsg);
                sqlite3_free(errorMsg);
            }
            return false;
        }
        return true;
    }

    template<typename T>
    bool CSVImporter<T>::commitTransaction() {
        char* errorMsg = nullptr;
        int result = sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, &errorMsg);
        if (result != SQLITE_OK) {
            if (errorMsg) {
                logImportMessage(::utils::LogLevel::ERROR, "transaction_commit_fail", std::string("SQLite error: ") + errorMsg);
                sqlite3_free(errorMsg);
            }
            // Attempt rollback on commit failure
            rollbackTransaction();
            return false;
        }
        return true;
    }

    template<typename T>
    void CSVImporter<T>::rollbackTransaction() {
        char* errorMsg = nullptr;
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, &errorMsg);
        if (errorMsg) {
            logImportMessage(::utils::LogLevel::WARN, "transaction_rollback", std::string("Rollback completed, SQLite message: ") + errorMsg);
            sqlite3_free(errorMsg);
        }
    }

    template<typename T>
    void CSVImporter<T>::logImportMessage(::utils::LogLevel level, const std::string& operation, const std::string& message) {
        ::utils::LogEventContext context{
            "CSV_IMPORT",
            operation,
            m_entityName,
            std::nullopt,
            std::nullopt
        };
        ::utils::logStructured(level, context, message);
    }

} // namespace utils
} // namespace SilverClinic

#endif // CSV_IMPORTER_H