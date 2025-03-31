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
# Notes
#
# - To support both a Makefile and CMake workspace within CLion, ensure that
#   .idea/misc.xml contains both of the following elements within the <project>
#   element:
#
#     <project>
#       <component name="MakefileWorkspace" PROJECT_DIR="$PROJECT_DIR$" />
#       <component name="CMakeWorkspace" PROJECT_DIR="$PROJECT_DIR$" />
#     </project>
#
# - Using 4.0.0-rc4 (because of improvements on macOS integrating with XCode)
#   without any issues, but 3.30.x is specified here so CLion doesn't complain.
#
###############################################################################

cmake_minimum_required(VERSION 3.30.0)

project(
	partikle
	VERSION 0.0.1
	LANGUAGES C
	HOMEPAGE_URL https://github.com/subfuzion/ptkl
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing(test)
add_test(NAME test COMMAND $<TARGET_FILE:ptkltest>)

include_directories(include)
add_subdirectory(src)

###############################################################################
# Defaults

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)


###############################################################################
# Set compiled log level at the top level for convenience

target_compile_definitions(
	libstd
	PUBLIC
	LOG_LEVEL=LOG_LEVEL_ERROR
)
