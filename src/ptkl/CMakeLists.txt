cmake_minimum_required(VERSION 3.30.0)

project(
	ptkl
	VERSION 0.0.1
	LANGUAGES C
)

add_definitions(
	-DCONFIG_VERSION="${PROJECT_VERSION}"
	-DPTKL="ptkl"
)

set(
	PTKL_SRC
	ptkl.c
)

add_executable(
	ptkl
	${PTKL_SRC}
)
set_target_properties(
	ptkl
	PROPERTIES
	OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	C_STANDARD 23
)
target_link_libraries(
	ptkl
	PUBLIC
	libcli
	libptkl
	libqjs
	libstd
)
target_compile_options(
	ptkl
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)