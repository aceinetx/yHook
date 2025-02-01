cmake_minimum_required(VERSION 3.20)
project(yhook)
include_directories("include")

set(CMAKE_CXX_STANDARD 11)

add_library("yhook" SHARED "src/yhook.c")

add_library("yhook_static" "src/yhook.c")
set_target_properties(yhook_static PROPERTIES OUTPUT_NAME "yhook")

add_executable("yhook_example1" "src/example1.c")
target_link_libraries("yhook_example1" "yhook")

add_executable("yhook_example2" "src/example2.cpp")
target_link_libraries("yhook_example2" "yhook")
