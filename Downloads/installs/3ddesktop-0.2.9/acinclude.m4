#
#  This is an m4 macro of my own invention.  Its goal is to get the
#  right KDE and Qt -L and -I paths for LDFLAGS and CXXFLAGS
#  correspondingly.  I looked into using acinclude.m4 from KDE but
#  that fails (I believe) if it doesn't find the necessary items.  I
#  don't want the configure to fail I just want HAVE_KDE to *not* be
#  defined so the source can not use the KDE stuff.  So it will
#  default to GNOME mode if no KDE.  In other words I only want to
#  support KDE if its there, otherwise go fine without it.  Conversely
#  I should only support GNOME if its there but nobody's perfect....
#
#  I added some configure options so that you can add any include or
#  library paths and/or specify specifically KDE and Qt paths.
#
#  There is no doubt this isn't perfect.
#
#  Fingers crossed,
#  Brad 8/02


AC_DEFUN(AC_SET_KDE_INC_DIRS,
[
   AC_MSG_CHECKING(for KDE includes)

   AC_ARG_WITH(extra-includes, [  --with-extra-includes=DIR  adds non standard include paths],
                          
               extra_includes="$withval",
               extra_includes="NONE"
              )
   if test -n "$extra_includes" && \
      test "$extra_includes" != "NONE"; then
       for dir in $extra_includes; do
           CXXFLAGS="$CXXFLAGS -I$dir"
       done
   fi

   AC_ARG_WITH(extra-libs, [  --with-extra-libs=DIR      adds non standard library paths],
               extra_libs="$withval",
               extra_libs="NONE"
              )
   if test -n "$extra_libs" && \
      test "$extra_libs" != "NONE"; then
       for dir in $extra_libs; do
           LDFLAGS="$LDFLAGS -L$dir"
       done
   fi


   # 
   # Check for KDE includes
   # 

   kde_incs="/usr/include/kde"

   AC_ARG_WITH(kde-includes,
       [  --with-kde-includes=DIR    where the KDE includes are. ],
       [
          kde_incs="$kde_incs $withval"
       ])

   AC_ARG_WITH(kde-libraries,
       [  --with-kde-libraries=DIR   where the KDE library is installed.],
       [  
          LDFLAGS="$LDFLAGS -L$withval"
       ])

   kde_incs_final=""

   for dir in $kde_incs; do
       if test -d $dir; then
           kde_incs_final="$kde_incs_final $dir"
       fi
   done

   if test -z "$kde_incs_final"; then
        AC_MSG_WARN([KDE include path not found.  Disabling KDE support.])
   else

        for dir in $kde_incs_final; do
            CXXFLAGS="$CXXFLAGS -I$dir"
        done

        AC_MSG_RESULT($kde_incs_final)

        # 
        # Check for Qt includes
        # 

        AC_MSG_CHECKING(for Qt includes)

        qt_incs=`ls -1d /usr/lib/qt*/include 2> /dev/null`

        if test ! -z "$QTDIR"; then
            if test -d "$QTDIR/include"; then
                qt_incs="$QTDIR/include $qt_incs"
            fi
        fi

        AC_ARG_WITH(qt-includes,
            [  --with-qt-includes=DIR     where the Qt includes are. ],
            [
               qt_incs="$withval $qt_incs "
            ])

        AC_ARG_WITH(qt-libraries,
            [  --with-qt-libraries=DIR    where the Qt library is installed.],
            [  
               LDFLAGS="$LDFLAGS -L$withval"
            ])


        if test -z "$qt_incs"; then
            AC_MSG_WARN([No Qt include dirs found.  Disabling KDE support.])
        else 

            for inc in $qt_incs; do
                # BUG: what if none are real dirs?
                if test -d $dir; then
                    CXXFLAGS="$CXXFLAGS -I$inc"
                fi
            done

            AC_DEFINE(HAVE_KDE, 1, [Define if you have KDE])
            AC_SUBST(LIB_KDECORE, "-lkdecore")

            AC_MSG_RESULT($qt_incs)

        fi
   fi
])






dnl ########################### -*- Mode: M4 -*- #######################
dnl Copyright (C) 98, 1999 Matthew D. Langston <langston@SLAC.Stanford.EDU>
dnl
dnl This file is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This file is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this file; if not, write to:
dnl
dnl   Free Software Foundation, Inc.
dnl   Suite 330
dnl   59 Temple Place
dnl   Boston, MA 02111-1307, USA.
dnl ####################################################################


