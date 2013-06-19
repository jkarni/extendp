TARGET = xp

SDIR=src
ODIR=bin
CC=gcc

PREFIX=/usr/local
MAN=xp.1
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/share/man/man1

CFGFILE=./extendp.cfg

# Executable ------------------------------------------------------------------

ifeq ($(mode),debug)
	CFLAGS=-I./$(SDIR) -O0 -Wall -g
else
	CFLAGS=-I./$(SDIR) -O2 -Wall
endif

LIBS = -lreadline `pkg-config --cflags --libs glib-2.0`

_DEPS = extendp.h
DEPS = $(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ = main.o readcfg.o flag.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))



xp: $(OBJ)
	$(CC)  $^ $(CFLAGS) $(LIBS) -o $@

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir $(ODIR) 2>/dev/null || echo -n ""
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

# -----------------------------------------------------------------------------



# Install & clean -------------------------------------------------------------

.PHONY: clean install

install:
	mkdir -p $(BINDIR)
	install -s xp $(BINDIR)
	mkdir -p $(MANDIR)
	install $(MAN) $(MANDIR)
	ln -Fi $(CFGFILE) $$HOME/.extendp.cfg

clean:
	rm -f $(ODIR)/*.o core
