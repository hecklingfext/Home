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

#include "config.hpp"
#include <cstring>
static void 
usage_and_bye (void)
{
    msgout(INFO, 
           "usage: 3ddeskd [ OPTIONS ]\n\n"
           "  where:\n"
           "    --version        Display version information\n"
           "    --acquire[=#]    Grab images for all the desktops by cycling thru\n"
           "                       (sleep for # millisecs at each screen for refresh)\n"
           "    --mirror         Grab image from current desktop and use for all\n"
           "    --dir=/path      the install dir if different than /usr/share/3ddesktop\n"
           "    --texturesize=#  size of textures (larger for more detail) in powers of 2\n" 
           "    --wm=            Specify the Windowmanager type so all the virtual \n"
           "                     desktops are found correctly.  Available options are:\n"
           "                       kde2, kde3, gnome1, gnome2, ewmh, fluxbox, windowmaker,\n"
           "                       enlightenment, sawfishonly, workspaces\n"
           "    --fastest        hog the CPU for smoothest operation\n"
           "    -v               verbose/debug information\n\n");
    exit (1);
} // END usage_and_bye


static int 
get_boolean (char *value) 
{
    if (!value) return 0;
    
    if (!strcmp(value, "0") ||
        !strcmp(value, "false") ||
        !strcmp(value, "no") ||
        !strcmp(value, "off")) 
        return 0;
    
    if (!strcmp(value, "1") ||
        !strcmp(value, "true") ||
        !strcmp(value, "yes") ||
        !strcmp(value, "on")) 
        return 1;
    
    return 0;  // default false (?)
}


Options::Options(char *n = NULL) 
{

    if (n)
        strncpy(name, n, sizeof(name));


    acquire = 0;
    glcompression = 0;
    show_fps = 0;
    show_digits = 1;
    do_fullscreen = 1;
    entry_exit_movement = 1;
    use_breathing = 1;
    random_fun_delay = 15;
    face_type = FACE_TYPE_CAROUSEL;
    default_z_offset_distance = 0;
    daemonized = 0;
    goto_column = NO_GOTO;
    goto_row = NO_GOTO;

    strcpy(base_dir, SHAREDIR);

    digit_size = 50;
    digit_color = COLOR_RED;

    frame_color = COLOR_GRAY;
    use_wireframe = false;

    linear_spacing = 2.0;

    recalc = 1;

    animation_speed = 10;

    face_change_steps = 40;
    zoom_steps = 45;

    disable_exit_after_goto = 0;

    reverse_mousewheel = 0;
    swap_mousebuttons = 0;
    alt_mousebuttons = 0;

//        __default_background[0] = 0;
    background[0] = 0;
    // ----
    //arrangement = NULL;

}  // END Options::Options

void Options::set_color (int *v, char *color) 
{
    if (strcmp(color, "red") == 0) {
        *v = COLOR_RED;
    } else if (strcmp(color, "green") == 0) {
        *v = COLOR_GREEN;
    } else if (strcmp(color, "blue") == 0) {
        *v = COLOR_BLUE;
    } else if (strcmp(color, "lightblue") == 0) {
        *v = COLOR_LIGHTBLUE;
    } else if (strcmp(color, "white") == 0) {
        *v = COLOR_WHITE;
    } else if (strcmp(color, "gray") == 0) {
        *v = COLOR_GRAY;
    } else if (strcmp(color, "purple") == 0) {
        *v = COLOR_PURPLE;
    } else if (strcmp(color, "yellow") == 0) {
        *v = COLOR_YELLOW;
    } else {
        msgout (DEBUG, "conf file: invalid color '%s' (red,green,blue,yellow,white,purple,orange)\n", color);
        //*v = COLOR_WHITE;
    }
}  // END Options::set_color

