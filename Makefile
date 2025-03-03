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

###############################################################################
# ptkl

PTKL = ptkl
PTKL_SRCDIR = $(SRCDIR)/$(PTKL)
PTKL_SRCS = $(wildcard $(PTKL_SRCDIR)/*.c)
PTKL_OBJS = $(patsubst $(PTKL_SRCDIR)/%.c,$(OBJDIR)/%.o,$(PTKL_SRCS))

###############################################################################
# targets

PTKL = ptkl

TARGETS = $(BINDIR)/$(PTKL)

###############################################################################
# compiler

.SUFFIXES:
.SUFFIXES: .c .o

vpath %.h include
vpath %.c $(PTKL_SRCDIR)

CC = gcc

CFLAGS = -std=c23
CFLAGS = -g
CFLAGS += -Wall -Werror
CFLAGS += -Iinclude
CFLAGS += -MMD -MF $(OBJDIR)/$(@F).d

LDFLAGS=
LDEXPORT=
LIBS=

###############################################################################
# rules

.phony: all $(BINDIR) $(OBJDIR) clean

all: $(TARGETS)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(BINDIR) $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/$(PTKL): $(PTKL_OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) $(LDEXPORT) -o $@ $^ $(LIBS)

###############################################################################
-include $(wildcard $(OBJDIR)/*.d)
