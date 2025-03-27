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
	src/errors.c
	src/types/buffer.c
	src/types/list.c
	src/types/map.c
	src/types/stack.c
	src/types/strings.c
	src/types/vector.c
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