int 
Options::set_option (char *option, char *value, bool from_cmdline) 
{
    int tmpint;
    float tmpfloat;

    if (!option)
        return 1;

    if (strcmp(option, "mode") == 0) {

        if (strcmp(value, "cylinder") == 0) {
            face_type = FACE_TYPE_CYLINDER;
        } else if (strcmp(value, "linear") == 0) {
            face_type = FACE_TYPE_LINEAR;
        } else if (strcmp(value, "viewmaster") == 0) {
            face_type = FACE_TYPE_VIEWMASTER;
        } else if (strcmp(value, "carousel") == 0) {
            face_type = FACE_TYPE_CAROUSEL;
        } else if (strcmp(value, "priceisright") == 0) {
            face_type = FACE_TYPE_PRICEISRIGHT;
        } else if (strcmp(value, "flip") == 0) {
            face_type = FACE_TYPE_FLIP;
        } else if (strcmp(value, "random") == 0) {
            face_type = FACE_TYPE_RANDOM;
        } else {
            return 1;
        }

    } else if (strcmp(option, "zoomspeed") == 0) {
        float tmp = atof(value);
        if (tmp >= 1.0 && tmp <= 300.0) {
            zoom_steps = tmp;
        }

    } else if (strcmp(option, "changespeed") == 0) {
        float tmp = atof(value);
        if (tmp >= 1.0 && tmp <= 300.0) {
            face_change_steps = tmp;
        }

    } else if (strcmp(option, "animation_speed") == 0) {
        float tmp = atof(value);
        if (tmp >= 1.0 && tmp <= 100.0) {
            animation_speed = tmp;
        }

    } else if (strcmp(option, "linear_spacing") == 0) {
        float tmp = atof(value);
        if (tmp >= 0.0 && tmp <= 20) {
            linear_spacing = tmp;
            recalc = 1;
        }
    } else if (strcmp(option, "use_breathing") == 0) {
        use_breathing = get_boolean (value);

    } else if (strcmp(option, "digit_size") == 0) {
        digit_size = atoi(value);

    } else if (strcmp(option, "digit_color") == 0) {
        set_color (&digit_color, value);

    } else if (strcmp(option, "frame_color") == 0) {
        set_color (&frame_color, value);

    } else if (strcmp(option, "use_wireframe") == 0) {
        use_wireframe = get_boolean (value);

    } else if (strcmp(option, "fps") == 0) {
        show_fps = get_boolean (value);

    } else if (strcmp(option, "show_digit") == 0) {
        show_digits = get_boolean (value);;

    } else if (strcmp(option, "dir") == 0) {
        strncpy(base_dir, value, sizeof(base_dir));

    } else if (strcmp(option, "randdelay") == 0) {
        tmpint = atoi (value);
        if (0 <= tmpint && tmpint < 60)
            random_fun_delay = tmpint;

    } else if (strcmp(option, "depth") == 0) {
        tmpfloat = (float)atof(value);
        if (3.5 <= tmpfloat && tmpfloat < 40.0)
            default_z_offset_distance = tmpfloat;

    } else if (strcmp(option, "win") == 0) {
        do_fullscreen = from_cmdline ? 0 : get_boolean (value);

    } else if (strcmp(option, "acquire") == 0) {
        acquire = 1;
        if (value) {
            tmpint = atoi (value);
            // must be between 10ms and 3sec for now
            if (10 < tmpint && tmpint < 3000)
                acquire = tmpint;
        }

    } else if (strcmp(option, "nozoom") == 0) {
        entry_exit_movement = !get_boolean (value);

    } else if (strcmp(option, "zoom") == 0) {
        entry_exit_movement = get_boolean (value);

    } else if (strcmp(option, "gotocolumn") == 0) {
        if (!value)
            return 1;
        goto_column = atoi(value);
        if (goto_column < 1)
            return 1;
    } else if (strcmp(option, "gotorow") == 0) {
        if (!value)
            return 1;
        goto_row = atoi(value);
        if (goto_row < 1)
            return 1;

    } else if (strcmp(option, "gotoright") == 0) {
        goto_column = GOTO_FACE_RIGHT;
    } else if (strcmp(option, "gotoleft") == 0) {
        goto_column = GOTO_FACE_LEFT;
    } else if (strcmp(option, "gotoup") == 0) {
        goto_row = GOTO_FACE_UP;
    } else if (strcmp(option, "gotodown") == 0) {
        goto_row = GOTO_FACE_DOWN;

    } else if (strcmp(option, "dontexit") == 0) {
        disable_exit_after_goto = get_boolean (value);

    } else if (strcmp(option, "glcompression") == 0) {
        glcompression = get_boolean (value);

    } else if (strcmp(option, "reverse_mousewheel") == 0) {
        reverse_mousewheel = get_boolean (value);

    } else if (strcmp(option, "swap_mousebuttons") == 0) {
        swap_mousebuttons = get_boolean (value);

    } else if (strcmp(option, "alt_mousebuttons") == 0) {
        alt_mousebuttons = get_boolean (value);

//      } else if (strcmp(option, "background") == 0) {
//          strncpy(background, value, sizeof(background));
//            use_background_image = 1;

    } else {  // not found
        return 1;
    }

    return 0;  // success
}  // END Options::set_option



