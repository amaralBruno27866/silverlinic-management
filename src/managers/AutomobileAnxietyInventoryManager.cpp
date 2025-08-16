#include "managers/AutomobileAnxietyInventoryManager.h"
#include "core/Utils.h"
#include "utils/DbLogging.h"
#include <sstream>

using namespace SilverClinic;
using namespace SilverClinic::Forms;

bool AutomobileAnxietyInventoryManager::create(const AutomobileAnxietyInventory &form){
    const char* sql = R"SQL(INSERT INTO automobile_anxiety_inventory(
        id, case_profile_id, type,
        q1,q2,q3,q4,q5,q6,q7,q8,q9,q10,q11,q12,q13,
        q14_driver,q14_passenger,q14_no_difference,
        q15_a,q15_b,
        q16,q17,q18,
        q19,q19_sidewalks,q19_crossing,q19_both,
        q20,q21,q22,q23,
        created_at, modified_at
    ) VALUES(
        ?, ?, ?,               -- id, case_profile_id, type
        ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,        -- q1..q13 (13)
        ?, ?, ?,               -- q14 variants (3)
        ?, ?,                   -- q15_a, q15_b
        ?, ?, ?,                -- q16,q17,q18
        ?, ?, ?, ?,             -- q19 group (4)
        ?, ?, ?, ?,             -- q20..q23 (4)
        ?, ?                    -- created_at, modified_at
    );)SQL";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("AAI create", m_db, sql); return false; }
    int idx=1; sqlite3_bind_int(stmt,idx++,form.getAAIId()); sqlite3_bind_int(stmt,idx++,form.getCaseProfileId()); sqlite3_bind_text(stmt,idx++,form.getType().c_str(),-1,SQLITE_TRANSIENT);
#define BIND_BOOL(b) sqlite3_bind_int(stmt,idx++,(b)?1:0)
    BIND_BOOL(form.getQuestion1()); BIND_BOOL(form.getQuestion2()); BIND_BOOL(form.getQuestion3()); BIND_BOOL(form.getQuestion4()); BIND_BOOL(form.getQuestion5());
    BIND_BOOL(form.getQuestion6()); BIND_BOOL(form.getQuestion7()); BIND_BOOL(form.getQuestion8()); BIND_BOOL(form.getQuestion9()); BIND_BOOL(form.getQuestion10());
    BIND_BOOL(form.getQuestion11()); BIND_BOOL(form.getQuestion12()); BIND_BOOL(form.getQuestion13());
    BIND_BOOL(form.getQuestion14Driver()); BIND_BOOL(form.getQuestion14Passenger()); BIND_BOOL(form.getQuestion14NoDifference());
    BIND_BOOL(form.getQuestion15A()); sqlite3_bind_text(stmt,idx++,form.getQuestion15B().c_str(),-1,SQLITE_TRANSIENT);
    BIND_BOOL(form.getQuestion16()); BIND_BOOL(form.getQuestion17()); BIND_BOOL(form.getQuestion18());
    BIND_BOOL(form.getQuestion19()); BIND_BOOL(form.getQuestion19Sidewalks()); BIND_BOOL(form.getQuestion19Crossing()); BIND_BOOL(form.getQuestion19Both());
    BIND_BOOL(form.getQuestion20()); BIND_BOOL(form.getQuestion21()); BIND_BOOL(form.getQuestion22()); BIND_BOOL(form.getQuestion23());
    sqlite3_bind_text(stmt,idx++,form.getAAICreatedAt().toString().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,form.getAAIUpdatedAt().toString().c_str(),-1,SQLITE_TRANSIENT);
#undef BIND_BOOL
    bool ok = sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok; }

