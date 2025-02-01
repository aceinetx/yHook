cmake_minimum_required(VERSION 3.20)
project(yhook)
include_directories("include")

add_library("yhook" "src/yhook.c")

add_executable("yhook_example1" "src/example1.c")
target_link_libraries("yhook_example1" "yhook")

add_executable("yhook_example2" "src/example2.cpp")
target_link_libraries("yhook_example2" "yhook")
