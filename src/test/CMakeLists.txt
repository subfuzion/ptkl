cmake_minimum_required(VERSION 3.30.0)

project(
	ptkltest
	LANGUAGES C
)

set(PTKLTEST_SOURCES
	src/testmain.c
	src/tests/adt_test.c
	src/tests/cli_test.c
	src/tests/expect_test.c
	src/tests/log_test.c
	src/tests/string_test.c
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
	libstd
)
target_compile_options(
	ptkltest
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)
