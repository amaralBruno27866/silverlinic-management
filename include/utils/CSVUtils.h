#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

// Simple CSV reading utilities (no external deps)
// - Supports comma separated values
// - Handles quotes and escaped quotes within quoted fields
// - Trims whitespace around unquoted fields
namespace csv {

struct CSVTable {
    std::vector<std::string> headers; // ordered list
    std::vector<std::unordered_map<std::string, std::string>> rows; // header->value
};

class CSVReader {
public:
    // Read entire file into a CSVTable. Throws std::runtime_error on fatal errors.
    static CSVTable readFile(const std::string &path);

private:
    static std::vector<std::string> parseLine(const std::string &line);
    static std::string trim(const std::string &s);
};

// Utilities
inline std::optional<size_t> headerIndex(const std::vector<std::string>& headers, const std::string& name) {
    for (size_t i=0;i<headers.size();++i) if (headers[i] == name) return i;
    return std::nullopt;
}

inline std::string safeGet(const std::unordered_map<std::string, std::string>& row, const std::string& key) {
    auto it = row.find(key);
    if (it == row.end()) return "";
    return it->second;
}

// Normalize timestamps that might contain a 'T' (ISO 8601) into space separated format accepted by DateTime
inline std::string normalizeTimestampForDateTime(const std::string &ts) {
    std::string out = ts;
    // Replace 'T' with space if present and remove trailing 'Z'
    size_t tpos = out.find('T');
    if (tpos != std::string::npos) out[tpos] = ' ';
    if (!out.empty() && (out.back()=='Z' || out.back()=='z')) out.pop_back();
    return out;
}

} // namespace csv
