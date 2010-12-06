# Main Makefile for Brad Wasson's 3ddesktop
# Copyright (C) 2002 Brad Wasson.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

AUTOMAKE_OPTIONS = 1.6 gnu dist-bzip2

AM_CXXFLAGS = -Wall -O3 -DQT_CLEAN_NAMESPACE -DSHAREDIR=\"$(pkgdatadir)\" \
                         -DSYSCONFDIR=\"$(sysconfdir)\" \
                         $(imlib2_cflags) @GL_CFLAGS@

dist_sysconf_DATA = 3ddesktop.conf
dist_pkgdata_DATA = digits.bmp
man_MANS = 3ddesk.1 3ddeskd.1 

bin_PROGRAMS = 3ddesk 3ddeskd

3ddesk_SOURCES = 3ddesk.cpp

3ddeskd_SOURCES = 3ddeskd.cpp \
       xutil.cpp \
       arrange.cpp \
       util.cpp \
       win.cpp \
       camera.cpp \
       config.cpp

3ddeskd_LDFLAGS = $(imlib2_libs) @GL_LIBS@ 

noinst_HEADERS = 3ddesk.h \
       arrange.hpp  \
       config.hpp  \
       event.hpp  \
       face.hpp  \
       faceset.hpp  \
       message.hpp  \
       move.hpp  \
       win.hpp  \
       xutil.hpp \
       util.h

EXTRA_DIST = README ChangeLog autogen.sh 3ddesktop.spec

