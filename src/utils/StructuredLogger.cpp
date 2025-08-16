#include "utils/StructuredLogger.h"
#include <iostream>
#include <iomanip>

namespace utils {

StructuredLogger& StructuredLogger::instance(){ static StructuredLogger inst; return inst; }

void StructuredLogger::setMinimumLevel(LogLevel lvl){ std::lock_guard<std::mutex> lock(m_mutex); m_minLevel = lvl; }
LogLevel StructuredLogger::getMinimumLevel() const { return m_minLevel; }
void StructuredLogger::enableJson(bool enabled){ std::lock_guard<std::mutex> lock(m_mutex); m_json = enabled; }

std::string StructuredLogger::formatTimestamp() const {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::tm tm{}; localtime_r(&t, &tm);
    std::ostringstream oss; oss<< std::put_time(&tm, "%Y-%m-%dT%H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

std::string StructuredLogger::levelToString(LogLevel l) const {
    switch(l){ case LogLevel::TRACE: return "TRACE"; case LogLevel::DEBUG: return "DEBUG"; case LogLevel::INFO: return "INFO"; case LogLevel::WARN: return "WARN"; case LogLevel::ERROR: return "ERROR"; } return "INFO"; }

std::string StructuredLogger::escape(const std::string &in) const {
    std::string out; out.reserve(in.size()+8);
    for(char c: in){
        switch(c){
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

void StructuredLogger::log(LogLevel level, const LogEventContext &ctx, const std::string &message){
    if(level < m_minLevel) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_json){
        std::ostringstream oss;
        oss << '{'
            << "\"ts\":\"" << escape(formatTimestamp()) << "\",";
        oss << "\"level\":\"" << levelToString(level) << "\",";
        oss << "\"category\":\"" << escape(ctx.category) << "\",";
        oss << "\"action\":\"" << escape(ctx.action) << "\",";
        if(ctx.entityType) oss << "\"entityType\":\"" << escape(*ctx.entityType) << "\",";
        if(ctx.entityId) oss << "\"entityId\":\"" << escape(*ctx.entityId) << "\",";
        if(ctx.correlationId) oss << "\"corrId\":\"" << escape(*ctx.correlationId) << "\",";
        oss << "\"msg\":\"" << escape(message) << "\"";
        oss << '}';
        std::cout << oss.str() << std::endl;
    } else {
        std::cout << '[' << formatTimestamp() << "] " << levelToString(level) << ' ' << ctx.category << '/' << ctx.action;
        if(ctx.entityType) std::cout << ' ' << *ctx.entityType;
        if(ctx.entityId) std::cout << '#' << *ctx.entityId;
        std::cout << " - " << message << std::endl;
    }
}

void logStructured(LogLevel level, const LogEventContext &ctx, const std::string &message){
    StructuredLogger::instance().log(level, ctx, message);
}

}
