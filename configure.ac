AC_PREREQ([2.69])
AC_INIT([mymct], [0.1], [dbarajassolano@gmail.com])
AC_CONFIG_SRCDIR([src/file_parse.c])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_RANLIB

AX_FUNC_SNPRINTF

AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_HEADERS([sys/time.h], [], [echo "You need to have sys/time.h!" && exit -1])
AC_CHECK_FUNCS([strdup], [], [AC_LIBOBJ(src/strdup)])
AC_CHECK_FUNCS([gettimeofday strtol strtoul], [], [echo "You need gettimeofday, strtol and strtoul!", && exit -1])

AC_CONFIG_FILES([
	Makefile
	lib/Makefile
	])

AC_OUTPUT