bool AutomobileAnxietyInventoryManager::update(const AutomobileAnxietyInventory &form){
    const char* sql = R"SQL(UPDATE automobile_anxiety_inventory SET
        case_profile_id=?,
        q1=?,q2=?,q3=?,q4=?,q5=?,q6=?,q7=?,q8=?,q9=?,q10=?,q11=?,q12=?,q13=?,
        q14_driver=?,q14_passenger=?,q14_no_difference=?,
        q15_a=?,q15_b=?,
        q16=?,q17=?,q18=?,
        q19=?,q19_sidewalks=?,q19_crossing=?,q19_both=?,
        q20=?,q21=?,q22=?,q23=?,
        modified_at=?
    WHERE id=?;)SQL";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("AAI update", m_db, sql); return false; }
    int idx=1; sqlite3_bind_int(stmt,idx++,form.getCaseProfileId());
#define BIND_BOOL(b) sqlite3_bind_int(stmt,idx++,(b)?1:0)
    BIND_BOOL(form.getQuestion1()); BIND_BOOL(form.getQuestion2()); BIND_BOOL(form.getQuestion3()); BIND_BOOL(form.getQuestion4()); BIND_BOOL(form.getQuestion5());
    BIND_BOOL(form.getQuestion6()); BIND_BOOL(form.getQuestion7()); BIND_BOOL(form.getQuestion8()); BIND_BOOL(form.getQuestion9()); BIND_BOOL(form.getQuestion10());
    BIND_BOOL(form.getQuestion11()); BIND_BOOL(form.getQuestion12()); BIND_BOOL(form.getQuestion13());
    BIND_BOOL(form.getQuestion14Driver()); BIND_BOOL(form.getQuestion14Passenger()); BIND_BOOL(form.getQuestion14NoDifference());
    BIND_BOOL(form.getQuestion15A()); sqlite3_bind_text(stmt,idx++,form.getQuestion15B().c_str(),-1,SQLITE_TRANSIENT);
    BIND_BOOL(form.getQuestion16()); BIND_BOOL(form.getQuestion17()); BIND_BOOL(form.getQuestion18());
    BIND_BOOL(form.getQuestion19()); BIND_BOOL(form.getQuestion19Sidewalks()); BIND_BOOL(form.getQuestion19Crossing()); BIND_BOOL(form.getQuestion19Both());
    BIND_BOOL(form.getQuestion20()); BIND_BOOL(form.getQuestion21()); BIND_BOOL(form.getQuestion22()); BIND_BOOL(form.getQuestion23());
    std::string now = utils::getCurrentTimestamp(); sqlite3_bind_text(stmt,idx++,now.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,idx++,form.getAAIId());
#undef BIND_BOOL
    bool ok = sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok; }

AutomobileAnxietyInventory AutomobileAnxietyInventoryManager::mapRow(sqlite3_stmt* stmt) const {
    int col=0; int id = sqlite3_column_int(stmt,col++); int caseId=sqlite3_column_int(stmt,col++); sqlite3_column_text(stmt,col++); // skip type (not currently stored in object)
#define COL_BOOL() (sqlite3_column_int(stmt,col++)!=0)
    bool q1=COL_BOOL(); bool q2=COL_BOOL(); bool q3=COL_BOOL(); bool q4=COL_BOOL(); bool q5=COL_BOOL(); bool q6=COL_BOOL(); bool q7=COL_BOOL(); bool q8=COL_BOOL(); bool q9=COL_BOOL(); bool q10=COL_BOOL();
    bool q11=COL_BOOL(); bool q12=COL_BOOL(); bool q13=COL_BOOL(); bool q14_driver=COL_BOOL(); bool q14_passenger=COL_BOOL(); bool q14_no_diff=COL_BOOL(); bool q15_a=COL_BOOL();
    const unsigned char* q15_b_txt = sqlite3_column_text(stmt,col++);
    bool q16=COL_BOOL(); bool q17=COL_BOOL(); bool q18=COL_BOOL(); bool q19=COL_BOOL(); bool q19_sidewalks=COL_BOOL(); bool q19_crossing=COL_BOOL(); bool q19_both=COL_BOOL();
    bool q20=COL_BOOL(); bool q21=COL_BOOL(); bool q22=COL_BOOL(); bool q23=COL_BOOL();
    const unsigned char* createdTxt = sqlite3_column_text(stmt,col++); const unsigned char* modifiedTxt = sqlite3_column_text(stmt,col++);
#undef COL_BOOL
    AutomobileAnxietyInventory form(id, caseId,
        q1,q2,q3,q4,q5,q6,q7,q8,q9,q10,q11,q12,q13,
        q14_driver,q14_passenger,q14_no_diff,
        q15_a, std::string(reinterpret_cast<const char*>(q15_b_txt?q15_b_txt:(const unsigned char*)"")),
        q16,q17,q18,
        q19,q19_sidewalks,q19_crossing,q19_both,
        q20,q21,q22,q23,
        DateTime::fromString(reinterpret_cast<const char*>(createdTxt?createdTxt:(const unsigned char*)"")),
        DateTime::fromString(reinterpret_cast<const char*>(modifiedTxt?modifiedTxt:(const unsigned char*)"")));
    return form; }

