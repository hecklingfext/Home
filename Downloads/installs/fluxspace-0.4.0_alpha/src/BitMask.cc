//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// BitMask.cc
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

#include "BitMask.hh"
#include <string.h>
#include <stdio.h>

namespace
{
    void fixCoordinates(int& x, int& y, unsigned int width, unsigned int height)
    {
        if (x < 0)
            x = 0;
        else if (x >= width)
            x = width - 1;
        if (y < 0)
            y = 0;
        else if (y >= height)
            y = height - 1;
    }
};

BitMask::BitMask(
    Display*     display_,
    Drawable     drawable_,
    unsigned int width_,
    unsigned int height_)
  : display(display_),
    drawable(drawable_),
    dim(width_, height_),
    bits(NULL),
    pixmask(None)
{
    if (dim.bytes > 0)
        bits = new unsigned char[dim.bytes];
    clear();
}

BitMask::~BitMask()
{
    if (pixmask != None)
        XFreePixmap(display, pixmask);
    if (bits != NULL)
        delete [] bits;
}

void BitMask::invalidatePixmask()
{
    if (pixmask != None)
    {
        XFreePixmap(display, pixmask);
        pixmask = None;
    }
}

void BitMask::clear()
{
    // Nothing to work with?
    if (bits != NULL && dim.bytes > 0)
    {
        memset(bits, 0, dim.bytes);
        invalidatePixmask();
    }
}

// Fill rectangle with one bits.
void BitMask::setRectangle(int x, int y, unsigned int width, unsigned int height)
{
    // Nothing to work with?
    if (bits == NULL || dim.bytes == 0)
        return;

    // Sanitize the coordinates and dimensions.
    // Do nothing if it's a null rectangle.
    fixCoordinates(x, y, dim.width, dim.height);
    int tox = x + width  - 1;
    int toy = y + height - 1;
    fixCoordinates(tox, toy, dim.width, dim.height);
    if (tox < x && toy < y)
        return;

    // Fill a pixel row at a time.
    for (int row = y; row <= toy; row++)
        setBits(x, row, tox, row);

    // Need to rebuild the pixmask next time we're asked for it.
    invalidatePixmask();
}

// Fill bit range with ones, including both end points.
void BitMask::setBits(int fromx, int fromy, int tox, int toy)
{
    // Protect ourselves if we get bad bounds.
    fixCoordinates(fromx, fromy, dim.width, dim.height);
    fixCoordinates(tox,   toy,   dim.width, dim.height);
    unsigned int offset1 = dim.bitOffset(fromx, fromy);
    unsigned int offset2 = dim.bitOffset(tox,   toy);
    if (offset2 >= dim.bits || offset1 > offset2)
        return;

    // Set the rightmost bits of the first byte according to the
    // modulus of the starting offset.
    unsigned int  nbits   = 8 - (offset1 % 8);
    unsigned char curbyte = (0xff << (8 - nbits));

    // Loop through the whole bytes between the first and last bytes
    unsigned int nbyte = (offset1 + nbits - 1) / 8;
    for (unsigned int offset = offset1 + nbits; offset <= offset2; offset += 8)
    {
        // Flush the previous byte without further chopping.
        // Within the loop we deal with a byte at a time.
        bits[nbyte] = curbyte;

        // All the other bytes, with the possible exception of the last are filled.
        curbyte = 0xff;

        nbyte++;
    }

    // Chop any unused bits off of the last byte.
    curbyte &= (0xff >> (8 - ((offset2 % 8) + 1)));
    bits[nbyte] = curbyte;
}

Pixmap BitMask::getPixmask()
{
    if (pixmask == None)
    {
        if (bits != NULL)
        {
            char* charBits = reinterpret_cast<char*>(bits);
            pixmask = XCreateBitmapFromData(display, drawable, charBits, dim.width, dim.height);
        }
    }
    return pixmask;
}
