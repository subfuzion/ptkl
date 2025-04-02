cmake_minimum_required(VERSION 3.10)

project(libconsole)

# Find ncurses package
find_package(Curses REQUIRED)

# Set source files
set(SOURCES
    src/console.c
)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CURSES_INCLUDE_DIR}
)

# Link against ncurses
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${CURSES_LIBRARIES}
)
