// Corrected implementation aligning with SCL90R interface and table schema ordering
#include "managers/SCL90RManager.h"
#include "core/Utils.h"
#include "utils/DbLogging.h"
#include <algorithm>
#include <sstream>

using namespace SilverClinic;
using namespace SilverClinic::Forms;

// Table column order reference for scl90r:
// 0 id
// 1 case_profile_id
// 2 type
// 3..92 question_1..question_90 (90 cols)
// 93 gsi
// 94 pst
// 95 psdi
// 96 severity_level
// 97 created_at
// 98 modified_at

bool SCL90RManager::create(const SCL90R &form) {
    std::stringstream ss;
    ss << "INSERT INTO scl90r(id,case_profile_id,type";
    for(int i=1;i<=90;++i) ss << ",question_"<<i;
    ss << ",gsi,pst,psdi,severity_level,created_at,modified_at) VALUES(?,?,?";
    for(int i=1;i<=90;++i) ss << ",?";
    ss << ",0,0,0.0,'Minimal',?,?);";
    std::string sql = ss.str();

    sqlite3_stmt* stmt=nullptr;
    if(sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr)!=SQLITE_OK){ utils::logDbPrepareError("SCL90R create", m_db, sql.c_str()); return false; }
    int idx=1;
    sqlite3_bind_int(stmt,idx++,form.getSCLId());
    sqlite3_bind_int(stmt,idx++,form.getCaseProfileId());
    sqlite3_bind_text(stmt,idx++,form.getType().c_str(),-1,SQLITE_TRANSIENT);
    for(int i=1;i<=90;++i) sqlite3_bind_int(stmt,idx++,form.getQuestion(i));
    sqlite3_bind_text(stmt,idx++,form.getCreatedAt().toString().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,form.getUpdatedAt().toString().c_str(),-1,SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt)==SQLITE_DONE;
    sqlite3_finalize(stmt);
    if(ok) computeAndPersistDerived(form.getSCLId());
    return ok;
}

bool SCL90RManager::update(const SCL90R &form) {
    std::stringstream ss; ss << "UPDATE scl90r SET case_profile_id=?"; for(int i=1;i<=90;++i) ss << ",question_"<<i<<"=?"; ss << ",modified_at=? WHERE id=?"; std::string sql=ss.str();
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("SCL90R update", m_db, sql.c_str()); return false; } int idx=1;
    sqlite3_bind_int(stmt,idx++,form.getCaseProfileId());
    for(int i=1;i<=90;++i) sqlite3_bind_int(stmt,idx++,form.getQuestion(i));
    std::string now=utils::getCurrentTimestamp();
    sqlite3_bind_text(stmt,idx++,now.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,idx++,form.getSCLId());
    bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt);
    if(ok) computeAndPersistDerived(form.getSCLId());
    return ok;
}

SCL90R SCL90RManager::mapRow(sqlite3_stmt* stmt) const {
    int id=sqlite3_column_int(stmt,0);
    int caseId=sqlite3_column_int(stmt,1);
    // Extract 90 question values
    int q[90];
    for(int i=0;i<90;++i) q[i]=sqlite3_column_int(stmt,3+i);
    const char* created=reinterpret_cast<const char*>(sqlite3_column_text(stmt,97));
    const char* modified=reinterpret_cast<const char*>(sqlite3_column_text(stmt,98));
    // Use full constructor with all question values + timestamps
    SCL90R form(
        id, caseId,
        q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],q[8],q[9],
        q[10],q[11],q[12],q[13],q[14],q[15],q[16],q[17],q[18],q[19],
        q[20],q[21],q[22],q[23],q[24],q[25],q[26],q[27],q[28],q[29],
        q[30],q[31],q[32],q[33],q[34],q[35],q[36],q[37],q[38],q[39],
        q[40],q[41],q[42],q[43],q[44],q[45],q[46],q[47],q[48],q[49],
        q[50],q[51],q[52],q[53],q[54],q[55],q[56],q[57],q[58],q[59],
        q[60],q[61],q[62],q[63],q[64],q[65],q[66],q[67],q[68],q[69],
        q[70],q[71],q[72],q[73],q[74],q[75],q[76],q[77],q[78],q[79],
        q[80],q[81],q[82],q[83],q[84],q[85],q[86],q[87],q[88],q[89],
        DateTime::fromString(created?created:""),
        DateTime::fromString(modified?modified:"")
    );
    return form;
}

