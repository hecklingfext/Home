dnl Process this file with autoconf to produce a configure script.

dnl AC_INIT(acinclude.m4)

AC_INIT(3ddesk.cpp)

AC_CONFIG_SRCDIR([3ddesk.cpp])
AM_INIT_AUTOMAKE(3ddesktop,0.2.9)

dnl Checks for programs.
AC_LANG(C++)
AC_PROG_CXX

dnl Checks for header files.
AC_PATH_XTRA

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h)
dnl AC_CHECK_HEADERS(Imlib2.h)

dnl ## commented out 1/05 - removing kde 2.0 support
dnl AC_SET_KDE_INC_DIRS

AC_PATH_XTRA
CXXFLAGS="$CXXFLAGS $X_LIBS" dnl -I/usr/include/kde -I/usr/lib/qt-2.3.1/include
LIBS="$LIBS $X_PRE_LIBS $X_EXTRA_LIBS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"

AC_CHECK_HEADERS(GL/gl.h, ,
                                [AC_MSG_ERROR([GL/gl.h is needed to compile $PACKAGE])])
dnl AC_CHECK_HEADERS(X11/extensions/xf86vmode.h, ,
dnl                                 [AC_MSG_ERROR([X11/extensions/xf86vmode.h is needed to compile $PACKAGE])], 
dnl                 [INCLUDE='X11/Xlib.h'])

dnl Checks for libraries.

AC_CHECK_LIB(Xext, XextCreateExtension, , 
                                [AC_MSG_ERROR([libXext is needed to compile $PACKAGE])])
AC_CHECK_LIB(Xxf86vm, XF86VidModeSetViewPort, , 
                                [AC_MSG_ERROR([libXxf86vm is needed to compile $PACKAGE])])

MDL_HAVE_OPENGL

dnl AC_CHECK_LIB(GL, glBegin, ,
dnl                                 [AC_MSG_ERROR([libGL is needed to compile $PACKAGE])])
dnl AC_CHECK_LIB(GLU, gluPerspective, , 
dnl                                 [AC_MSG_ERROR([libGLU is needed to compile $PACKAGE])])

AC_CHECK_LIB(m, cos)

IMLIB2_CONFIG_IN_PATH=
AC_CHECK_PROG([IMLIB2_CONFIG_IN_PATH],[imlib2-config],[yes])
if test "$IMLIB2_CONFIG_IN_PATH" != yes; then
    echo "------- ERROR -------"
    echo "The imlib2-config development script was not found in your execute path."
    echo "You may have imlib2 installed somewhere not covered by your path."
    echo ""
    echo "If this is the case make sure you have the packages installed, AND"
    echo "that the imlib2-config script is in your execute path (see your"
    echo "shell's manual page on setting the \$PATH environment variable)."
    echo "---------------------"
    AC_MSG_ERROR([Fatal Error: no imlib2-config detected.])
    exit;
fi
imlib2_cflags=`imlib2-config --cflags`
imlib2_libs=`imlib2-config --libs`
AC_SUBST(imlib2_cflags)
AC_SUBST(imlib2_libs)

AM_CONFIG_HEADER(config.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_HEADER_TIME

dnl Checks for library functions.
AC_CHECK_FUNCS(gettimeofday mkdir strerror)

AC_CONFIG_FILES([Makefile 3ddesktop.spec maketar.sh])
AC_OUTPUT
