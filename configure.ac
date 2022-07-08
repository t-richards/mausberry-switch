AC_PREREQ(2.69)

AC_INIT([Mausberry Switch], [0.9], [https://github.com/t-richards/mausberry-switch/issues], [mausberry-switch], [https://github.com/t-richards/mausberry-switch])
AC_CONFIG_SRCDIR(src)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Programs
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_INSTALL

# Libraries
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.42)

# Headers
AC_CHECK_HEADERS([errno.h fcntl.h poll.h stdlib.h unistd.h])

# Types
AC_TYPE_SSIZE_T

AC_CONFIG_FILES([
Makefile
src/Makefile
tests/Makefile
])
AC_OUTPUT