Config::Config () 
{

    // defaults
    verbose = 0;

    use_kde = 0;
    use_ewmh = 0;
    sawfish_only = 0;
    use_workspaces = 0;
    use_viewareas = 0;
    early_desktop_switch = 1;
    disable_keys_in_goto = 1;
    autoacquire = 0;
    priority = 12;
    use_context_switch = 1;
    texture_size = 1024;
    bg_texture_size = 256;
    use_background_image = 0;
    use_context_switch = 1;
    mirror = 0;

    screen_width = 0;
    screen_height = 0;

    strcpy(working_dir, "/tmp/3ddesktop");
    pidfile[0] = 0;

    default_options = new Options("cmdline");
    if (!default_options) {
        msgout(ERROR, "out of memory: Config\n");
        end_program(-1);
    }
    options = default_options;
} // END Config::Config

Config::~Config () 
{
    if (pidfile[0])
        unlink(pidfile);
    delete default_options;
    list<Options *>::iterator k;
    for (k = config_options.begin(); k != config_options.end(); k++)
        delete *k;
    config_options.clear();

    list<Config::keybinding *>::iterator i;
    for (i = keybindings.begin(); i != keybindings.end(); i++)
        delete *i;
    keybindings.clear();

    list<Config::mousebinding *>::iterator i2;
    for (i2 = mousebindings.begin(); i2 != mousebindings.end(); i2++)
        delete *i2;
    mousebindings.clear();

} // END Config::~Config

void Config::init_from_command_line (int argc, char **argv) 
{

    int c;
    int option_index;

    // these are cmd line options to the daemon
    static struct option long_options[] = {
        //{"fps", 0, 0, 0},
        {"dir", 1, 0, 0},
        {"workspaces", 0, 0, 0},
        {"texturesize", 1, 0, 0},
        {"fastest", 0, 0, 0},
        {"win", 0, 0, 0},
        {"sawfish", 0, 0, 0},
        {"acquire", 2, 0, 0},
        {"wm", 1, 0, 0},
        {"kde", 0, 0, 0},
        {"gnome2", 0, 0, 0},
        {"ewmh", 0, 0, 0},
        {"kde2", 0, 0, 0},
        {"kde3", 0, 0, 0},
        {"gnome", 0, 0, 0},
        {"gnome1", 0, 0, 0},
        {"version", 0, 0, 0},
        {"mirror", 0, 0, 0},
        {0, 0, 0, 0}
    };

    while (1) {

        c = getopt_long (argc, argv, "v",
                         long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0: // long option
              
            if (strcmp((char *)long_options[option_index].name, "version") == 0) {
                printf("3ddesktop %s (3ddeskd server)\nWritten by Brad Wasson.\n\nCopyright (C) 2002 - 2005 Brad Wasson.\n"
                       "This is free software; see the source for copying conditions.  There is NO\n"
                       "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n"
                       "This binary compiled %s %s.\n\n",
                       VERSION, __DATE__, __TIME__);
                exit(0);
            }

                //msgout (DEBUG, "option %s", long_options[option_index].name);
            if (set_global_option ((char *)long_options[option_index].name, 
                                   optarg, 1))
            {
                if (default_options->set_option((char *)long_options[option_index].name, 
                                                optarg, 1))
                {
                    usage_and_bye();
                }
            }

            break;
        case 'v':
            verbose = 1;
            msgout (INFO, "Verbose is ON\n");
            break;
        default:
            usage_and_bye();
            break;
        }
    }  // end while args
} // END Config::init_from_command_line


