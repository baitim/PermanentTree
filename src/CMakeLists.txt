cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_executable(perm_tree perm_tree.cpp)
target_include_directories(perm_tree PUBLIC ${INCLUDE_DIR})

set(PYTHON_SCRIPT_RUN "${CMAKE_SOURCE_DIR}/tests/end_to_end/run_tests.py")
add_test(
    NAME run_perm_tree_target
    COMMAND bash -c "python3 ${PYTHON_SCRIPT_RUN}"
)