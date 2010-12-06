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
//  ------- 
//  This is based very directly off of STPH by Rob Hodges.  I just
//  ripped out only what I needed becase there was a lot of extra
//  stuff there.  Thanks Rob!
//

#include "3ddesk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "xutil.hpp"

#include "util.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

Bool error_handler_hit = False;


static int ignore_all_errors_ehandler (Display *dpy, XErrorEvent *error);


int VDesktops::init(Config *_cfg)
{
    cfg = _cfg;

    display = XOpenDisplay(NULL); /* Posix-compliant systems should behave sensibly with NULL here; */
    if (display == NULL) {          /* but probably we should try harder for those that don't... */
        msgout (ERROR, "Can not open display!\n");
        return -1;
    }

#ifdef HAVE_KDE
    int fake_argc = 1;
    char *fake_argv[] = { "3ddeskd", NULL };

    // FIXME: use destructor to delete!

    // need to call this for the KWin calls to work
    ka = new KApplication(display, fake_argc, fake_argv, "3ddesktop"); //main_argc, main_argv, "3ddesktop");
#endif

    screen = DefaultScreen(display);
    
    root_win = RootWindow(display,screen);

    XSetErrorHandler(ignore_all_errors_ehandler);

    return 0;
}


void VDesktops::get_vdesktop_info (int *column, int *row, 
                                   int *n_columns, int *n_rows)
{
    int c = 0, r = 0, nc = 0, nr = 0;

#ifdef HAVE_KDE
    if (cfg->use_kde) {
        c = KWin::currentDesktop() - 1;
	nc = KWin::numberOfDesktops();
        nr = 1;
    } else 
#endif // HAVE_KDE

    if (cfg->sawfish_only) {
        msgout(DEBUG, "vdesktops: sawfish only\n");
        c = sawfish_get_current_workspace();
        nc = sawfish_get_workspace_count();
        nr = 1;

    } else if (cfg->use_ewmh) {
        msgout (DEBUG, "vdesktops: EWMH\n");
        struct gnome_workspace_info wi;
        ewmh_get_desktop_info (&wi);
        c = wi.workspace;
        nc = wi.workspace_count;
        nr = 1;

    } else if (cfg->use_viewareas) {
        msgout(DEBUG, "vdesktops: areas\n");
        struct gnome_area_info ai;
        gnome_get_area_info(&ai);
        c = ai.harea;
        r = ai.varea;
        nc = ai.harea_count;
        nr = ai.varea_count;

        if (nc == 1 && nr == 1 && cfg->use_workspaces) {
            // try workspaces
            struct gnome_workspace_info wi;
            gnome_get_workspace_info (&wi);
            c = wi.workspace;
            nc = wi.workspace_count;
            nr = 1;
            if (nc <= 1) 
                msgout (ERROR, "Could not find more then one virtual desktop!\n");
        } else 
            cfg->use_workspaces = 0;
        

    } else if (cfg->use_workspaces) {
        msgout(DEBUG, "vdesktops: workspaces only\n");
        struct gnome_workspace_info wi;
        gnome_get_workspace_info (&wi);
        c = wi.workspace;
        nc = wi.workspace_count;
        nr = 1;

    } else {
        // this is the default - make a best guess - ewmh, areas, workspaces
        msgout (DEBUG, "vdesktops: making best guess...\n");
        struct gnome_workspace_info wi;
        ewmh_get_desktop_info (&wi);
        c = wi.workspace;
        nc = wi.workspace_count;
        nr = 1;

        if (nc == 1) {
            struct gnome_area_info ai;
            gnome_get_area_info(&ai);
            c = ai.harea;
            r = ai.varea;
            nc = ai.harea_count;
            nr = ai.varea_count;
        
            if (nc == 1 && nr == 1) {
                // columns and rows is from workspaces
                struct gnome_workspace_info wi;
                gnome_get_workspace_info (&wi);
                c = wi.workspace;
                nc = wi.workspace_count;
                nr = 1;
                if (nc > 1) cfg->use_workspaces = 1;
                else msgout (ERROR, "Could not find more then one virtual desktop!\n");
            } else 
                cfg->use_viewareas = 1;
                
        } else 
            cfg->use_ewmh = 1; 
            
    }

    msgout (DEBUG, "c=%d, r=%d, nc=%d, nr=%d\n", 
            c, r, nc, nr);

    if (column)    *column = c;
    if (row)       *row = r;
    if (n_columns) *n_columns = nc;
    if (n_rows)    *n_rows = nr;
}