int Config::set_global_option(char *option, char *value, bool from_cmdline) 
{

    if (strcmp(option, "verbose") == 0) {
        verbose = get_boolean (value);

    } else if (strcmp(option, "texturesize") == 0) {
        texture_size = atoi (value);

        switch (texture_size) {
        case  16: case  32: case   64: case  128:
        case 256: case 512: case 1024: case 2048:
            break;
        default:
            // I now use gluBuild2DMipmaps so this isn't
            // really necessary because it converts it however
            // it is sooooo slow.  To use glTexImage2D it is
            // necessary.
            msgout (ERROR, "Texture size must be a power of 2 (such as 256, 512, 1024 ...)\n");
            end_program(-1);  // should I ignore this error?
            break;
        }
    } else if (strcmp(option, "default_background_texturesize") == 0) {
        bg_texture_size = atoi (value);

        switch (bg_texture_size) {
        case  16: case  32: case   64: case  128:
        case 256: case 512: case 1024: case 2048:
            break;
        default:
            // I now use gluBuild2DMipmaps so this isn't
            // really necessary because it converts it however
            // it is sooooo slow.  To use glTexImage2D it is
            // necessary.
            msgout (ERROR, "BG Texture size must be a power of 2 (such as 256, 512, 1024 ...)\n");
            use_background_image = 0;
            break;
        }

    } else if (strcmp(option, "wm") == 0) {
        if (!value)
            return 1;

        if (strcmp (value, "sawfishonly") == 0) {
            sawfish_only = 1;
        } else if (strcmp (value, "kde") == 0 ||
                   strcmp (value, "kde2") == 0) {
            use_kde = 1;
            use_ewmh = 0;
        } else if (strcmp (value, "kde3") == 0) {
            use_ewmh = 1;
        } else if (strcmp (value, "gnome") == 0 ||
                   strcmp (value, "gnome1") == 0 ||
                   strcmp (value, "enlightenment") == 0) {
            use_viewareas = 1;
            use_workspaces = 1;
            use_ewmh = 0;
        } else if (strcmp (value, "gnome2") == 0 ||
                   strcmp (value, "ewmh") == 0) {
            use_ewmh = 1;
        } else if (strcmp (value, "gnomeworkspaces") == 0 ||
                   strcmp (value, "windowmaker") == 0 ||
                   strcmp (value, "workspaces") == 0) {
            use_workspaces = 1;
            use_ewmh = 0;
        } else if (strcmp (value, "fluxbox") == 0) {
            use_ewmh = 1;
            early_desktop_switch = 0;
        } else {
            return 1;
        }

    } else if (strcmp(option, "sawfish") == 0) {
        sawfish_only = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "kde") == 0) {
        use_kde = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "kde2") == 0) {
        use_kde = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "kde3") == 0) {
        use_ewmh = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "gnome") == 0) {
        // "gnome off" will actaully enable in .conf ... but oh well
        use_ewmh = 0;
        use_workspaces = 1;

    } else if (strcmp(option, "gnome1") == 0) {
        use_ewmh = 0;
        use_workspaces = 1;

    } else if (strcmp(option, "gnome2") == 0) {
        use_ewmh = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "ewmh") == 0) {
        use_ewmh = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "workspaces") == 0) {
        use_workspaces = from_cmdline ? 1 : get_boolean (value);

    } else if (strcmp(option, "fastest") == 0) {
        use_context_switch = !get_boolean (value);

    } else if (strcmp(option, "early_desktop_switch") == 0) {
        early_desktop_switch = get_boolean (value);

    } else if (strcmp(option, "disable_keys_in_goto") == 0) {
        disable_keys_in_goto = get_boolean (value);

    } else if (strcmp(option, "autoacquire") == 0) {
        if (!strcmp(value, "off") ||
            !strcmp(value, "no") ||
            !strcmp(value, "false")) 
        {
            autoacquire = 0;
        } else {
            autoacquire = atoi (value);
        }
        
    } else if (strcmp(option, "mirror") == 0) {
        mirror = 1;
        
    } else if (strcmp(option, "priority") == 0) {
        priority = atoi (value);

    } else if (strcmp(option, "default_background") == 0) {
        strncpy(default_background, value, sizeof(default_background));
        use_background_image = 1;
        
    } else if (strcmp(option, "screen_width") == 0) {
        screen_width = atoi (value);
        if (screen_width < 10 || screen_width > 10000) 
            screen_width = 0;

    } else if (strcmp(option, "screen_height") == 0) {
        screen_height = atoi (value);
        if (screen_height < 10 || screen_height > 10000) 
            screen_height = 0;
    } else if (strcmp(option, "keybinding_left") == 0) {
        long tmp = strtol(value, NULL, 0);
        keybindings.push_back(new keybinding(tmp, LEFT));

    } else if (strcmp(option, "keybinding_right") == 0) {
        long tmp = strtol(value, NULL, 0);
        keybindings.push_back(new keybinding(tmp, RIGHT));

    } else if (strcmp(option, "keybinding_up") == 0) {
        long tmp = strtol(value, NULL, 0);
        keybindings.push_back(new keybinding(tmp, UP));

    } else if (strcmp(option, "keybinding_down") == 0) {
        long tmp = strtol(value, NULL, 0);
        keybindings.push_back(new keybinding(tmp, DOWN));

    } else if (strcmp(option, "keybinding_select") == 0) {
        long tmp = strtol(value, NULL, 0);
        keybindings.push_back(new keybinding(tmp, SELECT));

    } else if (strcmp(option, "mousebinding_left") == 0) {
        long tmp = strtol(value, NULL, 0);
        mousebindings.push_back(new mousebinding(tmp, LEFT));

    } else if (strcmp(option, "mousebinding_right") == 0) {
        long tmp = strtol(value, NULL, 0);
        mousebindings.push_back(new mousebinding(tmp, RIGHT));

    } else if (strcmp(option, "mousebinding_up") == 0) {
        long tmp = strtol(value, NULL, 0);
        mousebindings.push_back(new mousebinding(tmp, UP));

    } else if (strcmp(option, "mousebinding_down") == 0) {
        long tmp = strtol(value, NULL, 0);
        mousebindings.push_back(new mousebinding(tmp, DOWN));

    } else if (strcmp(option, "mousebinding_select") == 0) {
        long tmp = strtol(value, NULL, 0);
        mousebindings.push_back(new mousebinding(tmp, SELECT));

    } else {
        return 1;
    }

    return 0; 
} // END Config::set_global_option

