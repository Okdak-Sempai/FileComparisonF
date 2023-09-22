cmake_minimum_required(VERSION 3.26)
project(FileComparison C)

set(CMAKE_C_STANDARD 11)

# Get GTK+ package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Source files
add_executable(FileComparison FileComparison.c
        FilesFuncsLU.c
        FilesFuncsLU.h
        ToolsW.c
        ToolsW.h)

# Include GTK+
include_directories(${GTK3_INCLUDE_DIRS})

# GTK+ compilation flags
target_compile_options(FileComparison PRIVATE ${GTK3_CFLAGS_OTHER})

# GTK+ links
target_link_libraries(FileComparison ${GTK3_LIBRARIES})
