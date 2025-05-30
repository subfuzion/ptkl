cmake_minimum_required(VERSION 3.30.0)

project(
	libqjs
	VERSION 0.0.1
	LANGUAGES C
)

add_definitions(
	-DCONFIG_VERSION="${PROJECT_VERSION}"
)

include_directories(include quickjs)

set(
	QJS_SRC
	src/compiler.c
	src/engine.c
	quickjs/cutils.c
	quickjs/dtoa.c
	quickjs/libregexp.c
	quickjs/libunicode.c
	quickjs/quickjs.c
	quickjs/quickjs-libc.c
	quickjs/repl.c
)

add_library(
	libqjs
	STATIC
	${QJS_SRC}
)
set_target_properties(
	libqjs
	PROPERTIES
	ARCHIVE_OUTPUT_NAME qjs
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
target_include_directories(
	libqjs
	PUBLIC
	include
)
target_link_libraries(
	libqjs
	PUBLIC
	libstd
)

add_executable(
	qjs
	quickjs/qjs.c
	quickjs/repl.c
	${QJS_SRC}
)
target_link_libraries(
	qjs
	PUBLIC
	libqjs
)
target_compile_options(
	qjs
	PUBLIC
	$<$<CONFIG:Debug>:-Wall -Werror -g>
	$<$<CONFIG:Release>:-Wall -Werror -O3>
)
set_target_properties(
	qjs
	PROPERTIES
	OUTPUT_NAME qjs
	OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	C_STANDARD 23
)
