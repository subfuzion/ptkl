###############################################################################
#
# ptkl - Partikle Runtime
#
# MIT License
#
# Copyright 2025 Tony Pujals
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the “Software”), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
###############################################################################
#
# Basic cmake support was initially added because CLion couldn't find the
# include directory for header files without it. This only required the first
# few lines (up to and including the `include_directories` statement).
#
# Since then, cmake support was enhanced to provide parity with the Makefile,
# and the Makefile was enhanced to simplify using cmake. The Makefile contains
# a to initialize cmake with build directories that match the names used by
# CLion and to remove all cmake artifacts (except this file) if desired.
#
# To support both a Makefile and CMake workspace within CLion, ensure that
# .idea/misc.xml contains both of the following elements within the <project>
# element:
#
# <project>
#   <component name="MakefileWorkspace" PROJECT_DIR="$PROJECT_DIR$" />
#   <component name="CMakeWorkspace" PROJECT_DIR="$PROJECT_DIR$" />
# </project>
#
###############################################################################
# Using 4.0.0-rc4 (because of improvements on macOS integrating with XCode)
# without any issues, but 3.30.x is specified here so CLion doesn't complain.

cmake_minimum_required(VERSION 3.30.0)

project(
	ptkl
	VERSION 0.0.1
	LANGUAGES C
	HOMEPAGE_URL https://github.com/subfuzion/ptkl
)

# global default include directory path
include_directories(. include)

# global default binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

###############################################################################
# sources

set(ADT_SOURCES
	src/libptkl/adt/buffer.c
	src/libptkl/adt/list.c
	src/libptkl/adt/map.c
	src/libptkl/adt/stack.c
	src/libptkl/adt/vector.c
)

set(PTKL_LIB_SOURCES
	${ADT_SOURCE_FILES}
)

set(PTKL_SOURCES
	src/ptkl/ptkl.c
)

set(PTKLTEST_SOURCES
	src/test/libptkl/ptkltest.c
	src/test/libptkl/test.h
)

set(CLEAN_ALL_ARTIFACTS
	bin
	lib
	build
	CMakeFiles
	CMakeCache.txt
	cmake_install.cmake
	# cmake - JetBrains CLion
	cmake-build-debug*
	cmake-build-release*
)

add_custom_target(
	cleanall
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CLEAN_ALL_ARTIFACTS}
	COMMAND ${CMAKE_COMMAND} -E remove ${CLEAN_ALL_ARTIFACTS}
	COMMENT "clean all"
)

###############################################################################
# libpartikle

#add_library(
#	partikle
#	STATIC
#	${ADT_SOURCES}
#)
#target_include_directories(
#	partikle
#	PUBLIC
#	include
#)
#set_target_properties(
#	partikle
#	PROPERTIES
#	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
#	C_STANDARD 23
#)

###############################################################################
# ptkltest

add_executable(
	ptkltest
	${PTKLTEST_SOURCES}
)
target_include_directories(
	ptkltest
	PUBLIC
	include
)
target_link_libraries(
	ptkltest
	partikle
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

###############################################################################
# ptkl

add_executable(
	ptkl
	${PTKL_SOURCES}
)
target_include_directories(
	ptkl
	PUBLIC
	include
)
target_link_libraries(
	ptkl
#	partikle
	PRIVATE
	libptkl
)
target_compile_options(
	ptkl
	PUBLIC
	$<$<CONFIG:Debug>:-std=c23 -Wall -Werror -g>
	$<$<CONFIG:Release>:-std=c23 -Wall -Werror -O3>
)
set_property(
	TARGET ptkl
	PROPERTY C_STANDARD 23
)

###############################################################################
# test

enable_testing(
	ptkltest
)

add_test(
	NAME ptkltest
	COMMAND $<TARGET_FILE:ptkltest>
)

###############################################################################
# libptkl

add_subdirectory(src/libptkl)