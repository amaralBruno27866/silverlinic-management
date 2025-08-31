#include <cassert>
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include "managers/AutomobileAnxietyInventoryManager.h"
#include "forms/AutomobileAnxietyInventory.h"
#include "core/Utils.h"

using namespace SilverClinic;
using namespace SilverClinic::Forms;

static sqlite3* db=nullptr;

static void setup(){
    remove("test_aai_manager.db");
    if(sqlite3_open("test_aai_manager.db", &db)!=SQLITE_OK){ std::cerr<<"open fail"; std::exit(1);}    
    sqlite3_exec(db,"PRAGMA foreign_keys=ON;",nullptr,nullptr,nullptr);
    const char* schema = R"SQL(
CREATE TABLE assessor(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, email TEXT, normalized_email TEXT GENERATED ALWAYS AS (lower(trim(email))) VIRTUAL);
CREATE TABLE client(id INTEGER PRIMARY KEY, firstname TEXT, lastname TEXT, email TEXT);
CREATE TABLE automobile_anxiety_inventory(
    id INTEGER PRIMARY KEY,
    case_profile_id INTEGER,
    type TEXT,
    q1 INT,q2 INT,q3 INT,q4 INT,q5 INT,q6 INT,q7 INT,q8 INT,q9 INT,q10 INT,
    q11 INT,q12 INT,q13 INT,
    q14_driver INT,q14_passenger INT,q14_no_difference INT,
    q15_a INT,q15_b TEXT,
    q16 INT,q17 INT,q18 INT,
    q19 INT,q19_sidewalks INT,q19_crossing INT,q19_both INT,
    q20 INT,q21 INT,q22 INT,q23 INT,
    created_at TEXT, modified_at TEXT
);
INSERT INTO assessor(id,firstname,lastname,email) VALUES(100001,'Ana','Silva','ana@ex.com');
INSERT INTO client(id,firstname,lastname,email) VALUES(300001,'Joao','Souza','joao@ex.com');
)SQL";
    char* err=nullptr; if(sqlite3_exec(db,schema,nullptr,nullptr,&err)!=SQLITE_OK){ std::cerr<<"schema error: "<<(err?err:"")<<"\n"; std::exit(1);}    
}

static void teardown(){ if(db){ sqlite3_close(db); db=nullptr; } }

int main(){
    setup();
    AutomobileAnxietyInventoryManager mgr(db);
    AutomobileAnxietyInventory form(300001); // using client id pattern as case_profile_id placeholder in this isolated test
    form.setQuestion1(true); form.setQuestion2(true); form.setQuestion14Driver(true); form.setQuestion15A(true); form.setQuestion15B("Family");
    bool created = mgr.create(form);
    assert(created);
    auto loaded = mgr.getById(form.getAAIId());
    assert(loaded.has_value());
    assert(loaded->getQuestion1()==true);
    form.setQuestion3(true);
    bool updated = mgr.update(form);
    assert(updated);
    auto list = mgr.listByCase(form.getCaseProfileId());
    assert(!list.empty());
    const char* csvName = "aai_import.csv";
    std::ofstream csv(csvName);
    csv << "case_profile_id,question_1,question_2,question_3,question_4,question_5,question_6,question_7,question_8,question_9,question_10,question_11,question_12,question_13,question_14_driver,question_15_a,question_15_b,question_16,question_17,question_18,question_19,question_20,question_21,question_22,question_23\n";
    csv << form.getCaseProfileId() << ",1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,Text,0,0,0,0,0,0,0,0\n";
    csv.close();
    int imported = mgr.importFromCSV(csvName);
    assert(imported==1);
    teardown();
    std::cout << "test_AAIManager passed\n";
    return 0;
}
