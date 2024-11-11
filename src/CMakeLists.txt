cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_executable(perm_tree perm_tree.cpp)
target_include_directories(perm_tree PUBLIC ${INCLUDE_DIR})