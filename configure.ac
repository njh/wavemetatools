dnl Require a modern, widely available autoconf version
AC_PREREQ([2.69])


dnl ############# Initialisation

AC_INIT([wavemetatools], [0.2.0], [njh@aelius.com])

AC_CONFIG_SRCDIR(src/wavemetainfo.c)
AC_CONFIG_AUX_DIR([build-scripts])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE([foreign])


dnl ############# Compiler and tools Checks

AC_PROG_CC
AC_C_BIGENDIAN


dnl ############## Type checks

AC_TYPE_INT32_T
AC_TYPE_UINT32_T
AC_TYPE_INT16_T
AC_TYPE_UINT16_T
AC_TYPE_INT8_T
AC_TYPE_UINT8_T


dnl ############## Final Output

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
