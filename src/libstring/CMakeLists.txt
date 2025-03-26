cmake_minimum_required(VERSION 3.30.0)

project(
	libstring
	VERSION 0.0.1
	LANGUAGES C
)

add_definitions(
	-DCONFIG_VERSION="${PROJECT_VERSION}"
)

include_directories(include quickjs)

set(
	SDS_SRC
	sds/sds.h
	sds/sds.c
	sds/sdsalloc.h
)

add_library(
	libstring
	STATIC
	${SDS_SRC}
)
set_target_properties(
	libstring
	PROPERTIES
	ARCHIVE_OUTPUT_NAME string
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
target_include_directories(
	libstring
	PUBLIC
	include
	sds
)
