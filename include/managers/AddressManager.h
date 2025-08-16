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
    bool create(const Address& addr);
    std::optional<Address> getById(int id) const;
    std::vector<Address> listByUser(int userKey) const;
    bool update(const Address& addr);
    bool deleteById(int id);
};
}

#endif