int VDesktops::get_current_desktop (int *column, int *row)
{
    get_vdesktop_info (column, row, 0, 0);
    return 0;
}


int VDesktops::get_row_col_info (int *n_columns, int *n_rows)
{
    get_vdesktop_info (0, 0, n_columns, n_rows);
    return 0;
}


void VDesktops::set_vdesktop (int col, int row)
{
#ifdef HAVE_KDE
    if (cfg->use_kde) {
        KWin::setCurrentDesktop(col + 1);
        msgout (DEBUG, "vdesktops: switching to %d\n", col + 1);
        XSync(display, 0);
    } else 
#endif  // HAVE_KDE

    if (cfg->sawfish_only) {
        //msgout(DEBUG, "vdesktops: sawfish only\n");
        sawfish_set_current_workspace (col);

    } else if (cfg->use_workspaces) {
        //msgout(DEBUG, "vdesktops: workspaces only\n");
        gnome_set_current_workspace (col);

    } else if (cfg->use_ewmh) {
        ewmh_set_current_desktop (col);

    } else if (cfg->use_viewareas) {
        //msgout(DEBUG, "vdesktops: areas\n");
        gnome_set_current_area (col, row);

    } else {
        msgout (DEBUG, "vdesktops: set: no type predetermined.\n");
        ewmh_set_current_desktop (col);
        //gnome_set_current_area (col, row);
        
    }
}



#if 0

//===========================
// pwm
//===========================




int VDesktops::pwm_get_current_workspace (void)
{
    char *val;
    long length;
    Atom type;
    int size;
    Atom workspace_atom;
    int ws;
    
    workspace_atom = XInternAtom(display, "_PWM_WORKSPACE_NUM", False);

    if (Get_Window_Property(display, root_win, workspace_atom,
                            &length, &type, &size, &val)) 
    {
        msgout(ERROR, "get property PWM_WORKSPACE_NUM failed - setting one\n");
        ws = 1;
    } else {
        ws = ((long *) val)[0];
        XFree(val);
    }

    XSync(display, 0);
    return ws;  // pwm has varea hardcoded as one atm
}

#if 0
void VDesktops::pwm_set_current_workspace (int workspace)
{
    int rc;
    Atom area_atom;
    
    area_atom = XInternAtom(display, "_PWM_WORKSPACE_NUM", False);
    
    rc = send_client_message_32(display,
                                root_win, root_win,
                                area_atom,
                                SubstructureNotifyMask,
                                2,
                                workspace,
                                CurrentTime);
    XSync(display, 0);
    return;
}
#endif


void VDesktops::pwm_set_current_workspace (int workspace)
{
    CARD32 data[2];
    
    data[0] = workspace;
    
    Atom ws_atom = XInternAtom(display, "_PWM_WORKSPACE_NUM", False);
    
    XChangeProperty(display, root_win, ws_atom, XA_INTEGER,
                    32, PropModeReplace, (unsigned char*)data, 1);
}



int VDesktops::pwm_get_workspace_count (void)
{
    char *val;
    long length;
    Atom type;
    int size;
    Atom workspace_atom;
    int harea, varea;
    
    workspace_atom = XInternAtom(display, "_PWM_WORKSPACE_INFO", False);

    if (Get_Window_Property(display, root_win, workspace_atom,
                            &length, &type, &size, &val)) 
    {
        msgout(ERROR, "get property PWM_WORKSPACE_INFO failed - setting one\n");
        harea = 1;
        varea = 1;
    } else {
        harea = ((long *) val)[0];
        varea = ((long *) val)[1];
        XFree(val);
    }

    XSync(display, 0);
    return harea;  // pwm has varea hardcoded as one atm
}

#endif // if 0

//===========================
// gnome - areas
//===========================

void VDesktops::gnome_set_current_area (int harea, int varea)
{
    int rc;
    Atom area_atom;
    
    area_atom = XInternAtom(display, "_WIN_AREA", False);
    
    rc = send_client_message_32(display,
                                root_win, root_win,
                                area_atom,
                                SubstructureNotifyMask,
                                3,
                                harea,
                                varea,
                                CurrentTime);
    XSync(display, 0);
    return;
}


