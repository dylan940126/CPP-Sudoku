CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(project)

set(CMAKE_CXX_STANDARD 17)

add_executable(project sudoku.h main.cpp)

# -lncursesw
target_link_libraries(project ncursesw)