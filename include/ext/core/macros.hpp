#pragma once

#include <cstdio>
#include <cstdlib>

/// OpenGL error-checking wrapper. Calls glGetError() and prints to stderr on failure.
/// Usage: GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
#ifdef DEBUG
#define GL_CALL(cmd) \
    do { \
        cmd; \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            std::fprintf(stderr, "OpenGL error 0x%04X at %s:%d calling '%s'\n", \
                         err, __FILE__, __LINE__, #cmd); \
        } \
    } while (0)
#else
#define GL_CALL(cmd) cmd
#endif
