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

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

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
cmake_minimum_required(VERSION 3.30.0)
project(ptkl)

include_directories(. include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(ADT_SOURCES
        src/adt/buffer.c
        src/adt/list.c
        src/adt/map.c
        src/adt/string.c
        src/adt/vector.c
    )
set(PTKL_LIB_SOURCES
        ${ADT_SOURCE_FILES}
)
set(PTKL_SOURCES
        src/ptkl/ptkl.c
        ${PTKL_LIB_SOURCES}
)
set(PTKLTEST_SOURCES
        src/ptkltest/ptkltest.c
        ${PTKL_LIB_SOURCES}
)
set(CLEAN_ALL_ARTIFACTS
        bin
        CMakeFiles
        cmake-build-debug
        cmake-build-release
        CMakeCache.txt
        cmake_install.cmake
)

add_custom_target(
        cleanall
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CLEAN_ALL_ARTIFACTS}
        COMMAND ${CMAKE_COMMAND} -E remove ${CLEAN_ALL_ARTIFACTS}
        COMMENT "clean all"
)

add_executable(ptkl ${PTKL_SOURCES})
target_compile_options(ptkl
        PUBLIC
        $<$<CONFIG:Debug>:-std=c23 -Wall -Werror -g>
        $<$<CONFIG:Release>:-std=c23 -Wall -Werror -O3>
)

add_executable(ptkltest ${PTKLTEST_SOURCES})
target_compile_options(ptkltest
        PUBLIC
        $<$<CONFIG:Debug>:-std=c23 -Wall -Werror -g>
        $<$<CONFIG:Release>:-std=c23 -Wall -Werror -O3>
)
