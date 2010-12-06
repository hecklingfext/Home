// FontImp.cc for FbTk
// Copyright (c) 2002-2003 Henrik Kinnunen (fluxgen(at)users.sourceforge.net)
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

// $Id: FontImp.hh,v 1.3 2003/04/20 13:52:29 fluxgen Exp $

#ifndef FBTK_FONTIMP_HH
#define FBTK_FONTIMP_HH

#include "Color.hh"

#include <X11/Xlib.h>

#include <string>

namespace FbTk {

/**
   FontImp, second part of the bridge pattern for fonts
   pure interface class.
   @see Font
*/
class FontImp {
public:
    virtual ~FontImp() { }
    virtual bool load(const std::string &name) = 0;
    virtual void drawText(Drawable w, int screen, GC gc, const char *text, size_t len, int x, int y) const = 0;
    virtual unsigned int textWidth(const char * const text, unsigned int size) const = 0;
    virtual int ascent() const = 0;
    virtual int descent() const = 0;
    virtual unsigned int height() const = 0;
    virtual bool loaded() const = 0;
    virtual void rotate(float angle) { } // by default, no rotate support
protected:
    FontImp() { }
};

}; // end namespace FbTk

#endif // FBTK_FONTIMP_HH
