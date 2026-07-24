#include <exd/core/assert.hpp>
#include <cstdio>
#include <cstdlib>

namespace exd::core {

void assert_fail(const char* file, int line, const char* func,
                 const char* condition, const char* msg)
{
    if (msg)
        std::fprintf(stderr, "\nASSERTION FAILED: %s\n  %s:%d in %s\n  msg: %s\n",
                     condition, file, line, func, msg);
    else
        std::fprintf(stderr, "\nASSERTION FAILED: %s\n  %s:%d in %s\n",
                     condition, file, line, func);
    std::fflush(stderr);
    std::abort();
}

} // namespace exd::core
