project(project)

set(CMAKE_CXX_STANDARD 17)

add_executable(project sudoku.h
               main.cpp)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(project ${CURSES_LIBRARIES})