void VDesktops::gnome_get_area_info(struct gnome_area_info *info)
{
    Atom area_atom;
    Atom area_count_atom;
    char *val;
    long length;
    Atom type;
    int size;
   
    area_atom = XInternAtom(display, "_WIN_AREA", False);
    area_count_atom = XInternAtom(display, "_WIN_AREA_COUNT", False);

    if (Get_Window_Property(display, root_win, area_atom, &length, 
                            &type, &size, &val)) 
    {
        // use DEBUG instead of ERROR so that we don't get an error
        // message when it fails but workspaces are found
        msgout(DEBUG, "get property WIN_AREA failed - setting one\n");
        info->harea = 0;
        info->varea = 0;
    } else {
        info->harea = ((long *) val)[0];
        info->varea = ((long *) val)[1];
        XFree(val);
    }

    if (Get_Window_Property(display, root_win, area_count_atom,
                            &length, &type, &size, &val)) 
    {
        // use DEBUG instead of ERROR so that we don't get an error
        // message when it fails but workspaces are found
        msgout(DEBUG, "get property WIN_AREA_COUNT failed - setting one\n");
        info->harea_count = 1;
        info->varea_count = 1;
    } else {
        info->harea_count = ((long *) val)[0];
        info->varea_count = ((long *) val)[1];
        XFree(val);
    }

    return;
}

//===========================
// gnome - workspaces
//===========================

void VDesktops::gnome_set_current_workspace (int workspace)
{
    int rc;
    Atom workspace_atom;
    
    workspace_atom = XInternAtom(display, "_WIN_WORKSPACE", False);

    rc = send_client_message_32(display,
                                root_win, root_win,
                                workspace_atom,
                                SubstructureNotifyMask,
                                2,
                                workspace,
                                CurrentTime);
    XSync(display, 0);
    return;
}

void VDesktops::gnome_get_workspace_info(struct gnome_workspace_info *info)
{
    Atom workspace_atom;
    Atom workspace_count_atom;
    char *val;
    long length;
    Atom type;
    int size;
   
    workspace_atom = XInternAtom(display, "_WIN_WORKSPACE", False);
    workspace_count_atom = XInternAtom(display, "_WIN_WORKSPACE_COUNT", False);

    if (Get_Window_Property(display, root_win, workspace_atom, &length, 
                            &type, &size, &val)) 
    {
        msgout(DEBUG, "get property WIN_WORKSPACE failed - setting one\n");
        info->workspace = 0;
    } else {
        info->workspace = (long) * (long *)val;
        XFree(val);
    }

    if (Get_Window_Property(display, root_win, workspace_count_atom,
                            &length, &type, &size, &val)) 
    {
        msgout(DEBUG, "get property WIN_WORKSPACE_COUNT failed - setting one\n");
        info->workspace_count = 1;
    } else {
        info->workspace_count = (long) * (long *)val;
        XFree(val);
    }

    return;
}


//===========================
// EWMH - desktops (GNOME 2, KDE 3 etc)
//===========================

void VDesktops::ewmh_set_current_desktop (int workspace)
{
    int rc;
    Atom workspace_atom;
    
    workspace_atom = XInternAtom(display, "_NET_CURRENT_DESKTOP", False);

    rc = send_client_message_32(display,
                                root_win, root_win,
                                workspace_atom,
                               (SubstructureNotifyMask |
                                SubstructureRedirectMask),
                                2,
                                workspace,
                                CurrentTime);
    XSync(display, 0);
    return;
}

void VDesktops::ewmh_get_desktop_info(struct gnome_workspace_info *info)
{
    Atom desktop_atom;
    Atom desktop_count_atom;
    char *val;
    long length;
    Atom type;
    int size;
   
    desktop_atom = XInternAtom(display, "_NET_CURRENT_DESKTOP", False);
    desktop_count_atom = XInternAtom(display, "_NET_NUMBER_OF_DESKTOPS", False);

    if (Get_Window_Property(display, root_win, desktop_atom, &length, 
                            &type, &size, &val)) 
    {
        msgout(DEBUG, "get property _NET_CURRENT_DESKTOP failed - setting one\n");
        info->workspace = 0;
    } else {
        info->workspace = (long) * (long *)val;
        XFree(val);
    }

    if (Get_Window_Property(display, root_win, desktop_count_atom,
                            &length, &type, &size, &val)) 
    {
        msgout(DEBUG, "get property _NET_NUMBER_OF_DESKTOPS failed - setting one\n");
        info->workspace_count = 1;
    } else {
        info->workspace_count = (long) * (long *)val;
        XFree(val);
    }

    return;
}

