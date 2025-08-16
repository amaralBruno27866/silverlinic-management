#include <cassert>
#include <iostream>
#include <sqlite3.h>
#include "managers/AddressManager.h"
#include "core/Address.h"
#include "core/Utils.h"

using namespace SilverClinic;

int main(){
    sqlite3* db=nullptr; sqlite3_open(":memory:", &db);
    const char* createSql = "CREATE TABLE address(id INTEGER PRIMARY KEY,user_key INTEGER NOT NULL,street TEXT,city TEXT,province TEXT,postal_code TEXT,created_at TEXT NOT NULL,modified_at TEXT NOT NULL)";
    char* err=nullptr; int rc = sqlite3_exec(db, createSql, nullptr, nullptr, &err); if(rc!=SQLITE_OK){ std::cerr<<"Failed create table: "<<(err?err:"")<<std::endl; return 1; }
    AddressManager mgr(db);
    Address a(Address::getNextAddressId(), 500001, "123 Main St", "Toronto", "ON", "M5V1A1");
    bool created = mgr.create(a); assert(created);
    auto fetched = mgr.getById(a.getAddressId()); assert(fetched.has_value()); assert(fetched->getCity()=="TORONTO");
    auto list = mgr.listByUser(500001); assert(list.size()==1);
    // Update
    Address updated = *fetched; updated.setCity("Ottawa"); bool up = mgr.update(updated); assert(up); auto fetched2 = mgr.getById(a.getAddressId()); assert(fetched2->getCity()=="OTTAWA");
    // Delete
    bool del = mgr.deleteById(a.getAddressId()); assert(del); auto afterDel = mgr.getById(a.getAddressId()); assert(!afterDel.has_value());
    std::cout << "AddressManager tests passed" << std::endl; sqlite3_close(db); return 0; }
