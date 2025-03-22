cmake_minimum_required(VERSION 3.30.0)

project(
	ptkl
	LANGUAGES C
)

set(
	PTKL_SRC
	ptkl.c
)

add_executable(
	ptkl
	${PTKL_SRC}
)
target_include_directories(
	ptkl
	PUBLIC
	libptkl
)
target_link_libraries(
	ptkl
	PUBLIC
	libptkl
)
target_compile_options(
	ptkl
	PUBLIC
	$<$<CONFIG:Debug>:-std=c23 -Wall -Werror -g>
	$<$<CONFIG:Release>:-std=c23 -Wall -Werror -O3>
)
set_target_properties(
	ptkl
	PROPERTIES
	OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	C_STANDARD 23
)