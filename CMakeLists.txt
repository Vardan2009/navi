cmake_minimum_required(VERSION 2.6)
project(navi)

include_directories("include")
file (GLOB SOURCES src/*.c)
add_executable(${PROJECT_NAME} ${SOURCES})

# This might be different on other systems
target_link_libraries(${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libncursesw.so)