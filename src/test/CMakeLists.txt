cmake_minimum_required(VERSION 3.30.0)

project(
	test
	LANGUAGES C
)

##############################################################################
# sources

set(PTKLTEST_SOURCES
	ptkltest.c
	test.h
)

###############################################################################
# ptkltest

add_executable(
	ptkltest
	${PTKLTEST_SOURCES}
)
target_include_directories(
	libptkl
	PUBLIC
	include
)
target_link_libraries(
	ptkltest
	PRIVATE
	libptkl
)
target_compile_options(
	ptkltest
	PUBLIC
	$<$<CONFIG:Debug>:-std=c23 -Wall -Werror -g>
	$<$<CONFIG:Release>:-std=c23 -Wall -Werror -O3>
)
set_property(
	TARGET ptkltest
	PROPERTY C_STANDARD 23
)
