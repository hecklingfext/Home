## this one is commonly used with AM_PATH_PYTHONDIR ...
dnl AM_CHECK_PYMOD(MODNAME [,SYMBOL [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
dnl Check if a module containing a given symbol is visible to python.
AC_DEFUN([AM_CHECK_PYMOD],
[AC_REQUIRE([AM_PATH_PYTHON])
py_mod_var=`echo $1['_']$2 | sed 'y%./+-%__p_%'`
AC_MSG_CHECKING(for ifelse([$2],[],,[$2 in ])python module $1)
AC_CACHE_VAL(py_cv_mod_$py_mod_var, [
ifelse([$2],[], [prog="
import sys
try:
        import $1
except ImportError:
        sys.exit(1)
except:
        sys.exit(0)
sys.exit(0)"], [prog="
import $1
$1.$2"])
if $PYTHON -c "$prog" 1>&AC_FD_CC 2>&AC_FD_CC
  then
    eval "py_cv_mod_$py_mod_var=yes"
  else
    eval "py_cv_mod_$py_mod_var=no"
  fi
])
py_val=`eval "echo \`echo '$py_cv_mod_'$py_mod_var\`"`
if test "x$py_val" != xno; then
  AC_MSG_RESULT(yes)
  ifelse([$3], [],, [$3
])dnl
else
  AC_MSG_RESULT(no)
  ifelse([$4], [],, [$4
])dnl
fi
])

dnl a macro to check for ability to create python extensions
dnl  AM_CHECK_PYTHON_HEADERS([ACTION-IF-POSSIBLE], [ACTION-IF-NOT-POSSIBLE])
dnl function also defines PYTHON_INCLUDES
AC_DEFUN([AM_CHECK_PYTHON_HEADERS],
[AC_REQUIRE([AM_PATH_PYTHON])
AC_MSG_CHECKING(for headers required to compile python extensions)
dnl deduce PYTHON_INCLUDES
py_prefix=`$PYTHON -c "import sys; print sys.prefix"`
py_exec_prefix=`$PYTHON -c "import sys; print sys.exec_prefix"`
PYTHON_INCLUDES="-I${py_prefix}/include/python${PYTHON_VERSION}"
if test "$py_prefix" != "$py_exec_prefix"; then
  PYTHON_INCLUDES="$PYTHON_INCLUDES -I${py_exec_prefix}/include/python${PYTHON_VERSION}"
fi
AC_SUBST(PYTHON_INCLUDES)
dnl check if the headers exist:
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"
AC_TRY_CPP([#include <Python.h>],dnl
[AC_MSG_RESULT(found)
$1],dnl
[AC_MSG_RESULT(not found)
$2])
CPPFLAGS="$save_CPPFLAGS"
])

AC_DEFUN([AM_TRY_LINK_PYTHON], [
  if test -z "$PYTHON_LDFLAGS"; then
    try_link_python_save_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS $2"
    AC_CHECK_LIB($PYTHON_FULL_NAME, $1, PYTHON_LDFLAGS="$2", )
    LDFLAGS="$try_link_python_save_LDFLAGS"
  fi
])

dnl SC - This block is very hacky due to my poor understanding of autoconf
dnl SC - Use 3 different functions to avoid cache tests
dnl SC - Add -L/usr/lib/python2.x/config to find static library
AC_DEFUN([AM_CHECK_PYTHON_LIBS], [
  AC_REQUIRE([AM_PATH_PYTHON])
  AC_PATH_PROG(PYTHON_CONFIG_PROG, python-config)
  PYTHON_FULL_NAME="python${PYTHON_VERSION}"
  if test -z "$PYTHON_CONFIG_PROG" ; then
    PYTHON_TRY_LDFLAGS="-L${py_prefix}/lib/${PYTHON_FULL_NAME}/config $PYTHON_TRY_LDFLAGS"
  else
    PYTHON_TRY_LDFLAGS="`$PYTHON_CONFIG_PROG`"
  fi
  AM_TRY_LINK_PYTHON(Py_Initialize, [$PYTHON_TRY_LDFLAGS -lutil])
  AM_TRY_LINK_PYTHON(Py_Finalize, [$PYTHON_TRY_LDFLAGS -lutil])
  AM_TRY_LINK_PYTHON(Py_IsInitialized, [$PYTHON_TRY_LDFLAGS -lm -lutil -lpthread])
  if test -z "$PYTHON_LDFLAGS"; then
    AC_MSG_ERROR([*** Unable to determine proper Python link flags. Make sure you have installed the Python development package. ***])
  else
    LDFLAGS="$LDFLAGS $PYTHON_LDFLAGS"
  fi
])

dnl SWIG macro
AC_DEFUN([AC_PROG_SWIG], [
  AC_PATH_PROG(SWIG, swig)
  if test -z "$SWIG" ; then
    AC_MSG_ERROR([*** SWIG not found.  This is required for Python wrapper generation. ***])
  fi
])

# xft.m4
# Copyright (c) 2002 Henrik Kinnunen (fluxgen at linuxmail.org)

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the 
# Software is furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software. 

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
# DEALINGS IN THE SOFTWARE.

# AM_PATH_XFT1([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
AC_DEFUN([AM_PATH_XFT1],
[
	AC_CHECK_LIB(Xft, XftFontOpen,
		XFT_LIBS="-lXft"
		[$1],
		[$2]
	)
])

# AM_PATH_XFT2([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
AC_DEFUN([AM_PATH_XFT2],
[
	if test x$pkg_exec_prefix != x ; then
	   xft_args="$xft_args --exec-prefix=$pkg_exec_prefix"
	   if test x${PKG_CONFIG+set} != xset ; then
	       PKG_CONFIG=$pkg_exec_prefix/bin/pkg-config
    fi
fi

if test x$xft_prefix != x ; then
   xft_args="$xft_args --prefix=$xft_prefix"
   if test x${PKG_CONFIG+set} != xset ; then
      PKG_CONFIG=$xft_prefix/bin/pkg-config	  
   fi
fi

AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test "x$PKG_CONFIG" = "xno" ; then
	ifelse([$2], , :, [$2])
else 
	XFT_CFLAGS=`$PKG_CONFIG $xftconf_args --cflags xft`
	XFT_LIBS=`$PKG_CONFIG $xftconf_args --libs xft`
	ifelse([$1], , :, [$1])
fi

])

# AM_PATH_XFT(default-value, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
# Test for Xft, and define XFT_CFLAGS and XFT_LIBS
AC_DEFUN([AM_PATH_XFT],
[
 AC_ARG_WITH(xft-prefix,[  --with-xft-prefix=path  Prefix where Xft is installed (optional)],
            xft_prefix="$withval", xft_prefix="")
 AC_ARG_WITH(pkg-exec-prefix,[  --with-pkg-exec-prefix=path Exec prefix where pkg-config is installed (optional)],
            pkg_exec_prefix="$withval", pkg_exec_prefix="")
 AC_ARG_ENABLE(xft, [  --enable-xft            Xft (antialias) support (default=$1)],
	if test "x$enableval" = "xyes"; then
		TRY_XFT=yes
	else
		TRY_XFT=no
	fi
	,
	TRY_XFT=$1
 )

if test "x$TRY_XFT" = "xyes"; then
	AC_MSG_RESULT(yes)
	AM_PATH_XFT2(
		[$2],
		# xft2 failed: try xft1
		AM_PATH_XFT1(
		[$2],
		[$3]
		AC_MSG_RESULT([Cant find Xft libraries! Disabling Xft]))
	)
else
	AC_MSG_RESULT(no)
	[$3]
fi

CFLAGS="$CFLAGS $XFT_CFLAGS"
CXXFLAGS="$CXXFLAGS $XFT_CFLAGS"
LIBS="$LIBS $XFT_LIBS"

])
