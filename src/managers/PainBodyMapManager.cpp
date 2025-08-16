#include "managers/PainBodyMapManager.h"
#include "core/Utils.h"
#include "utils/DbLogging.h"

using namespace SilverClinic;
using namespace SilverClinic::Forms;

bool PainBodyMapManager::create(const PainBodyMap &form) {
    const char* sql = R"SQL(INSERT INTO pain_body_map(id,case_profile_id,type,pain_data_json,additional_comments,created_at,modified_at)
        VALUES(?,?,?,?,?,?,?);)SQL";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("PBM create", m_db, sql); return false; } int idx=1;
    sqlite3_bind_int(stmt,idx++,form.getPBMId()); sqlite3_bind_int(stmt,idx++,form.getCaseProfileId()); sqlite3_bind_text(stmt,idx++,form.getType().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,form.getPainDataJson().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,form.getAdditionalComments().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,form.getPBMCreatedAt().toString().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,form.getPBMUpdatedAt().toString().c_str(),-1,SQLITE_TRANSIENT);
    bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok;
}

bool PainBodyMapManager::update(const PainBodyMap &form) {
    const char* sql = R"SQL(UPDATE pain_body_map SET case_profile_id=?,pain_data_json=?,additional_comments=?,modified_at=? WHERE id=?;)SQL";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("PBM update", m_db, sql); return false; } int idx=1;
    sqlite3_bind_int(stmt,idx++,form.getCaseProfileId()); sqlite3_bind_text(stmt,idx++,form.getPainDataJson().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,form.getAdditionalComments().c_str(),-1,SQLITE_TRANSIENT); std::string now = utils::getCurrentTimestamp(); sqlite3_bind_text(stmt,idx++,now.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,idx++,form.getPBMId()); bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok;
}

PainBodyMap PainBodyMapManager::mapRow(sqlite3_stmt* stmt) const {
    int id = sqlite3_column_int(stmt,0); int caseId = sqlite3_column_int(stmt,1);
    const char* pain = reinterpret_cast<const char*>(sqlite3_column_text(stmt,3)); const char* addc = reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
    const char* created = reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)); const char* modified= reinterpret_cast<const char*>(sqlite3_column_text(stmt,6));
    return PainBodyMap(id, caseId, pain?pain:"{}", addc?addc:"", DateTime::fromString(created?created:""), DateTime::fromString(modified?modified:""));
}

std::optional<PainBodyMap> PainBodyMapManager::getById(int id) const { const char* sql="SELECT * FROM pain_body_map WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return std::nullopt; sqlite3_bind_int(stmt,1,id); std::optional<PainBodyMap> res; if(sqlite3_step(stmt)==SQLITE_ROW) res=mapRow(stmt); sqlite3_finalize(stmt); return res; }

std::vector<PainBodyMap> PainBodyMapManager::listByCase(int caseProfileId) const { std::vector<PainBodyMap> v; const char* sql="SELECT * FROM pain_body_map WHERE case_profile_id=? ORDER BY created_at"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return v; sqlite3_bind_int(stmt,1,caseProfileId); while(sqlite3_step(stmt)==SQLITE_ROW) v.push_back(mapRow(stmt)); sqlite3_finalize(stmt); return v; }

bool PainBodyMapManager::deleteById(int id) { const char* sql="DELETE FROM pain_body_map WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return false; sqlite3_bind_int(stmt,1,id); bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok; }

int PainBodyMapManager::importFromCSV(const std::string &filePath) {
    int success=0, failed=0; bool inTx=false; try{ auto table = csv::CSVReader::readFile(filePath); std::vector<std::string> required={"case_profile_id","pain_data_json"}; for(const auto &h: required){ if(std::find(table.headers.begin(),table.headers.end(),h)==table.headers.end()){ utils::logMessage("ERROR","PBM CSV missing header: "+h); return 0; } } if(sqlite3_exec(m_db,"BEGIN TRANSACTION;",nullptr,nullptr,nullptr)==SQLITE_OK) inTx=true; for(const auto &row: table.rows){ try{ int caseId = std::stoi(csv::safeGet(row,"case_profile_id")); std::string json = csv::safeGet(row,"pain_data_json"); if(json.empty()) json="{}"; std::string comments = csv::safeGet(row,"additional_comments"); std::string created = csv::safeGet(row,"created_at"); if(created.empty()) created = utils::getCurrentTimestamp(); DateTime dt = DateTime::fromString(csv::normalizeTimestampForDateTime(created)); PainBodyMap form(PainBodyMap::getNextId(), caseId, json, comments, dt, dt); if(!create(form)) failed++; else success++; } catch(const std::exception &e){ failed++; utils::logMessage("ERROR", std::string("PBM CSV row error: ")+e.what()); } } if(inTx){ if(sqlite3_exec(m_db,"COMMIT;",nullptr,nullptr,nullptr)!=SQLITE_OK) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr);} } catch(const std::exception &e){ if(inTx) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr); utils::logMessage("ERROR", std::string("PBM CSV file error: ")+e.what()); } utils::logMessage("INFO","PainBodyMapManager::importFromCSV success="+std::to_string(success)+", failed="+std::to_string(failed)); return success; }
