cmake_minimum_required(VERSION 3.30.0)

project(
	libstd
	VERSION 0.0.1
	LANGUAGES C
)

add_definitions(
	-DCONFIG_VERSION="${PROJECT_VERSION}"
)

include_directories(include sds)

set(
	SDS_SRC
	sds/sds.h
	sds/sds.c
	sds/sdsalloc.h
	errors.c
	types/buffer.c
	types/dstring.c
	types/list.c
	types/map.c
	types/results.c
	types/stack.c
	types/vector.c
)

add_library(
	libstd
	STATIC
	${SDS_SRC}
)
set_target_properties(
	libstd
	PROPERTIES
	ARCHIVE_OUTPUT_NAME std
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
target_include_directories(
	libstd
	PUBLIC
	include
	sds
)