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
	src/ptkl.c
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
	#$<$<CONFIG:Debug>:-Wall -Werror -O1 -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)
#set(
#	CMAKE_C_FLAGS
#	"${CMAKE_C_FLAGS} -Wall -Werror -O1 -g -Werror -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer"
#)
#target_link_options(
#	ptkl
#	BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address PUBLIC -fsanitize=leak
#)
