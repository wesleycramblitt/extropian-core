#pragma once

#include <cstdio>
#include <cstdlib>

namespace exd::core {

/// Halt immediately (call abort). Use for unrecoverable errors.
void assert_fail(const char* file, int line, const char* func,
                 const char* condition, const char* msg);

} // namespace exd::core

// ────────────────────────────────────────────────────
//  Assertion macros
// ────────────────────────────────────────────────────

/// Basic assertion.  Halts if condition is false.
/// In release builds (NDEBUG), compiles to (void)0 — no runtime cost.
#ifndef NDEBUG
#define EXD_ASSERT(cond)                                                       \
    do {                                                                       \
        if (!(cond)) ::exd::core::assert_fail(__FILE__, __LINE__, __func__,    \
                                              #cond, nullptr);                 \
    } while (0)
#else
#define EXD_ASSERT(cond) ((void)0)
#endif

/// Assertion with a printf-style message.
#ifndef NDEBUG
#define EXD_ASSERT_MSG(cond, fmt, ...)                                         \
    do {                                                                       \
        if (!(cond)) {                                                         \
            char _exd_amsg_buf[512];                                           \
            std::snprintf(_exd_amsg_buf, sizeof(_exd_amsg_buf),                \
                          fmt, ##__VA_ARGS__);                                 \
            ::exd::core::assert_fail(__FILE__, __LINE__, __func__,             \
                                     #cond, _exd_amsg_buf);                    \
        }                                                                      \
    } while (0)
#else
#define EXD_ASSERT_MSG(cond, fmt, ...) ((void)0)
#endif

/// Always-on check.  Halts even in release builds.
#define EXD_ENSURE(cond)                                                       \
    do {                                                                       \
        if (!(cond)) ::exd::core::assert_fail(__FILE__, __LINE__, __func__,    \
                                              #cond, nullptr);                 \
    } while (0)
