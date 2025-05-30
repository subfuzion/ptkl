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

include_directories(include)

set(
	PTKL_SRC
	src/ptkl.c
	src/commands/main.c
	src/commands/compile.c
	src/commands/console.c
	src/commands/data.c
	src/commands/help.c
	src/commands/logs.c
	src/commands/repl.c
	src/commands/run.c
	src/commands/serve.c
	src/commands/service.c
	src/commands/storage.c
	src/commands/version.c
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
target_include_directories(
	ptkl
	PUBLIC
	include
)
target_link_libraries(
	ptkl
	PUBLIC
	libcli
	libconsole
	libptkl
	libqjs
	libstd
)
target_compile_options(
	ptkl
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -O1 -g>
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
