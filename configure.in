
AC_INIT(src/wavemetainfo.c)
AM_CONFIG_HEADER(src/config.h)
AM_INIT_AUTOMAKE(wavemetatools, 0.1)


dnl ############## C Compiler Checks
AC_PROG_CC
AC_C_BIGENDIAN
AC_C_CONST


dnl ############## Standard Header Files
AC_HEADER_STDC


dnl ############## Type checks

AC_CHECK_TYPE(int32_t, long)
AC_CHECK_TYPE(u_int32_t, unsigned long)
AC_CHECK_TYPE(int16_t, short)
AC_CHECK_TYPE(u_int16_t, unsigned short)
AC_CHECK_TYPE(int8_t, char)
AC_CHECK_TYPE(u_int8_t, unsigned char)


dnl ############## Type checks

AC_OUTPUT([Makefile src/Makefile])

