#ifndef FORM_MANAGER_H
#define FORM_MANAGER_H

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <sqlite3.h>

namespace SilverClinic {

    struct FormGenerationResult {
        std::string key;          // form key requested
        std::string templatePath; // original template path
        std::string outputPath;   // written file path (empty if failed)
        std::string message;      // status / error message
        std::string formGuid;     // unique GUID for form tracking
        bool success {false};
    };

    class FormManager {
    public:
        explicit FormManager(sqlite3* db) : m_db(db) { ensureFormGuidsTable(); }

        // List available form keys
        std::vector<std::string> listAvailableForms() const;
        
        // GUID management
        std::string generateFormGuid() const;
        bool storeFormGuid(const std::string& guid, int caseProfileId, const std::string& formKey) const;
        std::optional<int> getCaseProfileByGuid(const std::string& guid) const;

    // Generate selected forms (by keys). If caseProfileId <= 0, context lookup is skipped
    // (allowed for base forms: assessor, client, case_profile). Forms that require context
    // will fail with an error if context is unavailable.
    std::vector<FormGenerationResult> generateForms(int caseProfileId,
                                                        const std::vector<std::string>& formKeys,
                                                        const std::string& templatesDir,
                                                        const std::string& outputDir) const;

    private:
        sqlite3* m_db {nullptr};

        struct Context {
            int caseProfileId;
            std::string caseCreatedAt;
            int clientId;
            int assessorId;
            std::string clientFullName;
            std::string clientEmail;
            std::string assessorFullName;
            std::string assessorEmail;
            std::string formGuid;  // unique identifier for this form instance
        };

        std::optional<Context> loadContext(int caseProfileId) const;
        std::string buildOutputFileName(const std::string& key, int caseProfileId) const;
        std::string loadFile(const std::string& path) const;
        bool writeFile(const std::string& path, const std::string& content) const;
    std::string injectContext(const std::string& html, const Context& ctx, const std::string& key) const;
        bool ensureFormGuidsTable() const;
    bool formRequiresContext(const std::string& key) const;

    // Helper to perform placeholder replacements (supports legacy uppercase tokens during transition)
    void replaceAllPlaceholders(std::string &buffer, const Context& ctx) const;
        bool isUniqueFormIdField(const std::string& line) const;
    };

} // namespace SilverClinic

#endif // FORM_MANAGER_H
