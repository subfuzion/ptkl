cmake_minimum_required(VERSION 3.30.0)

project(libconsole)

# Set source files
set(SOURCES
    src/console.c
    src/editline.c
    src/terminal.c
)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add examples subdirectory
add_subdirectory(examples)
