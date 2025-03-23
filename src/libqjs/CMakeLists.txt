cmake_minimum_required(VERSION 3.30.0)

project(
	libqjs
	LANGUAGES C
)

include_directories(include)

set(
	QJS_SRC
	quickjs/cutils.c
	quickjs/dtoa.c
	quickjs/libregexp.c
	quickjs/libunicode.c
	quickjs/quickjs.c
	quickjs/quickjs-libc.c
)

add_definitions(
	-DCONFIG_VERSION="${PROJECT_VERSION}"
)

add_executable(
	qjs
	quickjs/qjs.c
	quickjs/repl.c
	${QJS_SRC}
)
set_target_properties(
	qjs
	PROPERTIES
	OUTPUT_NAME qjs
	OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	C_STANDARD 23
)

add_library(
	libqjs
	STATIC
	${QJS_SRC}
)
target_include_directories(
	libqjs
	PUBLIC
	include
)
set_target_properties(
	libqjs
	PROPERTIES
	ARCHIVE_OUTPUT_NAME qjs
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
