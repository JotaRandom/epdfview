dnl Process this file with autoconf to produce a configure script.
AC_INIT([ePDFView], [0.1.8], [jordi@emma-soft.com])
AC_PREREQ([2.13])
AC_CONFIG_HEADER([config.h])
AC_COPYRIGHT([Copyright (C) 2006, 2007, 2009, 2010, 2011 Emma's Software.])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CANONICAL_HOST

dnl Parameters.
AC_ARG_ENABLE([debug], [  --enable-debug          turn on debugging [[default=no]]])
if test "x$enable_debug" != "xyes"; then
    DEBUGFLAGS="-DNDEBUG -DG_DISABLE_ASSERT"
else
    DEBUGFLAGS="-DDEBUG"
fi
CFLAGS="$CFLAGS -Wall $DEBUGFLAGS"
CXXFLAGS="$CXXFLAGS -Wall -Wno-long-long $DEBUGFLAGS"

dnl Check for programs.
AC_PROG_CC
AC_PROG_CXX
AC_CHECK_PROG(DOXYGEN, doxygen, yes,)
AM_CONDITIONAL(build_reference, test "x$DOXYGEN" = "xyes")

dnl Check for internationalization.
AM_GNU_GETTEXT

dnl Check for libraries
POPPLER_REQUIRED=0.5.0
GLIB_REQUIRED=2.8.0
GTK2_REQUIRED=2.6.0

PKG_CHECK_MODULES([GLIB], [gthread-2.0 >= $GLIB_REQUIRED])
AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIB_LIBS])
PKG_CHECK_MODULES([GTK2], [gtk+-2.0 >= $GTK2_REQUIRED])
AC_SUBST([GTK2_CFLAGS])
AC_SUBST([GTK2_LIBS])
PKG_CHECK_MODULES([POPPLER], [poppler-glib >= $POPPLER_REQUIRED gdk-2.0 >= $GTK2_REQUIRED pango])
AC_SUBST([POPPLER_CFLAGS])
AC_SUBST([POPPLER_LIBS])
dnl Check if we have poppler version 0.5.2 or higher.
PKG_CHECK_EXISTS([poppler-glib >= 0.5.2], [have_poppler_0_5_2=yes])
if test "x$have_poppler_0_5_2" = "xyes"; then
    AC_DEFINE([HAVE_POPPLER_0_5_2], [1], [Define to 1 if you have Poppler version 0.5.2 or higher.])
fi
PKG_CHECK_EXISTS([poppler-glib >= 0.6], [have_poppler_0_6_0=yes])
if test "x$have_poppler_0_6_0" = "xyes"; then
    AC_DEFINE([HAVE_POPPLER_0_6_0], [1], [Define to 1 if you have Poppler version 0.6.0 or higher.])
fi
PKG_CHECK_EXISTS([poppler-glib >= 0.8], [have_poppler_0_8_0=yes])
if test "x$have_poppler_0_8_0" = "xyes"; then
    AC_DEFINE([HAVE_POPPLER_0_8_0], [1], [Define to 1 if you have Poppler version 0.8.0 or higher.])
fi
PKG_CHECK_EXISTS([poppler-glib >= 0.15], [have_poppler_0_15_0=yes])
if test "x$have_poppler_0_15_0" = "xyes"; then
    AC_DEFINE([HAVE_POPPLER_0_15_0], [1], [Define to 1 if you have Poppler version 0.15.0 or higher.])
fi
PKG_CHECK_EXISTS([poppler-glib >= 0.15.1], [have_poppler_0_15_1=yes])
if test "x$have_poppler_0_15_1" = "xyes"; then
    AC_DEFINE([HAVE_POPPLER_0_15_1], [1], [Define to 1 if you have Poppler version 0.15.1 or higher.])
fi
PKG_CHECK_EXISTS([poppler-glib >= 0.17.0], [have_poppler_0_17_0=yes])
if test "x$have_poppler_0_17_0" = "xyes"; then
    AC_DEFINE([HAVE_POPPLER_0_17_0], [1], [Define to 1 if you have Poppler version 0.15.0 or higher.])
fi


AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
    os_win32=yes
    ;;
  *)
    os_win32=no
    ;;
esac
AC_MSG_RESULT([$os_win32])

AM_CONDITIONAL([OS_WIN32],[test "$os_win32" = "yes"])

if test "$os_win32" != "yes"; then
  EPDFVIEW_PATH_CUPS([CUPS], [have_cups=yes])
  AM_CONDITIONAL(cups_printing, test "x$have_cups" = "xyes")
  if test "x$have_cups" = "xyes"; then
      AC_SUBST([CUPS_CFLAGS])
      AC_SUBST([CUPS_LIBS])
      AC_DEFINE([HAVE_CUPS], [1], [Define to 1 if you have the CUPS API library.])
  fi
else
  AM_CONDITIONAL(cups_printing, false)
fi

dnl CPPUnit library
AM_PATH_CPPUNIT([1.10.2], CPPUNIT="yes")
AM_CONDITIONAL(build_tests, test "x$CPPUNIT" = "xyes" -a "x$enable_debug" = "xyes")

dnl Check for header files.
dnl Check for types.
dnl Check for compiler characteristics.
dnl Check for library functions.
dnl Check for system services.
dnl Output files
AC_CONFIG_FILES([   \
Makefile    \
data/Makefile   \
m4/Makefile \
intl/Makefile   \
po/Makefile.in  \
src/Makefile    \
src/gtk/Makefile    \
tests/Makefile  \
doc/Makefile    \
doc/Doxyfile])
AC_OUTPUT
