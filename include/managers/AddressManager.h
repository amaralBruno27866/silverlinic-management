#ifndef ADDRESS_MANAGER_H
#define ADDRESS_MANAGER_H

#include <vector>
#include <optional>
#include <sqlite3.h>
#include "core/Address.h"
#include "utils/DbLogging.h"

namespace SilverClinic {
class AddressManager {
    sqlite3* m_db;
public:
    explicit AddressManager(sqlite3* db): m_db(db) {}
    // Returns id of created record (>0) or existing id if duplicate (>0), or <=0 on error
    int create(const Address& addr);
    std::optional<Address> getById(int id) const;
    std::vector<Address> listByUser(int userKey) const;
    bool update(const Address& addr);
    bool deleteById(int id);
    // Return existing address id for same user and normalized street+postal
    std::optional<int> findExistingAddressId(const Address& addr) const;
};
}

#endif
