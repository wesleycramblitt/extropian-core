#pragma once

#include <cstdio>

// OpenGL debugging macro. Requires <glad/gl.h> to be included BEFORE this macro.
// Usage: GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
#ifdef NDEBUG
#define GL_CALL(cmd) cmd
#else
#define GL_CALL(cmd) \
    do { \
        cmd; \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            std::fprintf(stderr, "OpenGL error 0x%04X at %s:%d calling '%s'\n", \
                         err, __FILE__, __LINE__, #cmd); \
        } \
    } while (0)
#endif
