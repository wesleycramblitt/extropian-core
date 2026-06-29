cmake_minimum_required(VERSION 3.21)
project(extropian-core VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "Extropian Core: math, ECS, geometry, and foundation — zero-dependency engine bedrock"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Library target ────────────────────────────────────
add_library(ext-core STATIC
    # Math
    src/math/vec3.cpp
    src/math/quat.cpp
    src/math/mat4.cpp

    # ECS
    src/ecs/registry.cpp

    # Geometry
    src/geom/primitives.cpp
    src/geom/spatial.cpp

    # Foundation
    src/core/units.cpp
    src/core/config.cpp
    src/core/logging.cpp
    src/core/serialization.cpp
    src/core/allocator.cpp
    src/core/plugin.cpp
    src/core/event.cpp
)

target_include_directories(ext-core PUBLIC include)
target_include_directories(ext-core PRIVATE src)

add_library(ext::core ALIAS ext-core)

# ── Install ───────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS ext-core EXPORT ext-core-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ext-core-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ext-core)

# ── Tests ─────────────────────────────────────────────
option(EXT_CORE_BUILD_TESTS "Build tests" OFF)
if(EXT_CORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
