TARGET = xp


SDIR = ./src
ODIR = ./lib

CC=gcc
CFLAGS=-I$(SDIR) -O2

LIBS = -lm -lreadline `pkg-config --cflags --libs glib-2.0`

_DEPS = extendp.h
DEPS = $(patsubt %,$(SDIR)/%,$(_DEPS))

_OBJ = main.o readcfg.o flag.o
OBJ = $(patsubt %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xp: $(OBJ)
	gcc  $^ $(CFLAGS) $(LIBS) -o $@



.PHONY: clean

clean:
	rm -f $(ODIR)/*.o core
