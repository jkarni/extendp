ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = xp

AM_CXXFLAGS = $(GLIB_CFLAGS)

xp_SOURCES = main.c readcfg.c extendp.h
xp_LDADD = $(GLIB_LIBS) $(LIBREADLINE)
xp_CFLAGS = $(GLIB_CFLAGS) $(LIBREADLINE)

include_HEADERS = extendp.h


