# ── Math tests ──
add_executable(test_vec2  math/vec2_test.cpp)
add_executable(test_vec3  math/vec3_test.cpp)
add_executable(test_vec4  math/vec4_test.cpp)
add_executable(test_mat3  math/mat3_test.cpp)
add_executable(test_mat4  math/mat4_test.cpp)
add_executable(test_quat  math/quat_test.cpp)
add_executable(test_dquat math/dual_quat_test.cpp)

# ── ECS tests ──
add_executable(test_component ecs/component_test.cpp)
add_executable(test_entity    ecs/entity_test.cpp)
add_executable(test_registry  ecs/registry_test.cpp)

# ── Core tests ──
add_executable(test_config        core/config_test.cpp)
add_executable(test_units         core/units_test.cpp)
add_executable(test_serialization core/serialization_test.cpp)
add_executable(test_allocator     core/allocator_test.cpp)
add_executable(test_event         core/event_test.cpp)

set(TEST_TARGETS
    test_vec2 test_vec3 test_vec4 test_mat3 test_mat4 test_quat test_dquat
    test_component test_entity test_registry
    test_config test_units test_serialization test_allocator test_event
)

foreach(target ${TEST_TARGETS})
    target_link_libraries(${target} PRIVATE exd-core doctest::doctest_with_main)
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    add_test(NAME ${target} COMMAND ${target})
endforeach()
