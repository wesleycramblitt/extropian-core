cmake_minimum_required(VERSION 3.21)
project(extropian-core VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "Extropian Core: math, ECS, geometry, and foundation"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ext-core STATIC
    # ECS
    src/ecs/registry.cpp

    # Math (header-only with inline functions, but keep .cpp for ODR)
    src/math/vec3.cpp

    # Foundation stubs (placeholder implementations)
    src/core/config.cpp
    src/core/logging.cpp
    src/core/serialization.cpp
    src/core/allocator.cpp
    src/core/plugin.cpp
    src/core/event.cpp
    src/core/units.cpp

    # Geometry stubs
    src/geom/primitives.cpp
    src/geom/spatial.cpp
)

target_include_directories(ext-core PUBLIC include)
target_include_directories(ext-core PRIVATE src)

add_library(ext::core ALIAS ext-core)

include(GNUInstallDirs)
install(TARGETS ext-core EXPORT ext-core-targets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ext-core-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ext-core)

option(EXT_CORE_BUILD_TESTS "Build tests" OFF)
if(EXT_CORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
