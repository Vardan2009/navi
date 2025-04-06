cmake_minimum_required(VERSION 2.6)
project(navi)

find_library(NCURSESW_PATH NAMES ncursesw)

if(NOT NCURSESW_PATH)
    message(FATAL_ERROR "NCursesW not found!")
else()
    message("NCURSESW found at ${NCURSESW_PATH}")
endif()

include_directories("include")
file (GLOB SOURCES src/*.c)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${NCURSESW_PATH})
