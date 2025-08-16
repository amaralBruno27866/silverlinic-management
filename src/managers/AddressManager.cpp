#include "managers/AddressManager.h"
#include "core/Utils.h"
#include <string>

using namespace std;
using namespace SilverClinic;

bool AddressManager::create(const Address& addr){
    const char* sql = "INSERT INTO address(id,user_key,street,city,province,postal_code,created_at,modified_at) VALUES(?,?,?,?,?,?,?,?)";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("Address create", m_db, sql); return false; }
    int idx=1; sqlite3_bind_int(stmt,idx++,addr.getAddressId()); sqlite3_bind_int(stmt,idx++,addr.getUserKey());
    sqlite3_bind_text(stmt,idx++,addr.getStreet().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,addr.getCity().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,addr.getProvince().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,addr.getPostalCode().c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,idx++,addr.getCreatedAt().toString().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,addr.getUpdatedAt().toString().c_str(),-1,SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt)==SQLITE_DONE; if(!ok) utils::logDbStepError("Address create", m_db); sqlite3_finalize(stmt); return ok; }

optional<Address> AddressManager::getById(int id) const {
    const char* sql = "SELECT id,user_key,street,city,province,postal_code,created_at,modified_at FROM address WHERE id=?";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("Address getById", m_db, sql); return nullopt; }
    sqlite3_bind_int(stmt,1,id); optional<Address> res; int rc=sqlite3_step(stmt); if(rc==SQLITE_ROW){
        int idx=0; int aid=sqlite3_column_int(stmt,idx++); int user=sqlite3_column_int(stmt,idx++);
        string street = (const char*)sqlite3_column_text(stmt,idx++); string city=(const char*)sqlite3_column_text(stmt,idx++);
        string prov=(const char*)sqlite3_column_text(stmt,idx++); string postal=(const char*)sqlite3_column_text(stmt,idx++);
        string created=(const char*)sqlite3_column_text(stmt,idx++); string modified=(const char*)sqlite3_column_text(stmt,idx++);
        Address a(aid,user,street,city,prov,postal, DateTime::fromString(created), DateTime::fromString(modified)); res=a;
    } else if(rc!=SQLITE_DONE){ utils::logDbStepError("Address getById", m_db); }
    sqlite3_finalize(stmt); return res; }

vector<Address> AddressManager::listByUser(int userKey) const {
    vector<Address> v; const char* sql = "SELECT id,user_key,street,city,province,postal_code,created_at,modified_at FROM address WHERE user_key=? ORDER BY created_at";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("Address listByUser", m_db, sql); return v; }
    sqlite3_bind_int(stmt,1,userKey); int rc; while((rc=sqlite3_step(stmt))==SQLITE_ROW){ int idx=0; int aid=sqlite3_column_int(stmt,idx++); int user=sqlite3_column_int(stmt,idx++); string street=(const char*)sqlite3_column_text(stmt,idx++); string city=(const char*)sqlite3_column_text(stmt,idx++); string prov=(const char*)sqlite3_column_text(stmt,idx++); string postal=(const char*)sqlite3_column_text(stmt,idx++); string created=(const char*)sqlite3_column_text(stmt,idx++); string modified=(const char*)sqlite3_column_text(stmt,idx++); v.emplace_back(aid,user,street,city,prov,postal,DateTime::fromString(created),DateTime::fromString(modified)); }
    if(rc!=SQLITE_DONE){ utils::logDbStepError("Address listByUser", m_db);} sqlite3_finalize(stmt); return v; }

bool AddressManager::update(const Address& addr){
    const char* sql = "UPDATE address SET user_key=?,street=?,city=?,province=?,postal_code=?,modified_at=? WHERE id=?";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("Address update", m_db, sql); return false; }
    int idx=1; sqlite3_bind_int(stmt,idx++,addr.getUserKey()); sqlite3_bind_text(stmt,idx++,addr.getStreet().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,addr.getCity().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,addr.getProvince().c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_text(stmt,idx++,addr.getPostalCode().c_str(),-1,SQLITE_TRANSIENT); string now=utils::getCurrentTimestamp(); sqlite3_bind_text(stmt,idx++,now.c_str(),-1,SQLITE_TRANSIENT); sqlite3_bind_int(stmt,idx++,addr.getAddressId()); bool ok= sqlite3_step(stmt)==SQLITE_DONE; if(!ok) utils::logDbStepError("Address update", m_db); sqlite3_finalize(stmt); return ok; }

bool AddressManager::deleteById(int id){
    const char* sql = "DELETE FROM address WHERE id=?"; sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(m_db,sql,-1,&stmt,nullptr)!=SQLITE_OK){ utils::logDbPrepareError("Address delete", m_db, sql); return false; } sqlite3_bind_int(stmt,1,id); bool ok= sqlite3_step(stmt)==SQLITE_DONE; if(!ok) utils::logDbStepError("Address delete", m_db); sqlite3_finalize(stmt); return ok; }
