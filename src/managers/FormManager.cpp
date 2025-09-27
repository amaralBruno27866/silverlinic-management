#include "managers/FormManager.h"
#include "core/Utils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <cstring> // for strlen
#include <random>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace SilverClinic;
namespace fs = std::filesystem;

// Map form keys to template filenames and whether they require case context
struct FormMeta { string filename; bool needsContext; };
static const unordered_map<string,FormMeta> FORM_TEMPLATE_MAP = {
    // Base forms (no existing case needed)
    {"assessor", {"Assessor.html", false}},
    {"client",   {"Client.html", false}},
    {"case_profile", {"CaseProfile.html", false}},
    // Case-bound forms
    {"activities_of_daily_living", {"ActivitiesOfDailyLiving.html", true}},
    {"beck_anxiety_inventory",     {"BeckAnxietyInventory.html", true}},
    {"beck_depression_inventory",  {"BeckDepressionInventory.html", true}},
    {"automobile_anxiety_inventory", {"AutomobileAnxietyInventory.html", true}},
    {"pain_body_map",               {"PainBodyMap.html", true}},
    {"scl90r",                      {"SCL90R.html", true}}
};

vector<string> FormManager::listAvailableForms() const {
    vector<string> v; v.reserve(FORM_TEMPLATE_MAP.size());
    for (auto &p : FORM_TEMPLATE_MAP) v.push_back(p.first);
    return v;
}
bool FormManager::formRequiresContext(const string& key) const {
    auto it = FORM_TEMPLATE_MAP.find(key);
    if (it == FORM_TEMPLATE_MAP.end()) return false; // unknown handled elsewhere
    return it->second.needsContext;
}

// GUID Management Methods
string FormManager::generateFormGuid() const {
    // Generate a simple UUID-like string: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    static thread_local random_device rd;
    static thread_local mt19937 gen(rd());
    static thread_local uniform_int_distribution<> dis(0, 15);
    
    stringstream ss;
    const char* hex = "0123456789ABCDEF";
    
    // 8-4-4-4-12 format
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) ss << '-';
        ss << hex[dis(gen)];
    }
    
    return ss.str();
}

bool FormManager::ensureFormGuidsTable() const {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS form_guids (
            guid TEXT PRIMARY KEY,
            case_profile_id INTEGER NOT NULL,
            form_key TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (case_profile_id) REFERENCES case_profile(id)
        )
    )";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","table_create_fail","FormGuids","",{}}, errMsg ? errMsg : "Unknown error");
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool FormManager::storeFormGuid(const string& guid, int caseProfileId, const string& formKey) const {
    const char* sql = "INSERT INTO form_guids (guid, case_profile_id, form_key) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","prepare_fail","FormGuidStore","",{}}, sqlite3_errmsg(m_db));
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, guid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, caseProfileId);
    sqlite3_bind_text(stmt, 3, formKey.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","insert_fail","FormGuidStore",guid,{}}, sqlite3_errmsg(m_db));
        return false;
    }
    
    utils::logStructured(utils::LogLevel::INFO, {"MANAGER","guid_stored","FormManager",guid,{}}, "Form GUID stored for case " + to_string(caseProfileId));
    return true;
}

optional<int> FormManager::getCaseProfileByGuid(const string& guid) const {
    const char* sql = "SELECT case_profile_id FROM form_guids WHERE guid = ? LIMIT 1";
    sqlite3_stmt* stmt = nullptr;
    
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","prepare_fail","FormGuidLookup","",{}}, sqlite3_errmsg(m_db));
        return nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, guid.c_str(), -1, SQLITE_STATIC);
    
    optional<int> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

optional<FormManager::Context> FormManager::loadContext(int caseProfileId) const {
    Context ctx{}; ctx.caseProfileId = caseProfileId;
    const char* sql = R"(SELECT cp.client_id, cp.assessor_id, cp.created_at,
                                 c.firstname, c.lastname, c.email,
                                 a.firstname, a.lastname, a.email
                          FROM case_profile cp
                          LEFT JOIN client c ON cp.client_id = c.id
                          LEFT JOIN assessor a ON cp.assessor_id = a.id
                          WHERE cp.id = ? LIMIT 1)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","prepare_fail","FormContext","",""}, sqlite3_errmsg(m_db));
        return nullopt;
    }
    sqlite3_bind_int(stmt, 1, caseProfileId);
    optional<Context> result = nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ctx.clientId = sqlite3_column_int(stmt, 0);
        ctx.assessorId = sqlite3_column_int(stmt, 1);
        const char* createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        ctx.caseCreatedAt = createdAt ? createdAt : "";
    const char* cFirstPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    const char* cLastPtr  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    string cFirst = cFirstPtr ? cFirstPtr : "";
    string cLast  = cLastPtr  ? cLastPtr  : "";
        const char* cEmail = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    const char* aFirstPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    const char* aLastPtr  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    string aFirst = aFirstPtr ? aFirstPtr : "";
    string aLast  = aLastPtr  ? aLastPtr  : "";
        const char* aEmail = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        ctx.clientFullName = cFirst + (cLast.empty()?"":" ") + cLast;
        ctx.clientEmail = cEmail ? cEmail : "";
        ctx.assessorFullName = aFirst + (aLast.empty()?"":" ") + aLast;
        ctx.assessorEmail = aEmail ? aEmail : "";
        ctx.formGuid = generateFormGuid(); // Generate unique GUID for this form
        result = ctx;
    }
    sqlite3_finalize(stmt);
    return result;
}

