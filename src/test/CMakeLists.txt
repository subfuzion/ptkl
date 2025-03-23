cmake_minimum_required(VERSION 3.30.0)

project(
	test
	LANGUAGES C
)

set(PTKLTEST_SOURCES
	ptkltest.c
	test.h
)

add_executable(
	ptkltest
	${PTKLTEST_SOURCES}
)
set_target_properties(
	ptkltest
	PROPERTIES
	OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	C_STANDARD 23
)
target_include_directories(
	libptkl
	PUBLIC
	include
)
target_link_libraries(
	ptkltest
	PUBLIC
	libptkl
)
target_compile_options(
	ptkltest
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)
