#include "managers/BeckAnxietyInventoryManager.h"
#include "core/Utils.h"
#include "utils/DbLogging.h"
#include <algorithm>

using namespace SilverClinic;
using namespace SilverClinic::Forms;

int BeckAnxietyInventoryManager::computeTotal(const BeckAnxietyInventory &f) const {
    return f.getQuestion1()+f.getQuestion2()+f.getQuestion3()+f.getQuestion4()+f.getQuestion5()+
           f.getQuestion6()+f.getQuestion7()+f.getQuestion8()+f.getQuestion9()+f.getQuestion10()+
           f.getQuestion11()+f.getQuestion12()+f.getQuestion13()+f.getQuestion14()+f.getQuestion15()+
           f.getQuestion16()+f.getQuestion17()+f.getQuestion18()+f.getQuestion19()+f.getQuestion20()+
           f.getQuestion21();
}

std::string BeckAnxietyInventoryManager::computeSeverity(int total) const { return BeckAnxietyInventory::interpretScore(total); }

bool BeckAnxietyInventoryManager::create(const BeckAnxietyInventory &form) {
    if (!form.isValidData()) { utils::logMessage("ERROR","BAI create invalid data"); return false; }
    int total = computeTotal(form); std::string level = computeSeverity(total);
    const char* sql = R"SQL(INSERT INTO beck_anxiety_inventory(
        id,case_profile_id,type,
        question_1,question_2,question_3,question_4,question_5,question_6,question_7,question_8,question_9,question_10,
        question_11,question_12,question_13,question_14,question_15,question_16,question_17,question_18,question_19,question_20,question_21,
        total_score,severity_level,created_at,modified_at)
        VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);)SQL";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr)!=SQLITE_OK){ utils::logDbPrepareError("BAI create", m_db, sql); return false; } int idx=1;
    sqlite3_bind_int(stmt, idx++, form.getBAIId()); sqlite3_bind_int(stmt, idx++, form.getCaseProfileId()); sqlite3_bind_text(stmt, idx++, form.getType().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, idx++, form.getQuestion1()); sqlite3_bind_int(stmt, idx++, form.getQuestion2()); sqlite3_bind_int(stmt, idx++, form.getQuestion3()); sqlite3_bind_int(stmt, idx++, form.getQuestion4()); sqlite3_bind_int(stmt, idx++, form.getQuestion5());
    sqlite3_bind_int(stmt, idx++, form.getQuestion6()); sqlite3_bind_int(stmt, idx++, form.getQuestion7()); sqlite3_bind_int(stmt, idx++, form.getQuestion8()); sqlite3_bind_int(stmt, idx++, form.getQuestion9()); sqlite3_bind_int(stmt, idx++, form.getQuestion10());
    sqlite3_bind_int(stmt, idx++, form.getQuestion11()); sqlite3_bind_int(stmt, idx++, form.getQuestion12()); sqlite3_bind_int(stmt, idx++, form.getQuestion13()); sqlite3_bind_int(stmt, idx++, form.getQuestion14()); sqlite3_bind_int(stmt, idx++, form.getQuestion15());
    sqlite3_bind_int(stmt, idx++, form.getQuestion16()); sqlite3_bind_int(stmt, idx++, form.getQuestion17()); sqlite3_bind_int(stmt, idx++, form.getQuestion18()); sqlite3_bind_int(stmt, idx++, form.getQuestion19()); sqlite3_bind_int(stmt, idx++, form.getQuestion20()); sqlite3_bind_int(stmt, idx++, form.getQuestion21());
    sqlite3_bind_int(stmt, idx++, total); sqlite3_bind_text(stmt, idx++, level.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, idx++, form.getBAICreatedAt().toString().c_str(), -1, SQLITE_TRANSIENT); sqlite3_bind_text(stmt, idx++, form.getBAIUpdatedAt().toString().c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok;
}

bool BeckAnxietyInventoryManager::update(const BeckAnxietyInventory &form) {
    int total = computeTotal(form); std::string level = computeSeverity(total); std::string now = utils::getCurrentTimestamp();
    const char* sql = R"SQL(UPDATE beck_anxiety_inventory SET case_profile_id=?,
        question_1=?,question_2=?,question_3=?,question_4=?,question_5=?,question_6=?,question_7=?,question_8=?,question_9=?,question_10=?,
        question_11=?,question_12=?,question_13=?,question_14=?,question_15=?,question_16=?,question_17=?,question_18=?,question_19=?,question_20=?,question_21=?,
        total_score=?,severity_level=?,modified_at=? WHERE id=?;)SQL";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db, sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("BAI update", m_db, sql); return false; } int idx=1;
    sqlite3_bind_int(stmt, idx++, form.getCaseProfileId());
    sqlite3_bind_int(stmt, idx++, form.getQuestion1()); sqlite3_bind_int(stmt, idx++, form.getQuestion2()); sqlite3_bind_int(stmt, idx++, form.getQuestion3()); sqlite3_bind_int(stmt, idx++, form.getQuestion4()); sqlite3_bind_int(stmt, idx++, form.getQuestion5());
    sqlite3_bind_int(stmt, idx++, form.getQuestion6()); sqlite3_bind_int(stmt, idx++, form.getQuestion7()); sqlite3_bind_int(stmt, idx++, form.getQuestion8()); sqlite3_bind_int(stmt, idx++, form.getQuestion9()); sqlite3_bind_int(stmt, idx++, form.getQuestion10());
    sqlite3_bind_int(stmt, idx++, form.getQuestion11()); sqlite3_bind_int(stmt, idx++, form.getQuestion12()); sqlite3_bind_int(stmt, idx++, form.getQuestion13()); sqlite3_bind_int(stmt, idx++, form.getQuestion14()); sqlite3_bind_int(stmt, idx++, form.getQuestion15());
    sqlite3_bind_int(stmt, idx++, form.getQuestion16()); sqlite3_bind_int(stmt, idx++, form.getQuestion17()); sqlite3_bind_int(stmt, idx++, form.getQuestion18()); sqlite3_bind_int(stmt, idx++, form.getQuestion19()); sqlite3_bind_int(stmt, idx++, form.getQuestion20()); sqlite3_bind_int(stmt, idx++, form.getQuestion21());
    sqlite3_bind_int(stmt, idx++, total); sqlite3_bind_text(stmt, idx++, level.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, idx++, now.c_str(), -1, SQLITE_TRANSIENT); sqlite3_bind_int(stmt, idx++, form.getBAIId());
    bool ok = sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok;
}

