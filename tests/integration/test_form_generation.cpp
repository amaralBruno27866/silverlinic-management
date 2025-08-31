// Integration test: FormManager generation + basic round-trip (CSV import)
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sqlite3.h>
#include "managers/FormManager.h"
#include "managers/AssessorManager.h"
#include "core/Assessor.h"
#include "core/Client.h"
#include "core/CaseProfile.h"
#include "core/Utils.h"

using namespace std;
using namespace SilverClinic;
using namespace utils;
namespace fs = std::filesystem;

#define TEST_ASSERT(cond, msg) do { if(!(cond)) { cout << "❌ FAIL: " << msg << endl; return false; } else { cout << "✅ PASS: " << msg << endl; } } while(0)
#define RUN_TEST(fn) do { cout << "\n🧪 Running " #fn "..." << endl; if(fn()) { passed++; } else { failed++; } total++; } while(0)

static int total=0, passed=0, failed=0;

static string findProjectRoot() {
    fs::path p = fs::current_path();
    for (int i=0; i<6; ++i) {
        if (fs::exists(p / "CMakeLists.txt") && fs::exists(p / "web" / "views")) {
            return p.string();
        }
        if (p.has_parent_path()) p = p.parent_path(); else break;
    }
    return fs::current_path().string();
}

bool test_form_generation_and_placeholder_substitution() {
    sqlite3* db = nullptr;
    TEST_ASSERT(sqlite3_open(":memory:", &db) == SQLITE_OK, "Open in-memory DB");
    const char* ddl = R"SQL(
        CREATE TABLE assessor(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, phone TEXT, email TEXT, normalized_email TEXT GENERATED ALWAYS AS (lower(trim(email))) VIRTUAL, created_at TEXT, modified_at TEXT);
        CREATE TABLE client(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, phone TEXT, email TEXT, date_of_birth TEXT, created_at TEXT, modified_at TEXT);
        CREATE TABLE case_profile(id INTEGER PRIMARY KEY, client_id INTEGER NOT NULL, assessor_id INTEGER NOT NULL, status TEXT, notes TEXT, created_at TEXT, closed_at TEXT, modified_at TEXT);
    )SQL";
    char* err=nullptr; TEST_ASSERT(sqlite3_exec(db, ddl, nullptr, nullptr, &err)==SQLITE_OK, "Create schema"); if(err){ sqlite3_free(err);} 
    auto now = getCurrentTimestamp();
    int assessorId = 100001; int clientId = 300001; int caseId = 400001;
    string ins1 = "INSERT INTO assessor VALUES("+to_string(assessorId)+",'Ana','Silva','','ana@clinic.com','"+now+"','"+now+"');";
    string ins2 = "INSERT INTO client VALUES("+to_string(clientId)+",'Joao','Santos','','joao@client.com','1990-01-01','"+now+"','"+now+"');";
    string ins3 = "INSERT INTO case_profile VALUES("+to_string(caseId)+","+to_string(clientId)+","+to_string(assessorId)+",'Pending','Init','"+now+"',NULL,'"+now+"');";
    TEST_ASSERT(sqlite3_exec(db, ins1.c_str(), nullptr, nullptr, &err)==SQLITE_OK, "Insert assessor"); if(err){sqlite3_free(err);} 
    TEST_ASSERT(sqlite3_exec(db, ins2.c_str(), nullptr, nullptr, &err)==SQLITE_OK, "Insert client"); if(err){sqlite3_free(err);} 
    TEST_ASSERT(sqlite3_exec(db, ins3.c_str(), nullptr, nullptr, &err)==SQLITE_OK, "Insert case_profile"); if(err){sqlite3_free(err);} 
    FormManager fm(db);
    string projectRoot = findProjectRoot();
    string outDir = projectRoot + "/data/test/generated_forms";
    fs::create_directories(outDir);
    vector<string> keys = {"assessor", "beck_anxiety_inventory"};
    auto results = fm.generateForms(caseId, keys, projectRoot+"/web/views", outDir);
    TEST_ASSERT(results.size()==2, "Two form generation results");
    bool foundContext=false, foundBase=false;
    for (auto &r : results) {
        TEST_ASSERT(r.success, string("Form ")+r.key+" success");
        TEST_ASSERT(fs::exists(r.outputPath), string("Output exists for ")+r.key);
        string content; { ifstream in(r.outputPath); std::stringstream ss; ss<<in.rdbuf(); content=ss.str(); }
        TEST_ASSERT(content.find("{{case_profile_id}}")==string::npos, "No raw placeholder case_profile_id in output");
        if (r.key=="beck_anxiety_inventory") {
            TEST_ASSERT(content.find(to_string(caseId))!=string::npos, "Context form contains real case id");
            TEST_ASSERT(content.find("{{assessor_full_name}}") == string::npos, "Assessor name placeholder replaced");
            TEST_ASSERT(content.find("Ana Silva") != string::npos, "Assessor full name present");
            TEST_ASSERT(content.find("{{client_full_name}}") == string::npos, "Client name placeholder replaced");
            TEST_ASSERT(content.find("Joao Santos") != string::npos, "Client full name present");
            TEST_ASSERT(content.find("{{assessor_email}}") == string::npos, "Assessor email placeholder replaced");
            TEST_ASSERT(content.find("ana@clinic.com") != string::npos, "Assessor email present");
            foundContext=true;
        }
        if (r.key=="assessor") foundBase=true;
    }
    TEST_ASSERT(foundContext && foundBase, "Context + base forms generated");
    sqlite3_close(db);
    return true;
}

