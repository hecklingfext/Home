//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// BitMask.hh
//
//  Utility class to construct a border mask
//
//  Copyright (c) 2004 Steve Cooper, stevencooper@isomedia.com
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#include <X11/Xlib.h>

class BitMaskDimensions
{
  public:
    BitMaskDimensions(unsigned int width_, unsigned int height_)
      : width(width_),
        height(height_),
        bytesPerRow((width_ + 7) / 8),
        bitsPerRow((width_ + 7) / 8 * 8),
        bytes(((width_ + 7) / 8) * height_),
        bits((((width_ + 7) / 8) * height_) * 8)
    {}

    unsigned int bitOffset(int x, int y) { return (y * bitsPerRow + x); }

    unsigned int width, height, bytesPerRow, bitsPerRow, bytes, bits;
};

// Class to construct a bitmap for an XBM mask
class BitMask
{
  public:

    BitMask(Display* display_, Drawable drawable_, unsigned int width_, unsigned int height_);
    virtual ~BitMask();

    void clear();
    void setRectangle(int x, int y, unsigned int width, unsigned int height);
    void clearRectangle(int x, int y, unsigned int width, unsigned int height);

    Pixmap getPixmask();

  protected:
    void invalidatePixmask();
    void setBits(int fromx, int fromy, int tox, int toy);

    Display*          display;
    Drawable          drawable;
    BitMaskDimensions dim;
    unsigned char*    bits;
    Pixmap pixmask;
};
