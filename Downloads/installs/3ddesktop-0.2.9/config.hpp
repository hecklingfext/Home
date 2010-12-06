//  ----------------------------------------------------------
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

#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>  // for mkdir

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dirent.h>  // for PATH_MAX?
#include <getopt.h>

#include <X11/keysym.h>

#include "3ddesk.h"
#include "util.h"

// STL
using namespace std;
#include <list>



// options by command line or config file 
class Options {
public:
    char name[50];

    // bits/bools on/off
    int show_fps : 1;
    int show_digits : 1;
    int do_fullscreen : 1;
    int entry_exit_movement : 1;
    int use_breathing : 1;
    int daemonized : 1;
    int use_wireframe : 1;
    int glcompression : 1;

    int acquire;

#if defined(__FreeBSD__)
    char base_dir[MAXNAMLEN + 1];
#else
    char base_dir[PATH_MAX];
#endif

#if defined(__FreeBSD__)
//    char __default_background[MAXNAMLEN + 1];
    char background[MAXNAMLEN + 1];
#else
//    char __default_background[PATH_MAX];
    char background[PATH_MAX];
#endif

    int random_fun_delay;    // 15

    float animation_speed;  // ms between animations

    // steps between face changes (40 * 10ms animation step = 400ms to
    // change faces)
    float face_change_steps;
    float zoom_steps;

    FaceType_t face_type;

    float default_z_offset_distance;

    int goto_column;
    int goto_row;

    float digit_size;
    int digit_color;
    

    int frame_color;

    float linear_spacing;

    int recalc;  // set if arrangement needs to be recalculated

    int disable_exit_after_goto;

    int reverse_mousewheel;
    int swap_mousebuttons;
    int alt_mousebuttons;


    Options(char *n);

    void set_color (int *v, char *color);
    int set_option (char *option, char *value, bool from_cmdline);
};



class Config {
private:

    list <Options *> config_options;

    Options *default_options;


#if defined(__FreeBSD__)
    char config_file[MAXNAMLEN + 1];
#else
    char config_file[PATH_MAX];
#endif

    static const int use_tmp = 0;

public:

#if defined(__FreeBSD__)
    char working_dir[MAXNAMLEN + 1];
    char pidfile[MAXNAMLEN + 1];
#else
    char working_dir[PATH_MAX];
    char pidfile[PATH_MAX];
#endif

#if defined(__FreeBSD__)
    char default_background[MAXNAMLEN + 1];
#else
    char default_background[PATH_MAX];
#endif

    int use_kde : 1;
    int use_ewmh : 1;
    int sawfish_only : 1;
    int use_workspaces : 1;
    int use_viewareas : 1;
    int early_desktop_switch : 1;
    int disable_keys_in_goto : 1;
    int verbose : 1;

    int use_background_image : 1;
    int use_context_switch : 1;

    int texture_size;
    int bg_texture_size;

    int autoacquire;
    int mirror;

    int priority;

    int screen_width;
    int screen_height;


    typedef enum { LEFT, RIGHT, DOWN, UP, SELECT } action_t;

    class keybinding {
    public:
        KeySym key;
        action_t action;
        keybinding(KeySym k, action_t a) {
            key = k; action = a;
        }
    };

    class mousebinding {
    public:
        unsigned int button;
        action_t action;
        mousebinding(int b, action_t a) {
            button = b; action = a;
        }
    };

    list <keybinding *> keybindings;
    list <mousebinding *> mousebindings;

    // ptr to the current option set (either cmd line or a conf "view")
    Options *options;

    Config ();
    ~Config ();

    void init_from_command_line (int argc, char **argv);
    int set_global_option(char *option, char *value, bool from_cmdline);
    void init (int argc, char **argv);
    void set_config_set (char *name);
    void reload (void);
    void load_conf (void);
    void create_working_dir_if_necessary (void);
    int make_pidfile (void);

//    void sync_global_options (void) {
//        
//    };

};



#endif // _CONFIG_HPP
