#include <cassert>
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include "managers/BeckDepressionInventoryManager.h"
#include "forms/BeckDepressionInventory.h"
#include "core/Utils.h"
#include "core/DatabaseConfig.h"

using namespace std;
using namespace SilverClinic;            // Para acessar DatabaseConfig sem qualificação extensa
using namespace SilverClinic::Forms;     // Form classes

static sqlite3* db=nullptr;

static void setup(){
    DatabaseConfig::ensureDirectoriesExist();
    remove("test_bdi_manager.db");
    if(sqlite3_open("test_bdi_manager.db", &db)!=SQLITE_OK){ cerr<<"DB open fail"<<endl; exit(1);}    
    sqlite3_exec(db, "PRAGMA foreign_keys=ON;", nullptr,nullptr,nullptr);
    const char* sql = "CREATE TABLE beck_depression_inventory( id INTEGER PRIMARY KEY, case_profile_id INTEGER, type TEXT, question_1 INTEGER, question_2 INTEGER, question_3 INTEGER, question_4 INTEGER, question_5 INTEGER, question_6 INTEGER, question_7 INTEGER, question_8 INTEGER, question_9 INTEGER, question_10 INTEGER, question_11 INTEGER, question_12 INTEGER, question_13 INTEGER, question_14 INTEGER, question_15 INTEGER, question_16 INTEGER, question_17 INTEGER, question_18 INTEGER, question_19 INTEGER, question_20 INTEGER, question_21 INTEGER, total_score INTEGER, severity_level TEXT, created_at TEXT, modified_at TEXT );";
    sqlite3_exec(db, sql, nullptr,nullptr,nullptr);
}
static void teardown(){ if(db){ sqlite3_close(db); db=nullptr;} remove("test_bdi_manager.db"); }

int main(){
    setup();
    BeckDepressionInventoryManager mgr(db);
    // Usa construtor que recebe apenas case_profile_id; ID é gerado internamente
    BeckDepressionInventory form(400001);
    // Define cada questão (não existe método genérico setQuestion)
    form.setQuestion1(1);  form.setQuestion2(1);  form.setQuestion3(1);  form.setQuestion4(1);
    form.setQuestion5(1);  form.setQuestion6(1);  form.setQuestion7(1);  form.setQuestion8(1);
    form.setQuestion9(1);  form.setQuestion10(1); form.setQuestion11(1); form.setQuestion12(1);
    form.setQuestion13(1); form.setQuestion14(1); form.setQuestion15(1); form.setQuestion16(1);
    form.setQuestion17(1); form.setQuestion18(1); form.setQuestion19(1); form.setQuestion20(1);
    form.setQuestion21(1);
    assert(mgr.create(form));
    auto all = mgr.listByCase(400001); assert(all.size()==1);
    auto stored = all[0];
    assert(stored.getTotalScore()==21); // each 1
    assert(!stored.getSeverityLevel().empty());
    // Update: altera a questão 5 para 3
    stored.setQuestion5(3);
    assert(mgr.update(stored));
    auto again = mgr.listByCase(400001)[0];
    assert(again.getTotalScore()==23);
    // CSV import header + row
    const char* csv="id,case_profile_id,type,question_1,question_2,question_3,question_4,question_5,question_6,question_7,question_8,question_9,question_10,question_11,question_12,question_13,question_14,question_15,question_16,question_17,question_18,question_19,question_20,question_21,total_score,severity_level,created_at,modified_at\n200010,400001,BDI,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,, ,2025-01-01 00:00:00,2025-01-01 00:00:00\n";
    string path="bdi_import.csv"; { ofstream f(path); f<<csv; }
    int imported = mgr.importFromCSV(path);
    assert(imported==1);
    auto all2 = mgr.listByCase(400001); assert(all2.size()==2);
    cout<<"BDI manager tests passed"<<endl;
    remove(path.c_str());
    teardown();
    return 0;
}
