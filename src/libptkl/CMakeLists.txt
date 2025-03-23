cmake_minimum_required(VERSION 3.30.0)

project(
	libptkl
	VERSION 0.0.1
	LANGUAGES C
)

include_directories(include)

set(
	ADT_SRC
	adt/buffer.c
	adt/list.c
	adt/map.c
	adt/stack.c
	adt/vector.c
)

add_library(
	libptkl
	STATIC
	${ADT_SRC}
)
set_target_properties(
	libptkl
	PROPERTIES
	ARCHIVE_OUTPUT_NAME ptkl
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
target_include_directories(
	libptkl
	PUBLIC
	include
)
