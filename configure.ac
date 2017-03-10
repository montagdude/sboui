# Set project name and version
AC_INIT([sboui], [0.1])

# Safety check in case user overwrites srcdir
AC_CONFIG_SRCDIR([src/cpp/ListBox.cpp])

# Keep auxiliary build tools in build-aux
AC_CONFIG_AUX_DIR([build-aux])

# subdir-objects necessary since src files are in subdirs
AM_INIT_AUTOMAKE([-Wall subdir-objects foreign])

# Check for C++ compiler
AC_PROG_CXX

# Do the rest of the conftests with C++ compiler
AC_LANG_PUSH([C++])

# Check for ncurses
AC_CHECK_HEADER([curses.h], , [AC_MSG_ERROR([Cannot find curses.h] )])
AC_CHECK_LIB([ncurses], [initscr], , [AC_MSG_ERROR([Cannot link ncurses.] )])

# Check for libconfig++
# Tip: to find functions with C symbols in a C++ library, use readelf:
# https://nerdland.net/2009/07/detecting-c-libraries-with-autotools/
AC_CHECK_HEADER([libconfig.h++], , [AC_MSG_ERROR([Cannot find libconfig.h++] )])
AC_CHECK_LIB([config++], [config_setting_remove], , [AC_MSG_ERROR([Cannot link libconfig++.] )])

# Create a Makefile
AC_CONFIG_FILES([Makefile])

# Generate the output
AC_OUTPUT