bool test_missing_context_generation_failure() {
    sqlite3* db = nullptr;
    TEST_ASSERT(sqlite3_open(":memory:", &db) == SQLITE_OK, "Open DB (no data) for negative test");
    const char* ddl = R"SQL(
        CREATE TABLE assessor(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, phone TEXT, email TEXT, normalized_email TEXT GENERATED ALWAYS AS (lower(trim(email))) VIRTUAL, created_at TEXT, modified_at TEXT);
        CREATE TABLE client(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, phone TEXT, email TEXT, date_of_birth TEXT, created_at TEXT, modified_at TEXT);
        CREATE TABLE case_profile(id INTEGER PRIMARY KEY, client_id INTEGER NOT NULL, assessor_id INTEGER NOT NULL, status TEXT, notes TEXT, created_at TEXT, closed_at TEXT, modified_at TEXT);
    )SQL";
    char* err=nullptr; TEST_ASSERT(sqlite3_exec(db, ddl, nullptr, nullptr, &err)==SQLITE_OK, "Create schema"); if(err){sqlite3_free(err);} 
    FormManager fm(db);
    int missingCaseId = 499999; // not inserted
    string root = findProjectRoot();
    string outDir = root + "/data/test/generated_forms_missing";
    fs::create_directories(outDir);
    vector<string> keys = {"assessor", "beck_anxiety_inventory"};
    auto results = fm.generateForms(missingCaseId, keys, root+"/web/views", outDir);
    bool baseOK=false, contextFailed=false;
    for (auto &r : results) {
        if (r.key=="assessor") baseOK = r.success;
        if (r.key=="beck_anxiety_inventory") {
            contextFailed = !r.success && r.message.find("Missing case context")!=string::npos;
            TEST_ASSERT(!r.success, "Context form should fail without context");
            TEST_ASSERT(!fs::exists(r.outputPath), "Context form file not created on failure");
        }
    }
    TEST_ASSERT(baseOK, "Base form generated successfully without context");
    TEST_ASSERT(contextFailed, "Context form properly failed");
    sqlite3_close(db);
    return true;
}

bool test_assessor_csv_import_round_trip() {
    sqlite3* db=nullptr; TEST_ASSERT(sqlite3_open(":memory:", &db)==SQLITE_OK, "Open DB for CSV import");
    char* err=nullptr;
    const char* ddl = R"SQL(
        CREATE TABLE assessor(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, phone TEXT, email TEXT, created_at TEXT, modified_at TEXT);
    )SQL";
    TEST_ASSERT(sqlite3_exec(db, ddl, nullptr, nullptr, &err)==SQLITE_OK, "Create assessor table"); if(err){sqlite3_free(err);} 
    AssessorManager mgr(db);
    string projectRoot = findProjectRoot();
    string csvPath = projectRoot + "/data/test/assessor_import_test.csv";
    fs::create_directories(projectRoot + "/data/test");
    ofstream out(csvPath); 
    out << "id,firstname,lastname,phone,email,created_at,modified_at\n";
    auto now = getCurrentTimestamp();
    // Use valid 10-digit Canadian-style numbers
    out << "100010,Ana,Silva,4165551111,ana@x.com,"<<now<<","<<now<<"\n";
    out << "100011,Caio,Pereira,4165552222,caio@x.com,"<<now<<","<<now<<"\n";
    out.close();
    int imported = mgr.importFromCSV(csvPath);
    TEST_ASSERT(imported==2, "Imported 2 assessors successfully");
    sqlite3_stmt* stmt=nullptr; int count=-1; sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM assessor", -1, &stmt, nullptr); if(sqlite3_step(stmt)==SQLITE_ROW) count = sqlite3_column_int(stmt,0); sqlite3_finalize(stmt);
    TEST_ASSERT(count==2, "Assessor table row count == 2");
    sqlite3_close(db);
    return true;
}

int main(){
    cout << "🚀 Form Generation Integration Tests" << endl;
    RUN_TEST(test_form_generation_and_placeholder_substitution);
    RUN_TEST(test_missing_context_generation_failure);
    RUN_TEST(test_assessor_csv_import_round_trip);
    // Cleanup generated artifacts
    try {
        string root = findProjectRoot();
        fs::path gen1 = root + "/data/test/generated_forms";
        fs::path gen2 = root + "/data/test/generated_forms_missing";
        fs::path csv = root + "/data/test/assessor_import_test.csv";
        if (fs::exists(gen1)) fs::remove_all(gen1);
        if (fs::exists(gen2)) fs::remove_all(gen2);
        if (fs::exists(csv)) fs::remove(csv);
    } catch(const std::exception &e) {
        cout << "⚠️  Cleanup warning: " << e.what() << endl;
    }
    cout << "\n📊 Summary: "<<passed<<" passed, "<<failed<<" failed ("<<total<<" tests)" << endl;
    return failed==0?0:1;
}
