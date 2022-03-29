cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME jkit)
project(${PROJECT_NAME})

add_subdirectory(libmedia)
add_subdirectory(libcommon)
add_executable (${PROJECT_NAME} main.cpp) # 要求编译可执行文件
    
