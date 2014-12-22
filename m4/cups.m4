#
# EPDFVIEW_PATH_CUPS(PREFIX, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# Check to see whether the CUPS header and libraries exists.
#
AC_DEFUN([EPDFVIEW_PATH_CUPS],
[AC_ARG_WITH([cups],
    AS_HELP_STRING([--with-cups], [use the CUPS for printing (default)]),
    [ac_cv_use_cups=$withval], [ac_cv_use_cups=yes])
AC_PATH_PROG([CUPS_CONFIG], [cups-config], no, [$PATH])
AC_MSG_CHECKING([for CUPS])
if test "x$ac_cv_use_cups" = "xno"; then
    AC_MSG_RESULT([disabled])
    ac_cv_have_cups=no
elif test "x$CUPS_CONFIG" = "xno"; then
    AC_MSG_RESULT([cups-config not found.])
    ac_cv_have_cups=no
else
    AC_MSG_RESULT([yes])
    $1[]_CFLAGS=`$CUPS_CONFIG --cflags`
    $1[]_LIBS=`$CUPS_CONFIG --libs`
    ac_cv_have_cups=yes
fi

if test "x$ac_cv_have_cups" = "xyes"; then
    ifelse([$2], , :, [$2])
else
    ifelse([$3], , :, [$3])
fi
]) # EPDFVIEW_CUPS_PATH
