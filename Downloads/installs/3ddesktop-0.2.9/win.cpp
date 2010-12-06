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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>   // OpenGL itself.
#include <GL/glu.h>  // GLU support library.
#include <GL/glx.h>

#include <X11/extensions/xf86vmode.h>
#include <Imlib2.h>

#include "win.hpp"

#include "3ddesk.h" // for FOV
#include "config.hpp"

/* attributes for a single buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4, 
    GLX_GREEN_SIZE, 4, 
    GLX_BLUE_SIZE, 4, 
    GLX_DEPTH_SIZE, 16,
    None
};

/* attributes for a double buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER, 
    GLX_RED_SIZE, 4, 
    GLX_GREEN_SIZE, 4, 
    GLX_BLUE_SIZE, 4, 
    GLX_DEPTH_SIZE, 16,
    None 
};


unsigned int GLWindow::get_width()
{ return width; }

unsigned int GLWindow::get_height()
{ return height; }

unsigned int GLWindow::get_screen_width()
{ return screen_width; }

unsigned int GLWindow::get_screen_height()
{ return screen_height; }


Display *GLWindow::get_display()
{ return display; }

Window GLWindow::get_window()
{ return win; }


static int
X_errors_handler (Display *display, XErrorEvent *error)
{
    msgout (ERROR, "-------- X error\n");
    return 0;
}


extern Config *cfg;  // FIXME!

GLWindow::GLWindow() {
    screenshot_data = NULL;
}

GLWindow::~GLWindow() {
    if (screenshot_data)
        delete screenshot_data;
}

int GLWindow::open_display() 
{
    display = XOpenDisplay(0);
    if (display == NULL) {
        msgout (ERROR, "Can't open display\n");
        return -1;
    }
    
#if 0
    // this must be done later after create_GL_window and after
    // get_best_texture_size when we know what texturesize we will be
    // using (also init imgfinal then)
    screenshot_data = new unsigned char [cfg->texture_size * cfg->texture_size * 4];
    if (screenshot_data == NULL) {
        msgout (ERROR, "out of memory\n");
        end_program(-1);
    }
#endif

    screen = DefaultScreen(display);

    if (cfg->screen_width) {
        screen_width = cfg->screen_width;
    } else {  // zero is default
        screen_width = DisplayWidth(display, screen);
    }
    if (cfg->screen_height) { 
        screen_height = cfg->screen_height;
    } else {  // zero is default
        screen_height = DisplayHeight(display, screen);
    }

    Visual *vis = DefaultVisual(display, screen);
    Colormap cm = DefaultColormap(display, screen);

    imlib_context = imlib_context_new();
    imlib_context_set_display(display);
    imlib_context_set_visual(vis);
    imlib_context_set_colormap(cm);
    imlib_context_set_drawable(RootWindow(display, screen));
    imlib_context_set_anti_alias(1);
    imlib_context_set_blend(0);

 //   img = imlib_create_image(screen_width, screen_height);

    XSetErrorHandler(X_errors_handler);

    return 0;
}


// ripped from bbws.cc -- redundant to VDesktops::send_client_message
// but it was easier to just put another version here
static void sendClientMessage(Display *display, int screen, 
                              int win, Atom atom, XID data)
{
    XEvent e;
    unsigned long mask;
    
    e.xclient.type = ClientMessage;
    e.xclient.window = win;
    e.xclient.message_type = atom;
    e.xclient.format = 32;
    e.xclient.data.l[0] = (unsigned long) data;
    e.xclient.data.l[1] = CurrentTime;
    
    mask =  SubstructureRedirectMask;
    
    XSendEvent(display, RootWindow(display,screen),
               False, mask, &e);
}

int GLWindow::create_GL_window(char *title, Bool fs)
{
    Colormap cmap;
    int dpyWidth, dpyHeight;
    int glxMajorVersion, glxMinorVersion;
    int vidModeMajorVersion, vidModeMinorVersion;
    Atom wmDelete;
    Window winDummy;
    unsigned int borderDummy;
    
    static int first_time = 1;

    fullscreen = fs;

    if (fullscreen) {
        width = screen_width;
        height = screen_height;
        msgout (DEBUG, "width %d x %d\n", width, height);
    } else {
        width = 600;
        height = 480;
    }


    XF86VidModeQueryVersion(display, &vidModeMajorVersion,
                            &vidModeMinorVersion);

    msgout(DEBUG, "XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
           vidModeMinorVersion);


    /* get an appropriate visual */
    vi = glXChooseVisual(display, screen, attrListDbl);
    if (vi == NULL)
    {
        vi = glXChooseVisual(display, screen, attrListSgl);
        msgout(DEBUG, "Only Singlebuffered Visual!\n");
    }
    else
    {
        msgout(DEBUG, "Got Doublebuffered Visual!\n");
    }
    glXQueryVersion(display, &glxMajorVersion, &glxMinorVersion);
    
    msgout(DEBUG, "glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
    


    if (first_time) {
        /* create a GLX context */
        msgout (DEBUG, "create context\n");
        ctx = glXCreateContext(display, vi, 0, GL_TRUE);
        
        first_time = 0;
    }

    /* create a color map */
    cmap = XCreateColormap(display, RootWindow(display, vi->screen),
                           vi->visual, AllocNone);
    
    
    attr.colormap = cmap;
    attr.border_pixel = 0;

    if (fullscreen)
    {
        XF86VidModeSetViewPort(display, screen, 0, 0);

        dpyWidth = screen_width;
        dpyHeight = screen_height;

        msgout(DEBUG, "Resolution %dx%d\n", dpyWidth, dpyHeight);

        /* create a fullscreen window */
        attr.override_redirect = True;
        attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
            StructureNotifyMask | SubstructureRedirectMask;
        win = XCreateWindow(display, RootWindow(display, vi->screen),
                                   0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
                                   CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
                                   &attr);

        Atom wmproto[2];
        wmproto[0] = XInternAtom (display, "WM_DELETE_WINDOW", False);
        wmproto[1] = XInternAtom(display, "_BLACKBOX_STRUCTURE_MESSAGES", False);
        XSetWMProtocols(display, win, wmproto, 2);


        // this code fragment from xscreensaver - for making a blank mouse pointer
        XColor black;
        black.red = black.green = black.blue = 0;

        Pixmap bit;
        bit = XCreatePixmapFromBitmapData (display, win,
                                           "\000", 1, 1,
                                           0, //BlackPixelOfScreen (screen),
                                           0, //BlackPixelOfScreen (screen),
                                           1);
        empty_cursor = XCreatePixmapCursor (display, bit, bit, &black, &black,
                                             0, 0);
        XFreePixmap (display, bit);
        // -----

        //XUndefineCursor (display, win);

        //XDefineCursor (display, win, empty_cursor);

        //XWarpPointer(display, None, win, 0, 0, 0, 0, 0, 0);
        
        XMapRaised(display, win);
        int rc1 = XGrabKeyboard(display, win, True, GrabModeSync,
                                GrabModeAsync, CurrentTime);
        int rc2 = XGrabPointer(display, win, True, ButtonPressMask | ButtonMotionMask | PointerMotionMask,
                               GrabModeAsync, GrabModeAsync, win, empty_cursor, CurrentTime);
        msgout(DEBUG, "xgrabkeyboard rc = %d\n", rc1);
        msgout(DEBUG, "xgrabpointer rc = %d\n", rc2);
        XSync(display, 0);

    }
    else
    {
        /* create a window in window mode*/
        attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
            StructureNotifyMask;
        win = XCreateWindow(display, RootWindow(display, vi->screen),
                                   0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
                                   CWBorderPixel | CWColormap | CWEventMask, &attr);
        /* only set window title and handle wm_delete_events if in windowed mode */
        wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(display, win, &wmDelete, 1);
        XSetStandardProperties(display, win, title,
                               title, None, NULL, 0, NULL);
        XMapRaised(display, win);
    }       

    Atom kwm_atom = 0;
    long int data=1;
    kwm_atom = XInternAtom(display, "KWM_MODULE", False);
    
    XChangeProperty(display, win, kwm_atom, kwm_atom, 32,
                    PropModeReplace, (unsigned char *)&data, 1);
    
    sendClientMessage(display, screen, win, kwm_atom, (long) win);


    /* connect the glx-context to the window */
    glXMakeCurrent(display, win, ctx);
    XGetGeometry(display, win, &winDummy, &x, &y,
                 (unsigned int *)&width, (unsigned int *)&height, 
                 &borderDummy, &depth);

    msgout(DEBUG, "Depth %d\n", depth);

    if (glXIsDirect(display, ctx)) 
        msgout(DEBUG, "Congrats, you have Direct Rendering!\n");
    else {
        msgout (ERROR, 
                "glXIsDirect failed, no Direct Rendering possible!\n"
                "Please configure hardware acceleration.  Exiting.\n");
        fprintf (stderr, 
                 "3ddeskd: glXIsDirect failed, no Direct Rendering possible!\n"
                 "3ddeskd: Please configure hardware acceleration.  Exiting.\n");
        end_program(-1);
    }

    
    return 1;
}


