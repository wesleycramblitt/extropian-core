cmake_minimum_required(VERSION 3.21)
project(extropian-core VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "Extropian Core: math, ECS, geometry, and foundation"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(exd-core STATIC
    # ECS
    src/ecs/registry.cpp

    # Foundation
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

target_include_directories(exd-core PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_include_directories(exd-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(exd::core ALIAS exd-core)

include(GNUInstallDirs)
install(TARGETS exd-core EXPORT exd-core-targets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT exd-core-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/exd-core)

option(EXT_CORE_BUILD_TESTS "Build tests" OFF)
if(EXT_CORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
