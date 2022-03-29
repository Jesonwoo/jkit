cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME "libcommon")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)
set(SRC Singleton.h Singleton.cpp
        Factory.h Factory.cpp)
add_library(${PROJECT_NAME} SHARED  ${SRC})
