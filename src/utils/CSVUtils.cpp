#include "utils/CSVUtils.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace csv {

std::string CSVReader::trim(const std::string &s) {
    size_t start = 0; while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) ++start;
    if (start == s.size()) return "";
    size_t end = s.size()-1; while (end > start && isspace(static_cast<unsigned char>(s[end]))) --end;
    return s.substr(start, end-start+1);
}

std::vector<std::string> CSVReader::parseLine(const std::string &line) {
    std::vector<std::string> result;
    std::string field;
    bool inQuotes = false;
    for (size_t i=0;i<line.size();++i) {
        char c = line[i];
        if (inQuotes) {
            if (c=='"') {
                if (i+1 < line.size() && line[i+1]=='"') { // escaped quote
                    field.push_back('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field.push_back(c);
            }
        } else {
            if (c=='"') {
                inQuotes = true;
            } else if (c==',') {
                result.push_back(trim(field));
                field.clear();
            } else {
                field.push_back(c);
            }
        }
    }
    result.push_back(trim(field));
    return result;
}

CSVTable CSVReader::readFile(const std::string &path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Cannot open CSV file: " + path);

    CSVTable table;
    std::string line;
    bool first = true;
    while (std::getline(f, line)) {
        // Skip BOM if present in first line
        if (first && line.size()>=3 && (unsigned char)line[0]==0xEF && (unsigned char)line[1]==0xBB && (unsigned char)line[2]==0xBF) {
            line = line.substr(3);
        }
        if (line.empty()) continue; // skip blank lines
        auto fields = parseLine(line);
        if (first) {
            table.headers = fields;
            first = false;
        } else {
            std::unordered_map<std::string,std::string> row;
            for (size_t i=0;i<table.headers.size() && i<fields.size(); ++i) {
                row[table.headers[i]] = fields[i];
            }
            table.rows.push_back(std::move(row));
        }
    }
    return table;
}

} // namespace csv
