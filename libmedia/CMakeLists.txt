cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME "libmedia")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)
file(GLOB_RECURSE SOURCES "*.cpp" "*.c" "*.h")
add_library(${PROJECT_NAME} SHARED  ${SOURCES})