void 
Config::init (int argc, char **argv) 
{
    init_from_command_line(argc, argv);
    
//    sync_global_options();

    create_working_dir_if_necessary();
}

void
Config::set_config_set (char *name) 
{
    if (!strcmp(name, "default") || !strcmp(name, "cmdline")) {
        options = default_options;
        return;
    }

    int do_random = -1;
    if (!strcmp(name, "random")) {
        msgout (DEBUG, "*** picking random view\n");
        do_random = get_randomi(0, config_options.size());
    }

    list<Options *>::iterator k;
    int cnt = 0;
    for (k = config_options.begin(); k != config_options.end(); k++, cnt++) {

        if ( (do_random != -1 && cnt == do_random) 
             ||
             (do_random == -1 && !strcmp ((*k)->name, name)) )
        {
            options = *k;
            msgout (DEBUG, "*** FOUND cfg set %s\n", (*k)->name);
            break;
        }
    }
}  // END Config::set_config_set


void 
Config::reload (void)
{
    // FIXME: track .conf timestamp for reloading efficiency?
    
    list<Config::keybinding *>::iterator i;
    for (i = keybindings.begin(); i != keybindings.end(); i++)
        delete *i;
    keybindings.clear();

    list<Config::mousebinding *>::iterator i2;
    for (i2 = mousebindings.begin(); i2 != mousebindings.end(); i2++)
        delete *i2;
    mousebindings.clear();

    list<Options *>::iterator k;
    for (k = config_options.begin(); k != config_options.end(); k++)
        delete *k;
    config_options.clear();
    options = default_options;
    load_conf();
}

