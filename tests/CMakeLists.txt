# ── Math tests ──
add_executable(test_vec2     math/vec2_test.cpp)
add_executable(test_vec3     math/vec3_test.cpp)
add_executable(test_vec4     math/vec4_test.cpp)
add_executable(test_mat3     math/mat3_test.cpp)
add_executable(test_mat4     math/mat4_test.cpp)
add_executable(test_quat     math/quat_test.cpp)
add_executable(test_dquat    math/dual_quat_test.cpp)
add_executable(test_raycast  math/raycast_test.cpp)
add_executable(test_color    math/color_test.cpp)
add_executable(test_bounds   math/bounds_test.cpp)

# ── ECS tests ──
add_executable(test_component      ecs/component_test.cpp)
add_executable(test_entity         ecs/entity_test.cpp)
add_executable(test_registry       ecs/registry_test.cpp)
add_executable(test_command_buffer ecs/command_buffer_test.cpp)
add_executable(test_system_graph   ecs/system_graph_test.cpp)

# ── Core tests ──
add_executable(test_config        core/config_test.cpp)
add_executable(test_units         core/units_test.cpp)
add_executable(test_serialization core/serialization_test.cpp)
add_executable(test_allocator     core/allocator_test.cpp)
add_executable(test_event         core/event_test.cpp)
add_executable(test_clock         core/clock_test.cpp)
add_executable(test_string_util   core/string_util_test.cpp)
add_executable(test_random        core/random_test.cpp)
add_executable(test_handle        core/handle_test.cpp)
add_executable(test_hash          core/hash_test.cpp)
add_executable(test_pool          core/pool_test.cpp)
add_executable(test_fs            core/fs_test.cpp)

set(TEST_TARGETS
    test_vec2 test_vec3 test_vec4 test_mat3 test_mat4 test_quat test_dquat
    test_raycast test_color test_bounds
    test_component test_entity test_registry test_command_buffer test_system_graph
    test_config test_units test_serialization test_allocator test_event
    test_clock test_string_util test_random test_handle test_hash test_pool test_fs
)

foreach(target ${TEST_TARGETS})
    target_link_libraries(${target} PRIVATE exd-core doctest::doctest_with_main)
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    add_test(NAME ${target} COMMAND ${target})
endforeach()
