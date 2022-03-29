cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME "libmedia")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)
set(SRC test.cpp test.h)
add_library(${PROJECT_NAME} SHARED  ${SRC})
