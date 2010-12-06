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

#include <GL/glx.h>
#include <Imlib2.h>

class GLWindow {
private:
    Display *display;
    int screen;
    Window win;
    GLXContext ctx;
    XSetWindowAttributes attr;
    XF86VidModeModeInfo deskMode;
    XF86VidModeModeInfo fullscreenMode;

    XVisualInfo *vi;
    Cursor empty_cursor;

    Bool fullscreen;

    int x, y;
    
    unsigned int width, height;
    unsigned int screen_width, screen_height;
    unsigned int depth;    

    unsigned char *screenshot_data;
    Imlib_Context imlib_context;
    //Imlib_Image imgfinal;
    //Imlib_Image img;

public:
    GLWindow ();
    ~GLWindow ();

    int open_display();

    int create_GL_window(char *title, Bool fullscreen);
    int kill_GL_window();

    int hide_GL_window();
    int unhide_GL_window();

    void resize_scene();
    void resize_scene(int w, int h);

    int get_best_texture_size (int current_size);

    int grab_screenshot_data_portion(int x1, int y1, int x2, int y2);
    int grab_screenshot_data();
    
    int load_screenshot_data(GLuint *texture);

    unsigned int get_width();
    unsigned int get_height();
    unsigned int get_screen_width();
    unsigned int get_screen_height();
    Window get_window();
    Display *get_display();
    
    unsigned char *get_screenshot_data_ptr(void);

};
