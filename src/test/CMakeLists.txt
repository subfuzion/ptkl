cmake_minimum_required(VERSION 3.30.0)

project(
	test
	LANGUAGES C
)

set(PTKLTEST_SOURCES
	test.h
	test_main.c
	adt_test.c
	cli_test.c
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
target_link_libraries(
	ptkltest
	PUBLIC
	libcli
	libptkl
)
target_compile_options(
	ptkltest
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)