string FormManager::buildOutputFileName(const string& key, int caseProfileId) const {
    return key + string("_case") + to_string(caseProfileId) + ".html";
}

string FormManager::loadFile(const string& path) const {
    ifstream in(path, ios::in | ios::binary);
    if (!in) return {};
    stringstream ss; ss << in.rdbuf();
    return ss.str();
}

bool FormManager::writeFile(const string& path, const string& content) const {
    ofstream out(path, ios::out | ios::binary | ios::trunc);
    if (!out) return false;
    out << content;
    return true;
}

bool FormManager::isUniqueFormIdField(const string& line) const {
    static const vector<string> prefixes = {"bai_id","bdi_id","scl90r_id","adl_id","pain_body_map_id","automobile_anxiety_inventory_id"};
    for (auto &p : prefixes) {
        if (line.find("id=\""+p+"\"") != string::npos) return true;
        if (line.find("name=\""+p+"\"") != string::npos) return true;
    }
    return false;
}

string FormManager::injectContext(const string& html, const Context& ctx, const string& /*key*/) const {
    stringstream in(html); string line; vector<string> lines; lines.reserve(512);
    bool insertedBlock = false; bool insideFormInfo = false;
    regex formInfoHeader(R"((Form Information))", regex_constants::icase);
    while (getline(in, line)) {
        if (isUniqueFormIdField(line)) continue;
        if (regex_search(line, formInfoHeader)) insideFormInfo = true;
        auto setValue = [&](const string& id, const string& value){
            size_t pos = line.find("id=\""+id+"\"");
            if (pos != string::npos) {
                regex valAttr(R"(value=\"[^\"]*\")");
                string replacement = "value=\"" + value + "\"";
                if (regex_search(line, valAttr)) {
                    line = regex_replace(line, valAttr, replacement);
                } else {
                    size_t gt = line.rfind('>');
                    if (gt != string::npos) line.insert(gt, " " + replacement + " ");
                }
            }
        };
        setValue("case_profile_id", to_string(ctx.caseProfileId));
        setValue("assessor_full_name", ctx.assessorFullName);
        setValue("assessor_email", ctx.assessorEmail);
        setValue("client_full_name", ctx.clientFullName);
        setValue("client_email", ctx.clientEmail);
        setValue("created_at", ctx.caseCreatedAt);
        setValue("form_guid", ctx.formGuid);
        lines.push_back(line);
        if (insideFormInfo && !insertedBlock && (line.find("</fieldset>") != string::npos || line.find("</legend>") != string::npos)) {
            string block;
            block += "<!-- Injected dynamic context -->\n";
            block += "<div class=\"sc-generated-context\">\n";
            block += "  <p><strong>Case ID:</strong> " + to_string(ctx.caseProfileId) + "</p>\n";
            block += "  <p><strong>Form GUID:</strong> <span class=\"form-guid\">" + ctx.formGuid + "</span></p>\n";
            block += "  <p><strong>Assessor:</strong> " + ctx.assessorFullName + " (" + ctx.assessorEmail + ")</p>\n";
            block += "  <p><strong>Client:</strong> " + ctx.clientFullName + " (" + ctx.clientEmail + ")</p>\n";
            block += "  <p><strong>Case Created At:</strong> " + ctx.caseCreatedAt + "</p>\n";
            block += "  <!-- Hidden field for form GUID -->\n";
            block += "  <input type=\"hidden\" name=\"form_guid\" id=\"form_guid\" value=\"" + ctx.formGuid + "\"/>\n";
            block += "</div>\n";
            lines.push_back(block);
            insertedBlock = true;
        }
    }
    if (!insertedBlock) {
        lines.push_back("<!-- Injected dynamic context (fallback) -->\n");
        lines.push_back("<div class=\"sc-generated-context\">\n");
        lines.push_back("  <p><strong>Case ID:</strong> " + to_string(ctx.caseProfileId) + "</p>\n");
        lines.push_back("  <p><strong>Form GUID:</strong> <span class=\"form-guid\">" + ctx.formGuid + "</span></p>\n");
        lines.push_back("  <p><strong>Assessor:</strong> " + ctx.assessorFullName + " (" + ctx.assessorEmail + ")</p>\n");
        lines.push_back("  <p><strong>Client:</strong> " + ctx.clientFullName + " (" + ctx.clientEmail + ")</p>\n");
        lines.push_back("  <p><strong>Case Created At:</strong> " + ctx.caseCreatedAt + "</p>\n");
        lines.push_back("  <!-- Hidden field for form GUID -->\n");
        lines.push_back("  <input type=\"hidden\" name=\"form_guid\" id=\"form_guid\" value=\"" + ctx.formGuid + "\"/>\n");
        lines.push_back("</div>\n");
    }
    string joined; joined.reserve(html.size()+512);
    for (auto &l : lines) { joined += l; if (joined.empty() || joined.back()!='\n') joined += '\n'; }
    replaceAllPlaceholders(joined, ctx);
    return joined;
}