void Config::load_conf (void)
{
    // FIXME: track .conf timestamp for reloading efficiency?

    // Try opening  ~/.3ddesktop/3ddesktop.conf
    //       then   /usr/share/3ddesktop/3ddesktop.conf

    sprintf(config_file, "%s/3ddesktop.conf", working_dir);
        
    FILE *fp = fopen(config_file, "r");
    if (!fp) {

        sprintf(config_file, SYSCONFDIR "/3ddesktop.conf");
            
        fp = fopen(config_file, "r");
        if (!fp) {
                
            // don't error on this - may not be there
            msgout (DEBUG, "Could not open config file: 3ddesktop.conf: %s\n", 
                    strerror(errno));
            return;
        }
    }
        
    msgout (DEBUG, "load_conf: opened %s\n", config_file);

    // init some things so they aren't 'sticky'
    use_background_image = 0;
//        default_options->__use_background_image = 0;


    char var1[100];
    char var2[100];
    char var3[100];
    char var4[100];
    char var5[100];

    char buf[1000];
    int buf_size = 1000;

    char *p = NULL;
    int n, line = 0;;

    Options *o = NULL;

    while (fgets (buf, buf_size, fp) ) {
            
        line++;

        //msgout (DEBUG, "config load: line %d read -> %s\n", line, buf);

        // null terminate at comment
        p = strpbrk (buf, "#");
        if (p)
            *p = 0;
            
        // now see if blank line
        n = strspn(buf, " \t\n");
        if (n == (int)strlen (buf))
            continue;

        n = sscanf(buf, "%s %s %s %s %s", var1, var2, var3, var4, var5);
            
        if (n < 2) {
            msgout (DEBUG, "config load: see %d vars only (line %d)\n", n, line);
            continue;
        }

        // ok something meaningful(less?) in vars

        //msgout (DEBUG, "config load: line %d found: %s %s\n", line, var1, var2);

        if (strcmp(var1, "view") == 0) {
            if (strcmp(var2, "default") == 0) {
                o = default_options;

            } else {
                o = new Options(var2);
                if (!o) {
                    msgout (ERROR, "out of memory for Options\n");
                    end_program(-1);
                }
                msgout (DEBUG, "Found view: %s\n", var2);
                config_options.push_back(o);
            }

            continue;
        }

        if (!o) {
            //msgout(DEBUG, "not in a option set\n");

            // Treat these as "global" options 
            if (set_global_option(var1, var2, 0)) {
                // option not found try default
                default_options->set_option(var1, var2, 0);
            }

//                sync_global_options();
        } else {
            o->set_option(var1, var2, 0);
        }
    }

    fclose (fp);
} // END Config::load_conf

void
Config::create_working_dir_if_necessary (void) 
{
    // first formulat the full path to location
    char *home_dir = getenv("HOME");
    char *username = getenv("USER");
    char failsafe[] = "noname";

    if (username == NULL) {
        username = getenv("LOGNAME");
        if (username == NULL) {
            username = getlogin();
            if (username == NULL) 
                username = failsafe;
        }
    }

    if (home_dir == NULL || use_tmp) {
        //msgout (DEBUG, "$HOME not set: using /tmp/.3ddesktop-user\n");
        sprintf(working_dir, "/tmp/.3ddesktop-%s", username);
    } else {
        sprintf(working_dir, "%s/.3ddesktop", home_dir);
    }


    DIR *dir_p = opendir (working_dir);
    if (dir_p == NULL) {
        if (errno == ENOENT) {
            // try to make the dir now

            //char cmd[100];
            //sprintf (cmd, "mkdir %s", working_dir);
            //system (cmd);
            if (mkdir (working_dir, S_IRWXU) < 0) {
                msgout(ERROR, "Could not make directory %s: %s\n",
                       working_dir, strerror(errno));
                end_program(1);
            }
        } else {
            msgout(ERROR, "Could not open directory %s: %s\n", 
                   working_dir, strerror(errno));
            end_program (1);
        }
    } else {
        closedir(dir_p);
    }
} // END Config::create_working_dir_if_necessary

int 
Config::make_pidfile (void) 
{
    pid_t pid;
    
    sprintf (pidfile, "%s/pid", working_dir);
    
    FILE *fh = fopen (pidfile, "w");
    if (fh == NULL) {
        msgout (ERROR, "Could not open %s: %s\n", 
                pidfile, strerror(errno));
        return -1;
    }
    
    pid = getpid();
    
    fprintf (fh, "%d", pid);
    
    fclose (fh);
    
    return 0;
} // END Config::make_pidfile 