int GLWindow::get_best_texture_size (int s)
{
    int width;
    for ( ; s >= 64; s >>= 1) {

        glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGB,
                     s, s, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, NULL);

        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);

        if (s == width) {
            msgout (DEBUG, "Found best texture size as %d\n", s);
            return s;
        }
    }
    return 64; // all cards should support 64
}


int GLWindow::kill_GL_window ()
{
    msgout (DEBUG, "kill window\n");

    XDestroyWindow(display, win);

    XSync(display, 0);

    if (fullscreen)
    {
        XF86VidModeSetViewPort(display, screen, 0, 0);
    }
    return 0;
}

int GLWindow::unhide_GL_window()
{
    if (fullscreen) {
        XF86VidModeSetViewPort(display, screen, 0, 0);
    }

    XMapRaised (display, win);
    
    if (fullscreen) {
        int rc1, rc2, retries = 0;

        // it seems this call can fail with return code 1
        // (AlreadyGrabbed) so we add a retry loop here.
 
        rc1 = XGrabKeyboard(display, win, True, GrabModeSync,
                            GrabModeAsync, CurrentTime);
        while (rc1 != 0 && 
               retries++ != 5000)  // no infinite loops!
        {
            //msgout (DEBUG, "xgrabkeyboard failed (%d)\n", rc1);
            rc1 = XGrabKeyboard(display, win, True, GrabModeSync,
                                GrabModeAsync, CurrentTime);
        }
        if (rc1 != 0)
            msgout(WARN, "xgrabkeyboard rc = %d\n", rc1);

        rc2 = XGrabPointer(display, win, True, ButtonPressMask | ButtonMotionMask | PointerMotionMask,
                           GrabModeAsync, GrabModeAsync, win, empty_cursor, CurrentTime);
        if (rc1 != 0 || rc2 != 0)
            msgout(WARN, "xgrabpointer rc = %d\n", rc2);
    }


    XSync(display, 0);
    return 0;
}

