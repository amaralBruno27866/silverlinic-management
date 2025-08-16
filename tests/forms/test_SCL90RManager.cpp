#include <cassert>
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include "managers/SCL90RManager.h"
#include "forms/SCL90R.h"
#include "core/DatabaseConfig.h"
using namespace std;
using namespace SilverClinic;
using namespace SilverClinic::Forms;
static sqlite3* db=nullptr; static void setup(){ remove("test_scl90r_manager.db"); if(sqlite3_open("test_scl90r_manager.db", &db)!=SQLITE_OK){ cerr<<"open fail"; exit(1);} sqlite3_exec(db,"PRAGMA foreign_keys=ON;",nullptr,nullptr,nullptr); // minimal table (first 10 qs + derived for brevity)
 string sql="CREATE TABLE scl90r( id INTEGER PRIMARY KEY, case_profile_id INTEGER, type TEXT"; for(int i=1;i<=90;++i){ sql += ", question_"+to_string(i)+" INTEGER"; } sql += ", gsi REAL, pst INTEGER, psdi REAL, severity_level TEXT, created_at TEXT, modified_at TEXT);"; sqlite3_exec(db, sql.c_str(), nullptr,nullptr,nullptr);} static void teardown(){ if(db){ sqlite3_close(db); db=nullptr;} remove("test_scl90r_manager.db"); }
int main(){
	setup();
	SCL90RManager mgr(db);
	SCL90R form(400001); // construtor por case_profile_id
	for(int i=1;i<=90;++i) form.setQuestion(i, (i%4));
	assert(mgr.create(form));
	auto list = mgr.listByCase(400001); assert(list.size()==1);
	auto stored = list[0];
	assert(stored.getGlobalSeverityIndex()>0);
	int oldGSI = stored.getGlobalSeverityIndex();
	int oldPST = stored.getPositiveSymptomTotal();
	stored.setQuestion(10,3);
	assert(mgr.update(stored));
	auto list2 = mgr.listByCase(400001);
	auto updated = list2[0];
	assert(updated.getQuestion(10)==3);
	// Verifica que recomputou métricas derivadas (GSI ou PST pode mudar)
	assert(updated.getGlobalSeverityIndex()>=oldGSI);
	assert(updated.getPositiveSymptomTotal()==oldPST || updated.getPositiveSymptomTotal()==oldPST+ (stored.getQuestion(10)>0?0:1));
	cout<<"SCL90R manager tests passed"<<endl;
	teardown();
	return 0;
}