std::optional<AutomobileAnxietyInventory> AutomobileAnxietyInventoryManager::getById(int id) const {
    const char* sql = "SELECT * FROM automobile_anxiety_inventory WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return std::nullopt; sqlite3_bind_int(stmt,1,id); std::optional<AutomobileAnxietyInventory> res; if(sqlite3_step(stmt)==SQLITE_ROW) res=mapRow(stmt); sqlite3_finalize(stmt); return res; }

std::vector<AutomobileAnxietyInventory> AutomobileAnxietyInventoryManager::listByCase(int caseProfileId) const { std::vector<AutomobileAnxietyInventory> v; const char* sql="SELECT * FROM automobile_anxiety_inventory WHERE case_profile_id=? ORDER BY created_at"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return v; sqlite3_bind_int(stmt,1,caseProfileId); while(sqlite3_step(stmt)==SQLITE_ROW) v.push_back(mapRow(stmt)); sqlite3_finalize(stmt); return v; }

bool AutomobileAnxietyInventoryManager::deleteById(int id){ const char* sql="DELETE FROM automobile_anxiety_inventory WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return false; sqlite3_bind_int(stmt,1,id); bool ok= sqlite3_step(stmt)==SQLITE_DONE; sqlite3_finalize(stmt); return ok; }

int AutomobileAnxietyInventoryManager::importFromCSV(const std::string &filePath){ int success=0, failed=0; bool inTx=false; try{ auto table=csv::CSVReader::readFile(filePath); std::vector<std::string> required={"case_profile_id"};
    // For questions: require 1-13 and 16-23; question 14 is represented by three variant columns (driver/passenger/no_difference)
    for(int i=1;i<=13;++i) required.push_back("question_"+std::to_string(i));
    for(int i=16;i<=23;++i) required.push_back("question_"+std::to_string(i));
    std::vector<std::string> optionals={"question_14_driver","question_14_passenger","question_14_no_difference","question_15_b","question_19_sidewalks","question_19_crossing","question_19_both"};
    for(const auto &h: required){ if(std::find(table.headers.begin(),table.headers.end(),h)==table.headers.end()){ utils::logMessage("ERROR","AAI CSV missing header: "+h); return 0; } }
    bool hasQ14Driver = std::find(table.headers.begin(),table.headers.end(),"question_14_driver")!=table.headers.end();
    bool hasQ14Passenger = std::find(table.headers.begin(),table.headers.end(),"question_14_passenger")!=table.headers.end();
    bool hasQ14NoDiff = std::find(table.headers.begin(),table.headers.end(),"question_14_no_difference")!=table.headers.end();
    if(!hasQ14Driver && !hasQ14Passenger && !hasQ14NoDiff){ utils::logMessage("INFO","AAI CSV: no question 14 variant columns present (driver/passenger/no_difference)"); }
    if(sqlite3_exec(m_db,"BEGIN TRANSACTION;",nullptr,nullptr,nullptr)==SQLITE_OK) inTx=true;
    for(const auto &row: table.rows){ try{ int caseId=std::stoi(csv::safeGet(row,"case_profile_id")); AutomobileAnxietyInventory form(caseId); // set boolean questions
            // Simple questions (1-13,16-18,19 yes/no,20-23) map to question_X columns (treat non-empty & not 0 as yes)
            auto readBool=[&](int q){ std::string v=csv::safeGet(row,"question_"+std::to_string(q)); if(v.empty()) return false; return v!="0"; };
            form.setQuestion1(readBool(1)); form.setQuestion2(readBool(2)); form.setQuestion3(readBool(3)); form.setQuestion4(readBool(4)); form.setQuestion5(readBool(5)); form.setQuestion6(readBool(6)); form.setQuestion7(readBool(7)); form.setQuestion8(readBool(8)); form.setQuestion9(readBool(9)); form.setQuestion10(readBool(10)); form.setQuestion11(readBool(11)); form.setQuestion12(readBool(12)); form.setQuestion13(readBool(13));
            // Question 14 options
            if(std::find(table.headers.begin(),table.headers.end(),"question_14_driver")!=table.headers.end()) form.setQuestion14Driver(csv::safeGet(row,"question_14_driver")=="1");
            if(std::find(table.headers.begin(),table.headers.end(),"question_14_passenger")!=table.headers.end()) form.setQuestion14Passenger(csv::safeGet(row,"question_14_passenger")=="1");
            if(std::find(table.headers.begin(),table.headers.end(),"question_14_no_difference")!=table.headers.end()) form.setQuestion14NoDifference(csv::safeGet(row,"question_14_no_difference")=="1");
            form.setQuestion15A(readBool(15)); if(std::find(table.headers.begin(),table.headers.end(),"question_15_b")!=table.headers.end()) form.setQuestion15B(csv::safeGet(row,"question_15_b"));
            form.setQuestion16(readBool(16)); form.setQuestion17(readBool(17)); form.setQuestion18(readBool(18)); form.setQuestion19(readBool(19));
            if(std::find(table.headers.begin(),table.headers.end(),"question_19_sidewalks")!=table.headers.end()) form.setQuestion19Sidewalks(csv::safeGet(row,"question_19_sidewalks")=="1");
            if(std::find(table.headers.begin(),table.headers.end(),"question_19_crossing")!=table.headers.end()) form.setQuestion19Crossing(csv::safeGet(row,"question_19_crossing")=="1");
            if(std::find(table.headers.begin(),table.headers.end(),"question_19_both")!=table.headers.end()) form.setQuestion19Both(csv::safeGet(row,"question_19_both")=="1");
            form.setQuestion20(readBool(20)); form.setQuestion21(readBool(21)); form.setQuestion22(readBool(22)); form.setQuestion23(readBool(23));
            std::string created = csv::safeGet(row,"created_at"); if(created.empty()) created = utils::getCurrentTimestamp(); // timestamps handled internally
            if(!create(form)) failed++; else success++; } catch(const std::exception &e){ failed++; utils::logMessage("ERROR", std::string("AAI CSV row error: ")+e.what()); } }
    if(inTx){ if(sqlite3_exec(m_db,"COMMIT;",nullptr,nullptr,nullptr)!=SQLITE_OK) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr);} }
    catch(const std::exception &e){ if(inTx) sqlite3_exec(m_db,"ROLLBACK;",nullptr,nullptr,nullptr); utils::logMessage("ERROR", std::string("AAI CSV file error: ")+e.what()); }
    utils::logMessage("INFO","AutomobileAnxietyInventoryManager::importFromCSV success="+std::to_string(success)+", failed="+std::to_string(failed)); return success; }
