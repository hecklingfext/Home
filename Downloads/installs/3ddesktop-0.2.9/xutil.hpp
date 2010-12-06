//
//  Copyright (C) 2002 Brad Wasson <bard@systemtoolbox.com>
//
//  This file is part of 3ddesktop.
//
//  3ddesktop is free software; you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2, or (at your option)
//  any later version.
//
//  3ddesktop is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with 3ddesktop; see the file COPYING.   If not, write to
//  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef _XUTIL_HPP
#define _XUTIL_HPP


#include "3ddesk.h"

#ifdef HAVE_KDE
#include <kapp.h>
#include <kwin.h>
#endif

#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include "config.hpp"

struct gnome_area_info {
    long harea; // current
    long varea; // current
    long harea_count;
    long varea_count;
};

struct gnome_workspace_info {
    long workspace; // current
    long workspace_count;
};



// for encapsulation we put the window management stuff in a class
class VDesktops {
private:
    //int ignore_all_errors_ehandler (Display *dpy, XErrorEvent *error);
    int Get_Window_Property(Display *dpy,
                            Window target_win, 
                            Atom atom, 
                            long *length, 
                            Atom *type, 
                            int *size,
                            char **data);
    int send_client_message_32 (Display *dpy,
                                Window recipient,
                                Window event_window,
                                Atom message_type,
                                long event_mask,
                                int n_longs,
                                ...);
    Display *display;
    int screen;
    int root_win;

    // FIXME: add destructor to delete
#ifdef HAVE_KDE
    KApplication *ka;
#endif

    Config *cfg;
  //int use_kde;
  //int use_workspaces;
  //int sawfish_only;
  //int pwm;
  //int blackbox;

public:
    int init(Config *_cfg);
      //(int _use_workspaces, int _use_sawfish, int _pwm, int _blackbox);

    //void get_vdesktop_info (int *current, int *count);
    void get_vdesktop_info (int *column, int *row, 
                            int *n_columns, int *n_rows);

    void set_vdesktop (int col, int row);
    int get_row_col_info (int *n_columns, int *n_rows);

    int get_current_desktop (int *column, int *row);
    int get_row_col_inf (int *n_columns, int *n_rows);

    //int get_current_desktop (void);
    //int get_number_of_desktops (void);

    void pwm_set_current_workspace (int workspace);
    int pwm_get_current_workspace (void);
    int pwm_get_workspace_count (void);

    void gnome_set_current_area (int harea, int varea);
    void gnome_get_area_info(struct gnome_area_info *info);

    void gnome_set_current_workspace (int workspace);
    void gnome_get_workspace_info(struct gnome_workspace_info *info);

    void ewmh_set_current_desktop (int workspace);
    void ewmh_get_desktop_info(struct gnome_workspace_info *info);

    int sawfish_get_current_workspace(void);
    int sawfish_get_workspace_count(void);
    void sawfish_set_current_workspace(int workspace);

};

#endif // END _XUTIL_HPP
