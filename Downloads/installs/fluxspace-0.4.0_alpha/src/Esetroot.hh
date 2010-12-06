/*

 * Esetroot -- Set the root pixmap.  This program enables non-Enlightenment
 *             users to use Eterm's support for pseudotransparency.
 *
 * Written by Nat Friedman with modifications by Gerald Britton
 *
 * Modified for use in fluxspace by Steve Cooper <stevencooper@isomedia.com>
 *
 */

#include <X11/Xlib.h>
#include <string>

class Esetroot
{
  public:
    Esetroot(Display* display_);
    virtual ~Esetroot();

    bool SetRoot(const char* fname,
                 const char* bgcolor = NULL,
                 bool        scale   = false,
                 bool        center  = false,
                 bool        fit     = false,
                 bool        mirror  = false);
  private:
    Display* display;
    Atom     prop_root;
    Atom     prop_esetroot;

    bool SetPixmapProperty(Pixmap p);
};
