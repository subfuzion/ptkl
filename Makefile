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
# common

BINDIR = bin
OBJDIR = obj
SRCDIR = src

# cmake
BUILDDIR = build

###############################################################################
# targets

PTKL = ptkl
PTKLTEST = $(PTKL)test

TARGETS = $(BINDIR)/$(PTKL) $(BINDIR)/$(PTKLTEST)

###############################################################################
# sources

PTKL_SRCDIR = $(SRCDIR)/$(PTKL)
PTKL_SRCS = $(wildcard $(PTKL_SRCDIR)/*.c)
PTKL_OBJS = $(patsubst $(PTKL_SRCDIR)/%.c,$(OBJDIR)/%.o,$(PTKL_SRCS))

PTKLTEST_SRCDIR = $(SRCDIR)/$(PTKLTEST)
PTKLTEST_SRCS = $(wildcard $(PTKLTEST_SRCDIR)/*.c)
PTKLTEST_OBJS = $(patsubst $(PTKLTEST_SRCDIR)/%.c,$(OBJDIR)/%.o,$(PTKLTEST_SRCS))

ADT_SRCDIR = $(SRCDIR)/adt
ADT_SRCS = $(wildcard $(ADT_SRCDIR)/*.c)
ADT_OBJS = $(patsubst $(ADT_SRCDIR)/%.c,$(OBJDIR)/%.o,$(ADT_SRCS))

SRC_DIRS = $(PTKL_SRCDIR) $(PTKLTEST_SRCDIR) $(ADT_SRCDIR)

###############################################################################
# compiler

.SUFFIXES:
.SUFFIXES: .c .o

vpath %.h include
vpath %.c $(SRC_DIRS)

# Ensure default CC if env var isn't set.
# HOWEVER, the best practice is to invoke make with CC explicitly. Ex:
# make -D CC=/opt/local/bin/clang
CC ?= clang

CFLAGS = -std=c23
CFLAGS += -g
CFLAGS += -Wall -Werror
CFLAGS += -Iinclude
CFLAGS += -MMD -MF $(OBJDIR)/$(@F).d

LDFLAGS=
LDEXPORT=
LIBS=

###############################################################################
# rules

.PHONY: all $(BINDIR) $(OBJDIR) clean format tidy

all: $(TARGETS)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(BINDIR) $(OBJDIR)

format:
	@clang-format -i $$(find src -type f -name *.[c,h] \
		-not -path "src/libqjs/quickjs/*" \
		-not -path "src/libstd/sds/*" \
		)

tidy:
	@clang-tidy $$(find src -type f -name *.[c,h] -not -path "src/libqjs/quickjs/*")

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/$(PTKL): $(PTKL_OBJS) | $(BINDIR)
	@$(CC) $(LDFLAGS) $(LDEXPORT) -o $@ $^ $(LIBS)

$(BINDIR)/$(PTKLTEST): $(PTKLTEST_OBJS) | $(BINDIR)
	@$(CC) $(LDFLAGS) $(LDEXPORT) -o $@ $^ $(LIBS)

###############################################################################
# cmake support
.PHONY: cmake-init
.PHONY: cmake-debug cmake-test-debug cmake-test-debug-run
.PHONY: cmake-release cmake-test-release
.PHONY: cmake-fresh cmake-clean cmake-clean-all cleanall
.PHONY: cmake-watch

CMAKE_INIT = cmake -DCMAKE_C_COMPILER=$(CC)
CLION_CMAKE_DIRS = cmake-build-debug*/ cmake-build-release*/

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

# Initialize cmake build debug and release directories
cmake-init: $(BUILDDIR)
	@$(CMAKE_INIT) -DCMAKE_BUILD_TYPE=Debug -S . -B $(BUILDDIR)/debug
	@$(CMAKE_INIT) -DCMAKE_BUILD_TYPE=Release -S . -B $(BUILDDIR)/release

cmake-debug: format
	@cmake --build $(BUILDDIR)/debug
	@echo [ OK ] $(BUILDDIR)/debug/bin/

cmake-debug-run: cmake-debug
	@clear
	@$(BUILDDIR)/debug/bin/ptkl

cmake-test-debug: cmake-debug
	#$(BUILDDIR)/debug/bin/ptkltest
	@cd $(BUILDDIR)/debug && ctest --verbose

cmake-test-debug-run: cmake-debug
	@clear
	@$(BUILDDIR)/debug/bin/ptkltest

cmake-release: format
	@cmake --build build/release
	@echo [ OK ] build/release/bin/

cmake-test-release: cmake-release
	#build/release/bin/ptkltest
	@cd $(BUILDDIR)/release && ctest --verbose

# Refreshes CMakeCache.txt
cmake-fresh:
	@cmake -B $(BUILDDIR)/debug --fresh
	@cmake -B $(BUILDDIR)/release --fresh

# Removes CMakeCache.txt so CMakeLists.txt can be reloaded in CLion (if using the same build dir)
cmake-clean:
	@rm -rf \
	$(BUILDDIR)/debug/CMakeCache.txt $(BUILDDIR)/debug/bin/* \
	$(BUILDDIR)/release/CMakeCache.txt $(BUILDDIR)/release/bin/*

# Removes all cmake artifacts (will need to rerun `make cmake-init`)
cmake-clean-all:
	@rm -rf \
	$(BUILDDIR) \
	CMakeFiles/ \
	CMakeCache.txt \
	cmake_install.cmake \
	$(CLION_CMAKE_DIRS)

cleanall: clean cmake-clean-all

cmake-watch:
	@watchexec -c=clear make cmake-test-debug-run

###############################################################################
# Examples


###############################################################################
-include $(wildcard $(OBJDIR)/*.d)
