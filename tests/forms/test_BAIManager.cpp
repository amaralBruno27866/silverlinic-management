#include <cassert>
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include "managers/BeckAnxietyInventoryManager.h"
#include "forms/BeckAnxietyInventory.h"
#include "core/DatabaseConfig.h"

using namespace std;
using namespace SilverClinic;
using namespace SilverClinic::Forms;
static sqlite3* db=nullptr;
static void setup(){
	DatabaseConfig::ensureDirectoriesExist();
	remove("test_bai_manager.db");
	if(sqlite3_open("test_bai_manager.db", &db)!=SQLITE_OK){ cerr<<"open fail"; exit(1);} 
	sqlite3_exec(db,"PRAGMA foreign_keys=ON;",nullptr,nullptr,nullptr);
	const char* sql="CREATE TABLE beck_anxiety_inventory( id INTEGER PRIMARY KEY, case_profile_id INTEGER, type TEXT, question_1 INTEGER, question_2 INTEGER, question_3 INTEGER, question_4 INTEGER, question_5 INTEGER, question_6 INTEGER, question_7 INTEGER, question_8 INTEGER, question_9 INTEGER, question_10 INTEGER, question_11 INTEGER, question_12 INTEGER, question_13 INTEGER, question_14 INTEGER, question_15 INTEGER, question_16 INTEGER, question_17 INTEGER, question_18 INTEGER, question_19 INTEGER, question_20 INTEGER, question_21 INTEGER, total_score INTEGER, severity_level TEXT, created_at TEXT, modified_at TEXT );";
	sqlite3_exec(db,sql,nullptr,nullptr,nullptr);
}
static void teardown(){ if(db){ sqlite3_close(db); db=nullptr;} remove("test_bai_manager.db"); }
int main(){
	setup();
	BeckAnxietyInventoryManager mgr(db);
	BeckAnxietyInventory form(400001); // construtor com case_profile_id
	// Define cada questão com valor 2
	form.setQuestion1(2); form.setQuestion2(2); form.setQuestion3(2); form.setQuestion4(2); form.setQuestion5(2);
	form.setQuestion6(2); form.setQuestion7(2); form.setQuestion8(2); form.setQuestion9(2); form.setQuestion10(2);
	form.setQuestion11(2); form.setQuestion12(2); form.setQuestion13(2); form.setQuestion14(2); form.setQuestion15(2);
	form.setQuestion16(2); form.setQuestion17(2); form.setQuestion18(2); form.setQuestion19(2); form.setQuestion20(2);
	form.setQuestion21(2);
	assert(mgr.create(form));
	auto list = mgr.listByCase(400001); assert(list.size()==1);
	auto stored = list[0];
	assert(stored.getTotalScore()==42);
	stored.setQuestion3(0); // atualizar pergunta 3
	assert(mgr.update(stored));
	auto again = mgr.listByCase(400001)[0];
	assert(again.getTotalScore()==40);
	// CSV import
	const char* csv="id,case_profile_id,type,question_1,question_2,question_3,question_4,question_5,question_6,question_7,question_8,question_9,question_10,question_11,question_12,question_13,question_14,question_15,question_16,question_17,question_18,question_19,question_20,question_21,total_score,severity_level,created_at,modified_at\n210010,400001,BAI,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,, ,2025-01-01 00:00:00,2025-01-01 00:00:00\n";
	string path="bai_import.csv"; { ofstream f(path); f<<csv; }
	int imported = mgr.importFromCSV(path);
	assert(imported==1);
	auto list2 = mgr.listByCase(400001);
	assert(list2.size()==2);
	cout<<"BAI manager tests passed"<<endl;
	remove(path.c_str());
	teardown();
	return 0;
}