void FormManager::replaceAllPlaceholders(string &buffer, const Context& ctx) const {
    struct PH { const char* token; string value; };
    const string caseId = to_string(ctx.caseProfileId);
    vector<PH> ph = {
        {"{{case_profile_id}}", caseId},
        {"{{form_guid}}", ctx.formGuid},
        {"{{assessor_full_name}}", ctx.assessorFullName},
        {"{{assessor_email}}", ctx.assessorEmail},
        {"{{client_full_name}}", ctx.clientFullName},
        {"{{client_email}}", ctx.clientEmail},
        {"{{created_at}}", ctx.caseCreatedAt},
    };
    for (auto &p : ph) {
        size_t pos = 0;
        while ((pos = buffer.find(p.token, pos)) != string::npos) {
            buffer.replace(pos, strlen(p.token), p.value);
            pos += p.value.size();
        }
    }
}

vector<FormGenerationResult> FormManager::generateForms(int caseProfileId,
                                                        const vector<string>& formKeys,
                                                        const string& templatesDir,
                                                        const string& outputDir) const {
    vector<FormGenerationResult> results;
    // Decide whether we need context at all
    bool needContext = caseProfileId > 0; // only attempt lookup if positive id provided
    optional<Context> ctxOpt;
    if (needContext) {
        ctxOpt = loadContext(caseProfileId);
        if (!ctxOpt) {
            // Context requested but not found -> forms needing context will fail
            utils::logStructured(utils::LogLevel::ERROR, {"MANAGER","context_not_found","FormContext", to_string(caseProfileId), {}}, "Case profile context not found");
        }
    }
    error_code ec; fs::create_directories(outputDir, ec);
    if (ec) {
        for (auto &k : formKeys) results.push_back({k, "", "", string("Cannot create output directory: ")+ ec.message(), "", false});
        return results;
    }
    for (auto &key : formKeys) {
        FormGenerationResult r; r.key = key;
        auto it = FORM_TEMPLATE_MAP.find(key);
        if (it == FORM_TEMPLATE_MAP.end()) { r.message = "Unknown form key"; results.push_back(r); continue; }
        r.templatePath = (fs::path(templatesDir) / it->second.filename).string();
        string raw = loadFile(r.templatePath);
        if (raw.empty()) { r.message = "Template not found or empty"; results.push_back(r); continue; }
    bool requiresContext = it->second.needsContext;
        string processed = raw;
    if (requiresContext) {
            if (!ctxOpt) { r.message = "Missing case context"; results.push_back(r); continue; }
            processed = injectContext(raw, *ctxOpt, key);
            r.formGuid = ctxOpt->formGuid; // Store GUID in result
            
            // Store GUID in database for tracking
            if (!storeFormGuid(ctxOpt->formGuid, caseProfileId, key)) {
                utils::logStructured(utils::LogLevel::WARN, {"MANAGER","guid_store_fail","FormManager",ctxOpt->formGuid,{}}, "Failed to store GUID in database");
            }
        }
        int idForName = (caseProfileId > 0) ? caseProfileId : 0; // for base forms allow 0 in filename? keep 0 or omit
        r.outputPath = (fs::path(outputDir) / buildOutputFileName(key, idForName)).string();
        if (!writeFile(r.outputPath, processed)) { r.message = "Failed to write output"; results.push_back(r); continue; }
    r.success = true; r.message = requiresContext ? ("Generated with GUID: " + r.formGuid) : "Generated";
        results.push_back(r);
    }
    return results;
}
