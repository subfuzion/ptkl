cmake_minimum_required(VERSION 3.30.0)

project(
	libcli
	VERSION 0.0.1
	LANGUAGES C
)

add_definitions(
	-DCONFIG_VERSION="${PROJECT_VERSION}"
	-DPTKL="ptkl"
)

include_directories(include)

set(
	CLI_SRC
	include/args.h
	args.c
)

add_library(
	libcli
	STATIC
	${CLI_SRC}
)
set_target_properties(
	libcli
	PROPERTIES
	ARCHIVE_OUTPUT_NAME cli
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
target_include_directories(
	libcli
	PUBLIC
	include
)
target_link_libraries(
	libcli
	PUBLIC
	libptkl
	libstring
	libutils
)
target_compile_options(
	libcli
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)
