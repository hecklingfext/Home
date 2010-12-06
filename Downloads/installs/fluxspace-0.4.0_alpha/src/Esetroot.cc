/*
 * Esetroot -- Set the root pixmap.  This program enables non-Enlightenment
 *             users to use Eterm's support for pseudotransparency.
 *
 * Written by Nat Friedman with modifications by Gerald Britton
 *
 * Modified for use in fluxspace by Steve Cooper <stevencooper@isomedia.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <Imlib2.h>

#include "Esetroot.hh"

const char* propNameRoot     = "_XROOTPMAP_ID";
const char* propNameEsetroot = "ESETROOT_PMAP_ID";

Esetroot::Esetroot(Display* display_)
  : display(display_),
    prop_root(XInternAtom(display_, propNameRoot, False)),
    prop_esetroot(XInternAtom(display_, propNameEsetroot, False))
{
}

Esetroot::~Esetroot()
{
//?    SetPixmapProperty(None);
}

bool Esetroot::SetPixmapProperty(Pixmap newPixmap)
{
    if (prop_root == None)
        fprintf(stderr, "Fluxspace: X pixmap property unavailable.\n");
    if (prop_esetroot == None)
        fprintf(stderr, "Fluxspace: Esetroot pixmap property unavailable.\n");
    if (prop_root == None || prop_esetroot == None)
        return false;

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    Atom type;
    int format;
    unsigned long length, after;
    unsigned char *data_root, *data_esetroot;
    XGetWindowProperty(display,
                       root,
                       prop_root,
                       0L,
                       1L,
                       False,
                       AnyPropertyType,
                       &type,
                       &format,
                       &length,
                       &after,
                       &data_root);

    if (type == XA_PIXMAP)
    {
        XGetWindowProperty(display,
                           root,
                           prop_esetroot,
                           0L,
                           1L,
                           False,
                           AnyPropertyType,
                           &type,
                           &format,
                           &length,
                           &after,
                           &data_esetroot);

        // Free the previous pixmap
        if (data_root && data_esetroot)
            XFreePixmap(display, *((Pixmap*)data_root));
    }

    // Assume we're shutting down if no pixmap is specified.
    // TODO: handle this better
    if (newPixmap == None)
    {
        XKillClient(display, *((Pixmap*)data_root));
        return true;
    }

    XChangeProperty(display,
                    root,
                    prop_root,
                    XA_PIXMAP,
                    32,
                    PropModeReplace,
                    (unsigned char *)&newPixmap,
                    1);
    XChangeProperty(display,
                    root,
                    prop_esetroot,
                    XA_PIXMAP,
                    32,
                    PropModeReplace,
                    (unsigned char *)&newPixmap,
                    1);

    XSetCloseDownMode(display, RetainTemporary);
    XFlush(display);

    return true;
}

bool Esetroot::SetRoot(
    const char* fname,
    const char* bgcolor,
    bool        scale,
    bool        center,
    bool        fit,
    bool        mirror
)
{
    Imlib_Image im;
    Pixmap newPixmap = None, tempPixmap = None, imPixmap = None;
    GC gc;
    XGCValues gcv;
    XColor xcolor;
    int w, h, x, y;

    if (fname == NULL)
    {
        fprintf(stderr, "No image file specified\n");
        return false;
    }

    if (scale)
    {
        center = 0;
        mirror = 0;
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    Screen* scr = ScreenOfDisplay(display, screen);

    imlib_context_set_display(display);
    imlib_context_set_visual(DefaultVisual(display, DefaultScreen(display)));
    im = imlib_load_image_immediately(fname);
    if (im == NULL)
    {
        fprintf(stderr, "Unable to load image file \"%s\".\n", fname);
        return false;
    }

    imlib_context_set_image(im);

    if (scale)
    {
        w = scr->width;
        h = scr->height;
    }
    else if (mirror)
    {
        w = imlib_image_get_width() * 2;
        h = imlib_image_get_height() * 2;
    }
    else
    {
        w = imlib_image_get_width();
        h = imlib_image_get_height();
    }

    if (fit)
    {
        double x_ratio, y_ratio;

        x_ratio = ((double) scr->width) / ((double) w);
        y_ratio = ((double) scr->height) / ((double) h);
        if (x_ratio > y_ratio)
            x_ratio = y_ratio;
        w = (int) (w * x_ratio);
        h = (int) (h * x_ratio);
    }

    newPixmap = XCreatePixmap(display, root, scr->width, scr->height, DefaultDepth(display, screen));
    gcv.foreground = gcv.background = BlackPixel(display, screen);
    if (   bgcolor != NULL
        && XParseColor(display, DefaultColormap(display, screen), bgcolor, &xcolor)
        && XAllocColor(display, DefaultColormap(display, screen), &xcolor))
    {
        gcv.foreground = gcv.background = xcolor.pixel;
    }

    gc = XCreateGC(display, newPixmap, (GCForeground | GCBackground), &gcv);
    if (scale)
    {
        int ret = XFillRectangle(display, newPixmap, gc, 0, 0, w, h);
    }
    else if (center || fit)
    {
        int ret = XFillRectangle(display, newPixmap, gc, 0, 0, scr->width, scr->height);
        x = (scr->width - w) >> 1;
        y = (scr->height - h) >> 1;
    }
    else
    {
        x = 0;
        y = 0;
    }

    imlib_context_set_anti_alias(1);
    imlib_context_set_dither(1);
    imlib_context_set_blend(0);

    if (mirror)
    {
        tempPixmap = XCreatePixmap(display, root, w, h, DefaultDepth(display, screen));
        imlib_context_set_drawable(tempPixmap);
        imlib_render_image_on_drawable(0, 0);
        imlib_image_flip_horizontal();
        imlib_render_image_on_drawable(imlib_image_get_width(), 0);
        imlib_image_flip_vertical();
        imlib_render_image_on_drawable(imlib_image_get_width(), imlib_image_get_height());
        imlib_image_flip_horizontal();
        imlib_render_image_on_drawable(0, imlib_image_get_height());
    }
    else
    {
        imlib_context_set_drawable(root);
        imlib_render_pixmaps_for_whole_image_at_size(&tempPixmap, &imPixmap, w, h);
    }

    imlib_free_image();

    if (tempPixmap != None)
    {
        if (imPixmap)
        {
            XFreePixmap(display, imPixmap);
            imPixmap = None;
        }

        int ret = XSetTile(display, gc, tempPixmap);
        ret = XSetTSOrigin(display, gc, x, y);
        ret = XSetFillStyle(display, gc, FillTiled);
        if (center || fit)
            ret = XFillRectangle(display, newPixmap, gc, x, y, w, h);
        else
            ret = XFillRectangle(display, newPixmap, gc, x, y, scr->width, scr->height);
        XFreeGC(display, gc);

        XGrabServer(display);
        SetPixmapProperty(newPixmap);
        XSetWindowBackgroundPixmap(display, root, newPixmap);
        XClearWindow(display, root);
        XUngrabServer(display);
        XFlush(display);

        XFreePixmap(display, tempPixmap);
    }
    return 0;
}
