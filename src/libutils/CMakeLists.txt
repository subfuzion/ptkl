cmake_minimum_required(VERSION 3.30.0)

project(
	libutils
	VERSION 0.0.1
	LANGUAGES C
)

include_directories(include)

set(
	UTILS_SRC
	errors.c
	results.c
)

add_library(
	libutils
	STATIC
	${UTILS_SRC}
)

set_target_properties(
	libutils
	PROPERTIES
	ARCHIVE_OUTPUT_NAME utils
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
target_include_directories(
	libutils
	PUBLIC
	include
)
