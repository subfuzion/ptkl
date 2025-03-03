###############################################################################
# common

.SUFFIXES:
.SUFFIXES: .c .o

BINDIR = bin
OBJDIR = obj
SRCDIR = src

CC = gcc -std=c23

CFLAGS = -g
CFLAGS += -Wall -Werror
CFLAGS += -I$(PTKLINCDIR)
CFLAGS += -MMD -MF $(OBJDIR)/$(@F).d

LDFLAGS=
LDEXPORT=
LIBS=

###############################################################################
# ptkl

PTKL = ptkl
PTKLSRCDIR = $(SRCDIR)/$(PTKL)
PTKLINCDIR = $(PTKLSRCDIR)
PTKLSRC = $(wildcard $(PTKLSRCDIR)/*.c)
PTKLOBJ = $(patsubst $(PTKLSRCDIR)/%.c,$(OBJDIR)/%.o,$(PTKLSRC))

###############################################################################
# targets

PTKL = ptkl

TARGETS = $(BINDIR)/$(PTKL)

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

$(OBJDIR)/%.o: $(PTKLSRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/$(PTKL): $(PTKLOBJ) | $(BINDIR)
	$(CC) $(LDFLAGS) $(LDEXPORT) -o $@ $^ $(LIBS)


###############################################################################

-include $(wildcard $(OBJDIR)/*.d)
