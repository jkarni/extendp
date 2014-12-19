AC_PREREQ([2.69])
AC_INIT([extendp], [0.1], [jkarni@gmail.com])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AC_PROG_CC


AC_CHECK_FUNCS([strdup])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([termios.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([glib.h])

AC_CHECK_HEADERS([readline/readline.h], [:],[
    missing_headers="$missing_headers $ac_header"
])

AC_CHECK_LIB([readline], [readline],
             [AC_SUBST(LIBREADLINE,[-lreadline])],
             [missing_libraries="$missing_libraries libreadline"])

PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.28])

AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_PREREQ
AC_PROG_INSTALL
AC_TYPE_SIZE_T


AC_CONFIG_SRCDIR([src/extendp.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([ Makefile src/Makefile doc/Makefile ])
AC_OUTPUT