//===========================
// sawfish - w/o gnome
//===========================

// quick and dirty sawfish only workspace management 
// from Thomas Clausen
int VDesktops::sawfish_get_current_workspace(void)
{
    char cmd[500];
    FILE *pipe;
    int current_workspace;

    sprintf (cmd, "sawfish-client -e current-workspace");
    pipe = popen(cmd,"r");
    fscanf(pipe,"%i",&current_workspace);
    fclose(pipe);

    // msgout(DEBUG,"Current workspace: %i\n",current_workspace);
  
    return current_workspace;
}

// quick and dirty sawfish only workspace management 
// from Thomas Clausen
int VDesktops::sawfish_get_workspace_count(void)
{
    char cmd[500];
    FILE *pipe;
    int workspace_count;

    sprintf (cmd, "sawfish-client -e \"(+ 1 (cdr (workspace-limits)) (- (car (workspace-limits))))\"");
    pipe = popen(cmd,"r");
    fscanf(pipe,"%i",&workspace_count);
    fclose(pipe);

    // printf("Workspace count: %i\n",workspace_count);
  
    return workspace_count;
}

// quick and dirty sawfish only workspace management 
// from Thomas Clausen
void VDesktops::sawfish_set_current_workspace(int workspace)
{
    char cmd[500];
    sprintf (cmd, "sawfish-client -c \"(select-workspace %i)\"", workspace);
    system(cmd);
}


/* un-unashamedly ripped from stph.c, thanks Rob :) */
/* unashamedly ripped and adapted from gwmh.c, thanks Tim :) */
int VDesktops::send_client_message_32 (Display *display,
                                       Window recipient,
                                       Window event_window,
                                       Atom message_type,
                                       long event_mask,
                                       int n_longs,
                                       ...)
{
    XEvent xevent = { 0, };
    int i = 0;
    va_list var_args;

    if (n_longs > 5)
        return 1;
  
    va_start (var_args, n_longs);

    xevent.type = ClientMessage;
    xevent.xclient.window = event_window;
    xevent.xclient.message_type = message_type;
    xevent.xclient.format = 32;
    while (n_longs--)
        xevent.xclient.data.l[i++] = va_arg (var_args, long);

    va_end (var_args);

    if (!(XSendEvent (display, recipient, False, event_mask, &xevent)))
        return 1;
    if (error_handler_hit) {
        error_handler_hit = 0;
        return 1;
    }
  
    return 0;
}


#ifndef min
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

int 
VDesktops::Get_Window_Property(Display *display,
                               Window target_win, 
                               Atom atom, 
                               long *length, 
                               Atom *type, 
                               int *size,
                               char **data)
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long nbytes;
    unsigned long bytes_after;
    unsigned char *prop;
    int status;

    unsigned int max_len = 100;

    /* 
             Display *display;
             Window w;
             Atom property;
             long long_offset, long_length;
             Bool delete;
             Atom req_type;
             Atom *actual_type_return;
             int *actual_format_return;
             unsigned long *nitems_return;
             unsigned long *bytes_after_return;
             unsigned char **prop_return;
    */
    status = XGetWindowProperty(display, 
                                target_win, 
                                atom, 
                                0, 
                                (max_len+3)/4,
                                False, 
                                AnyPropertyType, 
                                &actual_type,
                                &actual_format, 
                                &nitems, 
                                &bytes_after,
                                &prop);

  
    if (error_handler_hit) {
        error_handler_hit = 0;
        return 1;
    }

    if (status != Success) /* some other problem... */
        return 1;

    if (actual_format == 32)
        nbytes = sizeof(long);
    else if (actual_format == 16)
        nbytes = sizeof(short);
    else
        nbytes = 1;
    *length = min(nitems * nbytes, max_len);
    *type = actual_type;
    *size = actual_format;
    *data = (char *)prop;

    if (prop == NULL)
        return 1;

    return 0;
}


static int
ignore_all_errors_ehandler (Display *display, XErrorEvent *error)
{
    error_handler_hit = True;
    msgout (ERROR, "X error\n");
    return 0;
}

