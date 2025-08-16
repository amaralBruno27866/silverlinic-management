#ifndef STRUCTURED_LOGGER_H
#define STRUCTURED_LOGGER_H

#include <string>
#include <optional>
#include <chrono>
#include <sstream>
#include <mutex>

namespace utils {

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR }; 

struct LogEventContext {
    std::string category;        // e.g. DB, MANAGER, FORM
    std::string action;          // e.g. create, update, delete
    std::optional<std::string> entityType; // e.g. Client, CaseProfile
    std::optional<std::string> entityId;   // textual ID
    std::optional<std::string> correlationId; // for request grouping
};

class StructuredLogger {
public:
    static StructuredLogger& instance();
    void setMinimumLevel(LogLevel lvl);
    LogLevel getMinimumLevel() const;
    void enableJson(bool enabled);
    void log(LogLevel level, const LogEventContext &ctx, const std::string &message);
private:
    StructuredLogger() = default;
    std::string formatTimestamp() const;
    std::string levelToString(LogLevel) const;
    std::string escape(const std::string&) const;
    LogLevel m_minLevel = LogLevel::INFO;
    bool m_json = false;
    std::mutex m_mutex;
};

// Convenience free function
void logStructured(LogLevel level, const LogEventContext &ctx, const std::string &message);

}

#endif