BeckAnxietyInventory BeckAnxietyInventoryManager::mapRow(sqlite3_stmt* stmt) const {
    int id = sqlite3_column_int(stmt,0); int caseId = sqlite3_column_int(stmt,1); int q[21]; for(int i=0;i<21;++i) q[i]=sqlite3_column_int(stmt,3+i);
    const char* created = reinterpret_cast<const char*>(sqlite3_column_text(stmt,26)); const char* modified = reinterpret_cast<const char*>(sqlite3_column_text(stmt,27));
    return BeckAnxietyInventory(id,caseId,q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],q[8],q[9],q[10],q[11],q[12],q[13],q[14],q[15],q[16],q[17],q[18],q[19],q[20], DateTime::fromString(created?created:""), DateTime::fromString(modified?modified:""));
}

std::optional<BeckAnxietyInventory> BeckAnxietyInventoryManager::getById(int id) const { const char* sql="SELECT * FROM beck_anxiety_inventory WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return std::nullopt; sqlite3_bind_int(stmt,1,id); std::optional<BeckAnxietyInventory> res; if(sqlite3_step(stmt)==SQLITE_ROW) res = mapRow(stmt); sqlite3_finalize(stmt); return res; }

std::vector<BeckAnxietyInventory> BeckAnxietyInventoryManager::listByCase(int caseProfileId) const { std::vector<BeckAnxietyInventory> v; const char* sql="SELECT * FROM beck_anxiety_inventory WHERE case_profile_id=? ORDER BY created_at"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return v; sqlite3_bind_int(stmt,1,caseProfileId); while(sqlite3_step(stmt)==SQLITE_ROW) v.push_back(mapRow(stmt)); sqlite3_finalize(stmt); return v; }

bool BeckAnxietyInventoryManager::deleteById(int id) { const char* sql="DELETE FROM beck_anxiety_inventory WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return false; sqlite3_bind_int(stmt,1,id); bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok; }

int BeckAnxietyInventoryManager::importFromCSV(const std::string &filePath) {
    int success=0, failed=0; bool inTx=false; try{ auto table = csv::CSVReader::readFile(filePath); std::vector<std::string> required={"case_profile_id"}; for(int i=1;i<=21;++i) required.push_back("question_"+std::to_string(i)); for(const auto &h: required){ if(std::find(table.headers.begin(),table.headers.end(),h)==table.headers.end()){ utils::logMessage("ERROR","BAI CSV missing header: "+h); return 0; } } if(sqlite3_exec(m_db,"BEGIN TRANSACTION;",nullptr,nullptr,nullptr)==SQLITE_OK) inTx=true; for(const auto &row: table.rows){ try{ int caseId = std::stoi(csv::safeGet(row,"case_profile_id")); int q[21]; for(int i=0;i<21;++i){ std::string v=csv::safeGet(row,"question_"+std::to_string(i+1)); q[i]= v.empty()?0:std::stoi(v); if(q[i]<0||q[i]>3) q[i]=0; } std::string created = csv::safeGet(row,"created_at"); if(created.empty()) created = utils::getCurrentTimestamp(); DateTime dt = DateTime::fromString(csv::normalizeTimestampForDateTime(created)); BeckAnxietyInventory form(BeckAnxietyInventory::getNextId(), caseId,q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],q[8],q[9],q[10],q[11],q[12],q[13],q[14],q[15],q[16],q[17],q[18],q[19],q[20], dt, dt); if(!create(form)) failed++; else success++; } catch(const std::exception &e){ failed++; utils::logMessage("ERROR", std::string("BAI CSV row error: ")+e.what()); } } if(inTx){ if(sqlite3_exec(m_db,"COMMIT;",nullptr,nullptr,nullptr)!=SQLITE_OK) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr);} } catch(const std::exception &e){ if(inTx) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr); utils::logMessage("ERROR", std::string("BAI CSV file error: ")+e.what()); } utils::logMessage("INFO","BeckAnxietyInventoryManager::importFromCSV success="+std::to_string(success)+", failed="+std::to_string(failed)); return success; }
