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

CC = gcc

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

.PHONY: all $(BINDIR) $(OBJDIR) clean cmake-setup cmake-clean cmake-clean-all

all: $(TARGETS)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(BINDIR) $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/$(PTKL): $(PTKL_OBJS) | $(BINDIR)
	@$(CC) $(LDFLAGS) $(LDEXPORT) -o $@ $^ $(LIBS)

$(BINDIR)/$(PTKLTEST): $(PTKLTEST_OBJS) | $(BINDIR)
	@$(CC) $(LDFLAGS) $(LDEXPORT) -o $@ $^ $(LIBS)

###############################################################################
# cmake support

CMAKE = cmake -DCMAKE_C_COMPILER=clang

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)


# Initialize cmake build debug and release directories
cmake-init: $(BUILDDIR)
	@$(CMAKE) -DCMAKE_BUILD_TYPE=Debug -S . -B $(BUILDDIR)/debug
	@$(CMAKE) -DCMAKE_BUILD_TYPE=Release -S . -B $(BUILDDIR)/release

# Removes CMakeCache.txt so CMakeLists.txt can be reloaded in CLion
cmake-clean:
	@rm -rf \
	$(BUILDDIR)/debug/CMakeCache.txt $(BUILDDIR)/debug/bin/* \
	$(BUILDDIR)/release/CMakeCache.txt $(BUILDDIR)/release/bin/*

# Removes all cmake artifacts (will need to rerun `make cmake-init`)
cmake-clean-all:
	@rm -rf \
	$(BUILDDIR) \
	CMakeFiles/ \
	cmake-build-debug*/ \
	cmake-build-release*/ \
	CMakeCache.txt \
	cmake_install.cmake

###############################################################################
-include $(wildcard $(OBJDIR)/*.d)
