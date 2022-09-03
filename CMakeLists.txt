project(build-trace)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)

add_definitions(-Wno-builtin-macro-redefined)
include(utils.cmake)
enable_testing()

add_subdirectory(hook)
add_subdirectory(gcc-plugin)
# add_subdirectory(test)