dnl @synopsis MDL_HAVE_OPENGL
dnl 
dnl Search for OpenGL.  We search first for Mesa (a GPL'ed version of
dnl OpenGL) before a vendor's version of OpenGL, unless we were
dnl specifically asked not to with `--with-Mesa=no' or `--without-Mesa'.
dnl
dnl The four "standard" OpenGL libraries are searched for: "-lGL",
dnl "-lGLU", "-lGLX" (or "-lMesaGL", "-lMesaGLU" as the case may be) and
dnl "-lglut".
dnl
dnl All of the libraries that are found (since "-lglut" or "-lGLX" might
dnl be missing) are added to the shell output variable "GL_LIBS", along
dnl with any other libraries that are necessary to successfully link an
dnl OpenGL application (e.g. the X11 libraries).  Care has been taken to
dnl make sure that all of the libraries in "GL_LIBS" are listed in the
dnl proper order.
dnl
dnl Additionally, the shell output variable "GL_CFLAGS" is set to any
dnl flags (e.g. "-I" flags) that are necessary to successfully compile
dnl an OpenGL application.
dnl
dnl The following shell variable (which are not output variables) are
dnl also set to either "yes" or "no" (depending on which libraries were
dnl found) to help you determine exactly what was found.
dnl
dnl   have_GL
dnl   have_GLU
dnl   have_GLX
dnl   have_glut
dnl
dnl A complete little toy "Automake `make distcheck'" package of how to
dnl use this macro is available at:
dnl
dnl   ftp://ftp.slac.stanford.edu/users/langston/autoconf/ac_opengl-0.01.tar.gz
dnl
dnl Please note that as the ac_opengl macro and the toy example evolves,
dnl the version number increases, so you may have to adjust the above
dnl URL accordingly.
dnl
dnl @version 0.01 $Id: acinclude.m4,v 1.2 2002/08/21 01:12:25 bard123 Exp $
dnl @author Matthew D. Langston <langston@SLAC.Stanford.EDU>

AC_DEFUN(MDL_HAVE_OPENGL,
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])
  AC_REQUIRE([MDL_CHECK_LIBM])

  AC_CACHE_CHECK([for OpenGL], mdl_cv_have_OpenGL,
  [
dnl Check for Mesa first, unless we were asked not to.

    
    AC_ARG_ENABLE(Mesa, 
                  AC_HELP_STRING([--with-Mesa],
                                 [Prefer the Mesa library over a vendors native OpenGL library (default=yes)])
                  [ use_Mesa=$enableval ], [ use_Mesa=yes ])

    if test x"$use_Mesa" = xyes; then
       GL_search_list="MesaGL   GL"
      GLU_search_list="MesaGLU GLU"
      GLX_search_list="MesaGLX GLX"
    else
       GL_search_list="GL  MesaGL"
      GLU_search_list="GLU MesaGLU"
      GLX_search_list="GLX MesaGLX"
    fi      

    AC_LANG_SAVE
    AC_LANG_C

dnl If we are running under X11 then add in the appropriate libraries.
    if ! test x"$no_x" = xyes; then
dnl Add everything we need to compile and link X programs to GL_CFLAGS
dnl and GL_X_LIBS.
      GL_CFLAGS="$X_CFLAGS"
      GL_X_LIBS="$X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS $LIBM"
    fi
    GL_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$GL_CFLAGS"

    GL_save_LIBS="$LIBS"
    LIBS="$GL_X_LIBS"

    # Save the "AC_MSG_RESULT file descriptor" to FD 8.
    exec 8>&AC_FD_MSG

    # Temporarily turn off AC_MSG_RESULT so that the user gets pretty
    # messages.
    exec AC_FD_MSG>/dev/null

    AC_SEARCH_LIBS(glAccum,          $GL_search_list, have_GL=yes,   have_GL=no)
    AC_SEARCH_LIBS(gluBeginCurve,   $GLU_search_list, have_GLU=yes,  have_GLU=no)
    AC_SEARCH_LIBS(glXChooseVisual, $GLX_search_list, have_GLX=yes,  have_GLX=no)
    AC_SEARCH_LIBS(glutInit,        glut,             have_glut=yes, have_glut=no)

    # Restore pretty messages.
    exec AC_FD_MSG>&8

    if test -n "$LIBS"; then
      mdl_cv_have_OpenGL=yes
      GL_LIBS="$LIBS"
      AC_SUBST(GL_CFLAGS)
      AC_SUBST(GL_LIBS)
    else
      mdl_cv_have_OpenGL=no
      GL_CFLAGS=
    fi

dnl Reset GL_X_LIBS regardless, since it was just a temporary variable
dnl and we don't want to be global namespace polluters.
    GL_X_LIBS=

    LIBS="$GL_save_LIBS"
    CPPFLAGS="$GL_save_CPPFLAGS"

    AC_LANG_RESTORE
  ])
])


dnl @synopsis MDL_CHECK_LIBM
dnl 
dnl Search for math library (typically -lm).
dnl
dnl The variable LIBM (which is not an output variable by default) is
dnl set to a value which is suitable for use in a Makefile (for example,
dnl in make's LOADLIBES macro) provided you AC_SUBST it first.
dnl
dnl @version 0.01 $Id: acinclude.m4,v 1.2 2002/08/21 01:12:25 bard123 Exp $
dnl @author Matthew D. Langston <langston@SLAC.Stanford.EDU>

# AC_CHECK_LIBM - check for math library
AC_DEFUN(MDL_CHECK_LIBM,
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
LIBM=
case "$host" in
*-*-beos* | *-*-cygwin*)
  # These system don't have libm
  ;;
*-ncr-sysv4.3*)
  AC_CHECK_LIB(mw, _mwvalidcheckl, LIBM="-lmw")
  AC_CHECK_LIB(m, main, LIBM="$LIBM -lm")
  ;;
*)
  AC_CHECK_LIB(m, main, LIBM="-lm")
  ;;
esac
])