int GLWindow::hide_GL_window()
{
    XUnmapWindow (display, win);
    
    XSync(display, 0);

    if (fullscreen)
    {
        XF86VidModeSetViewPort(display, screen, 0, 0);
    }
    return 0;
}

void
GLWindow::resize_scene()
{
    resize_scene(width, height);
}

void
GLWindow::resize_scene(int w, int h)
{
    // Let's not core dump, no matter what.
    if (h == 0)
        h = 1;

    width = w;
    height = h;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);

} // END resize_scene


int GLWindow::grab_screenshot_data_portion(int x1, int y1, int x2, int y2)
{
    int texture_x1 = (int)ceil((cfg->texture_size * x1) / (float)screen_width); 
    int texture_x2 = (int)ceil((cfg->texture_size * x2) / (float)screen_width); 

    int texture_y1 = (int)ceil((cfg->texture_size * y1) / (float)screen_height); 
    int texture_y2 = (int)ceil((cfg->texture_size * y2) / (float)screen_height); 


    // deleted in destructor (life of object)
    if (!screenshot_data) {
        screenshot_data = new unsigned char [cfg->texture_size * cfg->texture_size * 4];
        if (screenshot_data == NULL) {
            msgout (ERROR, "out of memory\n");
            end_program(-1);
        }
    }


    Imlib_Image img = imlib_create_image(screen_width, screen_height);
    Imlib_Image imgfinal = imlib_create_image(cfg->texture_size, cfg->texture_size);

    /* set the display , visual, colormap and drawable we are using */
    imlib_context_set_image(img);
    imlib_copy_drawable_to_image((Pixmap)0, //Pixmap mask, 
                                 x1, y1,
                                 x2 - x1, y2 - y1,
                                 0, 0, //int destination_x, int destination_y,
                                 1 ); //char need_to_grab_x);

    imlib_context_set_image(imgfinal);
    //imlib_image_clear();

    //msgout (DEBUG, "---portion of textures is %dx%d to %dx%d\n",
    //        texture_x1, texture_y1, texture_x2, texture_y2);

    //msgout (DEBUG, "===real portion is %dx%d to %dx%d\n",
    //        x1, y1, x2, y2);

    imlib_blend_image_onto_image(img, 0, 
                                 0, 0, x2 - x1, y2 - y1, 
                                 texture_x1, texture_y1, 
                                 texture_x2 - texture_x1, texture_y2 - texture_y1);

    unsigned int *tmp = imlib_image_get_data();

    int w = imlib_image_get_width();
    int h = imlib_image_get_height();

    int x, y;
    int offset;
    int img_offset;

    w = h = cfg->texture_size;
    
    for (y = 0; y < h; y++) {

        for (x = 0; x < w; x++) {

            offset = (y * w + x) * 4;
            img_offset = (y * w + x);

            screenshot_data[offset]     = (tmp[img_offset] >> 16) & 0xff;
            screenshot_data[offset + 1] = (tmp[img_offset] >> 8) & 0xff;
            screenshot_data[offset + 2] =  tmp[img_offset] & 0xff;
            screenshot_data[offset + 3] = (tmp[img_offset] >> 24) & 0xff;


        }
    }
    imlib_free_image();
    imlib_context_set_image(img);
    imlib_free_image();
    return 0;
}


int GLWindow::grab_screenshot_data()
{
    // deleted in destructor (life of object)
    if (!screenshot_data) {
        msgout(DEBUG, "Allocating screenshot buffer %d bytes\n", cfg->texture_size * cfg->texture_size * 4);
        screenshot_data = new unsigned char [cfg->texture_size * cfg->texture_size * 4];
        if (screenshot_data == NULL) {
            msgout (ERROR, "out of memory\n");
            end_program(-1);
        }
    }

    Imlib_Image img = imlib_create_image_from_drawable((Pixmap)0,0,0,screen_width,screen_height,1);
   
    convert_imlib_image_to_opengl_data(cfg->texture_size, img, screenshot_data);

    imlib_context_set_image(img);
    imlib_free_image();
    return 0;
}


unsigned char *GLWindow::get_screenshot_data_ptr(void)
{
    return screenshot_data;
}

