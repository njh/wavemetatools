dnl Require autoconf version >= 2.57
AC_PREREQ(2.57)


dnl ############# Initialisation

AC_INIT([wavemetatools], [0.2.0], [njh@aelius.com])

AC_CONFIG_SRCDIR(src/wavemetainfo.c)
AC_CONFIG_AUX_DIR(build-scripts) 
AC_CANONICAL_SYSTEM

dnl Version 1.7 of automake is recommended
AM_INIT_AUTOMAKE(1.7)
AM_CONFIG_HEADER(src/config.h)


dnl ############# Compiler and tools Checks

AC_PROG_CC
AC_C_BIGENDIAN
AC_C_CONST


dnl ############## Type checks

AC_CHECK_TYPE(int32_t, long)
AC_CHECK_TYPE(u_int32_t, unsigned long)
AC_CHECK_TYPE(int16_t, short)
AC_CHECK_TYPE(u_int16_t, unsigned short)
AC_CHECK_TYPE(int8_t, char)
AC_CHECK_TYPE(u_int8_t, unsigned char)


dnl ############## Final Output

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
