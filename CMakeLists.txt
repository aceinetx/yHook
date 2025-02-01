cmake_minimum_required(VERSION 3.20)
project(yhook)
include_directories("include")

add_library("yhook" "src/yhook.c")

add_executable("yhook_test" "src/test.c")
target_link_libraries("yhook_test" "yhook")
