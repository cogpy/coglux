# CogLux Tests
cmake_minimum_required(VERSION 3.15)

# Find GTest if available
find_package(GTest QUIET)

if(GTest_FOUND)
    # GTest-based tests
    add_executable(coglux_tests
        test_coglux.cpp
    )
    
    target_link_libraries(coglux_tests
        PRIVATE
        coglux
        GTest::gtest_main
    )
    
    include(GoogleTest)
    gtest_discover_tests(coglux_tests)
else()
    # Simple test without GTest
    add_executable(coglux_basic_test
        test_basic.cpp
    )
    
    target_link_libraries(coglux_basic_test PRIVATE coglux)
    
    add_test(NAME basic_test COMMAND coglux_basic_test)
endif()

# Unit tests
add_executable(coglux_unit_tests
    test_unit.cpp
)

target_link_libraries(coglux_unit_tests PRIVATE coglux)

add_test(NAME unit_tests COMMAND coglux_unit_tests)
add_test(NAME unit_version COMMAND coglux_unit_tests version)
add_test(NAME unit_node COMMAND coglux_unit_tests node)
add_test(NAME unit_graph COMMAND coglux_unit_tests graph)

set_tests_properties(
    unit_tests unit_version unit_node unit_graph
    PROPERTIES LABELS "Unit"
)
