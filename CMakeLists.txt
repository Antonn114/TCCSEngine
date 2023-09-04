cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 11)
project(TCCSEngine)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(SOURCES
    src/tccsgraphics.c
    src/tccsmath.c
    src/main.c)


add_executable(TCCSEngine  ${SOURCES})

target_include_directories(TCCSEngine
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(TCCSEngine ${CURSES_LIBRARIES})
target_link_libraries(TCCSEngine m)
