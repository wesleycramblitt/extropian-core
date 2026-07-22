#pragma once

#include <cstdarg>

namespace exd::core {

/// Severity levels for log output.
/// Error goes to stderr; everything else goes to stdout.
enum class LogLevel { Debug, Info, Warning, Error };

/// Free-function logging API. Thread-safe via internal mutex.
void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warn(const char* fmt, ...);
void log_error(const char* fmt, ...);

} // namespace exd::core
