ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = xp
xp_SOURCES = main.c readcfg.c extendp.h
xp_LDFLAGS = $(GLIB_LIBS)
xp_CFLAGS = $(GLIB_CFLAGS)
xp_LDADD = $(LIBREADLINE)

include_HEADERS = extendp.h

dist_doc_DATA = extendp.cfg

man1_MANS = xp.1
