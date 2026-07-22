#include <exd/core/logging.hpp>

#include <cstdio>
#include <cstdarg>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>

namespace exd::core {

namespace {
    std::mutex g_log_mutex;
}

static const char* level_str(LogLevel lv) {
    switch (lv) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
    }
    return "????";
}

static void log_impl(LogLevel lv, const char* fmt, va_list args) {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    char time_buf[32];
    std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", std::localtime(&t));

    std::fprintf((lv >= LogLevel::Error) ? stderr : stdout,
                 "[%s] %s: ", time_buf, level_str(lv));
    std::vfprintf((lv >= LogLevel::Error) ? stderr : stdout, fmt, args);
    std::fprintf((lv >= LogLevel::Error) ? stderr : stdout, "\n");
}

void log_debug(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    log_impl(LogLevel::Debug, fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    log_impl(LogLevel::Info, fmt, args);
    va_end(args);
}

void log_warn(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    log_impl(LogLevel::Warning, fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    log_impl(LogLevel::Error, fmt, args);
    va_end(args);
}

} // namespace exd::core
