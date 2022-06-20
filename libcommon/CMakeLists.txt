cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME "libcommon")
#set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
message("source_dir: ${PROJECT_SOURCE_DIR}")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)

#set(SRC Singleton.h Singleton.cpp Factory.h Factory.cpp)

aux_source_directory(. SRC_LIST)

add_library(${PROJECT_NAME} SHARED  ${SRC_LIST})

install(FILES Factory.h, Singleton.h DESTINATION include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${PROJECT_NAME})
