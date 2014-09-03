#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(mausberry-switch, 0.05, tom@tomrichards.net)
AC_CONFIG_SRCDIR([src/switch.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([LIBCONFIG], [libconfig >= 1.4],,
    AC_MSG_ERROR([libconfig 1.4 or newer not found.])
)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK

AC_CONFIG_FILES([
Makefile
src/Makefile
etc/Makefile
])
AC_OUTPUT
