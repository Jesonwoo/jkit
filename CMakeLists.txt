cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME jkit)
project(${PROJECT_NAME})

# 设置安装目录
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/build/install)

# 支持c++11
set(CMAKE_CXX_STANDARD 11)
# 支持c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置QT GUI相关配置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Remove strings matching given regular expression from a list.
# @param(in,out) aItems Reference of a list variable to filter.
# @param aRegEx Value of regular expression to match.
function (filter_items aItems aRegEx)
    # For each item in our list
    foreach (item ${${aItems}})
        # Check if our items matches our regular expression
        if ("${item}" MATCHES ${aRegEx})
            # Remove current item from our list
            list (REMOVE_ITEM ${aItems} ${item})
        endif ("${item}" MATCHES ${aRegEx})
    endforeach(item)
    # Provide output parameter
    set(${aItems} ${${aItems}} PARENT_SCOPE)
endfunction (filter_items)

add_subdirectory(libmedia)
add_subdirectory(libcommon)

file(GLOB_RECURSE SOURCES "*.cpp" "*.c")
filter_items(SOURCES "libmedia")
filter_items(SOURCES "libcommon")
filter_items(SOURCES "build")

file(GLOB_RECURSE HEADERS "*.h")
filter_items(HEADERS "libmedia")
filter_items(HEADERS "libcommon")
filter_items(HEADERS "3rd")

#file(GLOB_RECURSE FORMS "*.ui")
#filter_items(FORMS "libmedia")
#filter_items(FORMS "libcommon")

file(GLOB_RECURSE RESOURCES "*.qrc")
filter_items(RESOURCES "libmedia")
filter_items(RESOURCES "libcommon")

include_directories(.)

find_package(Qt5 COMPONENTS Core Quick Concurrent  REQUIRED)
add_executable (${PROJECT_NAME} ${HEADERS} ${SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick libmedia)
