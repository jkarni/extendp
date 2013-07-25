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
	CFLAGS=-I./$(SDIR) -O3
endif

LIBS = -lreadline `pkg-config --cflags --libs glib-2.0`

_DEPS = extendp.h
DEPS = $(patsubst %,$(SDIR)/%,$(_DEPS))

_OBJ = main.o readcfg.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))



xp: $(OBJ)
	@$(CC)  $^ $(CFLAGS) $(LIBS) -o $@
	@echo ""
	@echo "Done."

$(ODIR)/%.o: $(SDIR)/%.c
	@echo "Compiling xp..."
	@mkdir $(ODIR) 2>/dev/null || echo ""
	@$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

# -----------------------------------------------------------------------------



# Install & clean -------------------------------------------------------------

.PHONY: clean install

install:
	@mkdir -p $(BINDIR)
	@echo "Moving binary to $(BINDIR)... "
	@install -s xp $(BINDIR)
	@echo "Done."
	@echo "Moving man to $(MANDIR)... "
	@mkdir -p $(MANDIR)
	@echo "Done."
	@install $(MAN) $(MANDIR)
	@ln -Fi $(CFGFILE) $$HOME/.extendp.cfg || echo ""
	@[[ -n "`which xp`" ]] || echo "$(BINDIR) is not in your path!"
	@echo "Installation successful."

clean:
	@rm -f $(ODIR)/*.o core