std::optional<SCL90R> SCL90RManager::getById(int id) const { const char* sql="SELECT * FROM scl90r WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return std::nullopt; sqlite3_bind_int(stmt,1,id); std::optional<SCL90R> r; if(sqlite3_step(stmt)==SQLITE_ROW) r=mapRow(stmt); sqlite3_finalize(stmt); return r; }

std::vector<SCL90R> SCL90RManager::listByCase(int caseProfileId) const { std::vector<SCL90R> v; const char* sql="SELECT * FROM scl90r WHERE case_profile_id=? ORDER BY created_at"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return v; sqlite3_bind_int(stmt,1,caseProfileId); while(sqlite3_step(stmt)==SQLITE_ROW) v.push_back(mapRow(stmt)); sqlite3_finalize(stmt); return v; }

bool SCL90RManager::deleteById(int id) { const char* sql="DELETE FROM scl90r WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return false; sqlite3_bind_int(stmt,1,id); bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok; }

int SCL90RManager::importFromCSV(const std::string &filePath) {
    int success=0, failed=0; bool inTx=false;
    try{
        auto table=csv::CSVReader::readFile(filePath);
        std::vector<std::string> required={"case_profile_id"};
        for(int i=1;i<=90;++i) required.push_back("question_"+std::to_string(i));
    for(const auto &h: required){ if(std::find(table.headers.begin(),table.headers.end(),h)==table.headers.end()){ utils::LogEventContext ctx{"IMPORT","validate","SCL90R", std::nullopt, std::nullopt}; utils::logStructured(utils::LogLevel::ERROR, ctx, std::string("CSV missing header: ")+h); return 0; } }
        if(sqlite3_exec(m_db,"BEGIN TRANSACTION;",nullptr,nullptr,nullptr)==SQLITE_OK) inTx=true;
        for(const auto &row: table.rows){
            try{
                int caseId=std::stoi(csv::safeGet(row,"case_profile_id"));
                // Build question array
                int q[90];
                for(int i=0;i<90;++i){ std::string v=csv::safeGet(row,"question_"+std::to_string(i+1)); int val= v.empty()?0:std::stoi(v); if(val<0||val>3) val=0; q[i]=val; }
                std::string created = csv::safeGet(row,"created_at"); if(created.empty()) created = utils::getCurrentTimestamp(); DateTime dt = DateTime::fromString(csv::normalizeTimestampForDateTime(created));
                SCL90R form(
                    SCL90R::getNextId(), caseId,
                    q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],q[8],q[9],
                    q[10],q[11],q[12],q[13],q[14],q[15],q[16],q[17],q[18],q[19],
                    q[20],q[21],q[22],q[23],q[24],q[25],q[26],q[27],q[28],q[29],
                    q[30],q[31],q[32],q[33],q[34],q[35],q[36],q[37],q[38],q[39],
                    q[40],q[41],q[42],q[43],q[44],q[45],q[46],q[47],q[48],q[49],
                    q[50],q[51],q[52],q[53],q[54],q[55],q[56],q[57],q[58],q[59],
                    q[60],q[61],q[62],q[63],q[64],q[65],q[66],q[67],q[68],q[69],
                    q[70],q[71],q[72],q[73],q[74],q[75],q[76],q[77],q[78],q[79],
                    q[80],q[81],q[82],q[83],q[84],q[85],q[86],q[87],q[88],q[89],
                    dt, dt);
                if(!create(form)) failed++; else success++;
            } catch(const std::exception &e){ failed++; utils::LogEventContext ctx{"IMPORT","row_error","SCL90R", std::nullopt, std::nullopt}; utils::logStructured(utils::LogLevel::ERROR, ctx, std::string("CSV row error: ")+e.what()); }
        }
        if(inTx){ if(sqlite3_exec(m_db,"COMMIT;",nullptr,nullptr,nullptr)!=SQLITE_OK) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr);} }
    catch(const std::exception &e){ if(inTx) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr); utils::LogEventContext ctx{"IMPORT","file_error","SCL90R", std::nullopt, std::nullopt}; utils::logStructured(utils::LogLevel::ERROR, ctx, std::string("CSV file error: ")+e.what()); }
    { utils::LogEventContext ctx{"IMPORT","summary","SCL90R", std::nullopt, std::nullopt}; utils::logStructured(utils::LogLevel::INFO, ctx, std::string("importFromCSV success=")+std::to_string(success)+", failed="+std::to_string(failed)); }
    return success; }

void SCL90RManager::computeAndPersistDerived(int id) {
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,"SELECT * FROM scl90r WHERE id=?",-1,&stmt,nullptr)!=SQLITE_OK) return; sqlite3_bind_int(stmt,1,id);
    if(sqlite3_step(stmt)==SQLITE_ROW){
        SCL90R form = mapRow(stmt);
        int gsi = form.getGlobalSeverityIndex();
        int pst = form.getPositiveSymptomTotal();
        double psdi = form.getPositiveSymptomDistressIndex();
        std::string level = form.getSeverityLevel();
        sqlite3_finalize(stmt);
        sqlite3_stmt* upd=nullptr; if(sqlite3_prepare_v2(m_db,"UPDATE scl90r SET gsi=?,pst=?,psdi=?,severity_level=? WHERE id=?",-1,&upd,nullptr)!=SQLITE_OK) return;
        sqlite3_bind_int(upd,1,gsi); sqlite3_bind_int(upd,2,pst); sqlite3_bind_double(upd,3,psdi); sqlite3_bind_text(upd,4,level.c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_int(upd,5,id);
        sqlite3_step(upd); sqlite3_finalize(upd);
    } else { sqlite3_finalize(stmt); }
}
