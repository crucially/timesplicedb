
AC_PREREQ(2.59)
AC_INIT(TSDB, 0.0, [])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_SRCDIR([bin/tsdb/tsdb.c])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CC_STDC
if test "x$ac_cv_prog_cc_c99" = xno; then
  AC_MSG_ERROR([Could not find a C99 compatible compiler])
fi
AC_PROG_LIBTOOL

AC_SUBST(LIBDL)
AC_CHECK_LIB(dl, dlopen, [LIBDL="$LIBDL -ldl" ac_cv_func_dlopen_ldl=yes], ac_cv_func_dlopen_ldl=no)

dnl Check pod2man for creating man pages
AC_CHECK_PROG([HAVE_POD2MAN], [pod2man], "yes", "no")
AM_CONDITIONAL(HAVE_POD2MAN, [ test "yes" = "$HAVE_POD2MAN" ])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    bin/Makefile
    bin/tsdb/Makefile
    include/Makefile
	man/Makefile
])
AC_OUTPUT
