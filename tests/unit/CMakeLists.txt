cmake_minimum_required(VERSION 3.11)
project(unit)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(GTest REQUIRED)

add_executable(unit_perm_tree perm_tree_unit_test.cpp)
target_link_libraries(unit_perm_tree gtest::gtest)
target_include_directories(unit_perm_tree PUBLIC ${INCLUDE_DIR})

set(RUN_TESTS ./unit_perm_tree --gtest_color=yes)
add_test(
    NAME unit_perm_tree_target
    COMMAND ${RUN_TESTS}
)