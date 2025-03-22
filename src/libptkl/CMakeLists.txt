###############################################################################
#
# libptkl - Partikle Runtime static library
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

cmake_minimum_required(VERSION 3.30.0)

project(
	libptkl_project
	VERSION 0.0.1
	LANGUAGES C
)

###############################################################################
# sources

include_directories(. include)

set(
	ADT_SRC
	adt/buffer.c
	adt/list.c
	adt/map.c
	adt/stack.c
	adt/vector.c
)

set(
	LIBPTKL_SRC
	${ADT_SOURCE_FILES}
)

###############################################################################
# libptkl

add_library(
	libptkl
	STATIC
	${ADT_SRC}
)
target_include_directories(
	libptkl
	PUBLIC
	include
)
set_target_properties(
	libptkl
	PROPERTIES
	LIBRARY_OUTPUT_NAME libptkl
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	C_STANDARD 23
)
