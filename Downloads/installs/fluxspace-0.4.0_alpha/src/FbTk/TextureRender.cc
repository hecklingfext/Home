// TextureRender.cc for fluxbox
// Copyright (c) 2002 - 2003 Henrik Kinnunen (fluxgen at users.sourceforge.net)
//
// from Image.cc for Blackbox - an X11 Window manager
// Copyright (c) 1997 - 2000 Brad Hughes (bhughes at tcac.net)
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

// $Id: TextureRender.cc,v 1.6 2004/01/11 12:52:37 fluxgen Exp $

#include "TextureRender.hh"

#include "ImageControl.hh"
#include "App.hh"
#include "FbPixmap.hh"
#include "GContext.hh"

#include <iostream>
#include <cstring>
#include <string>
#include <cstdio>
using namespace std;

namespace FbTk {

TextureRender::TextureRender(ImageControl &imgctrl, 
                             unsigned int w, unsigned int h, 
                             XColor *_colors, size_t num_colors):
    control(imgctrl),
    colors(_colors),
    ncolors(ncolors),
    xtable(0), ytable(0) {

    width = ((signed) w > 0) ? w : 1;
    height = ((signed) h > 0) ? h : 1;
    // clamp to "normal" size
    if (width > 3200) {
        cerr<<"TextureRender: Warning! Width > 3200 setting Width = 3200"<<endl;
        width = 3200;
    }

    if (height > 3200) {
        cerr<<"TextureRender: Warning! Height > 3200 setting Height = 3200"<<endl;
        height = 3200;
    }

    red = new unsigned char[width * height];
    if (red == 0) {
        char sbuf[128];
        sprintf(sbuf, "%d", width*height);
        throw string("TextureRender::TextureRender(): Out of memory while allocating red buffer."+ string(sbuf));
    }


    green = new unsigned char[width * height];
    if (green == 0) {
        char sbuf[128];
        sprintf(sbuf, "%d", width*height);
        throw string("TextureRender::TextureRender(): Out of memory while allocating green buffer. size " + string(sbuf));
    }

    blue = new unsigned char[width * height];
    if (blue == 0) {
        char sbuf[128];
        sprintf(sbuf, "%d", width*height);
        throw string("TextureRender::TextureRender(): Out of memory while allocating blue buffer. size " + string(sbuf));
    }

    cpc = imgctrl.colorsPerChannel();
    cpccpc = cpc * cpc;

    imgctrl.colorTables(&red_table, &green_table, &blue_table,
                        &red_offset, &green_offset, &blue_offset,
                        &red_bits, &green_bits, &blue_bits);
	
}


TextureRender::~TextureRender() {
    delete [] red;
    delete [] green;
    delete [] blue;	
}


Pixmap TextureRender::render(const FbTk::Texture &texture) {
    if (texture.pixmap().drawable() != 0)
        return renderPixmap(texture);
    else if (texture.type() & FbTk::Texture::PARENTRELATIVE)
        return ParentRelative;
    else if (texture.type() & FbTk::Texture::SOLID)
        return renderSolid(texture);
    else if (texture.type() & FbTk::Texture::GRADIENT)
        return renderGradient(texture);

    return None;
}


Pixmap TextureRender::renderSolid(const FbTk::Texture &texture) {

    Display *disp = FbTk::App::instance()->display();

    FbPixmap pixmap(RootWindow(disp, control.screenNum()), 
                  width, height,
                  control.depth());

    if (pixmap.drawable() == None) {
        cerr<<"FbTk::TextureRender::render_solid(): error creating pixmap"<<endl;
        return None;
    }


    FbTk::GContext gc(pixmap), 
        hgc(pixmap), lgc(pixmap);

    gc.setForeground(texture.color());
    gc.setFillStyle(FillSolid);

    hgc.setForeground(texture.hiColor());

    pixmap.fillRectangle(gc.gc(), 0, 0, width, height);
	
    using namespace FbTk;
	
    if (texture.type() & Texture::INTERLACED) {
        lgc.setForeground(texture.colorTo());
        register unsigned int i = 0;
        for (; i < height; i += 2)
            pixmap.drawLine(lgc.gc(), 0, i, width, i);

    }

    lgc.setForeground(texture.loColor());

    if (texture.type() & Texture::BEVEL1) {
        if (texture.type() & Texture::RAISED) {
            pixmap.drawLine(lgc.gc(), 
                            0, height - 1, width - 1, height - 1);
            pixmap.drawLine(lgc.gc(),
                            width - 1, height - 1, width - 1, 0);

            pixmap.drawLine(hgc.gc(),
                            0, 0, width - 1, 0);
            pixmap.drawLine(hgc.gc(),
                            0, height - 1, 0, 0);
        } else if (texture.type() & Texture::SUNKEN) {
            pixmap.drawLine(hgc.gc(),
                            0, height - 1, width - 1, height - 1);
            pixmap.drawLine(hgc.gc(),
                            width - 1, height - 1, width - 1, 0);

            pixmap.drawLine(lgc.gc(),
                            0, 0, width - 1, 0);
            pixmap.drawLine(lgc.gc(),
                            0, height - 1, 0, 0);
        }
    } else if (texture.type() & Texture::BEVEL2) {
        if (texture.type() & Texture::RAISED) {
            pixmap.drawLine(lgc.gc(),
                            1, height - 3, width - 3, height - 3);
            pixmap.drawLine(lgc.gc(),
                            width - 3, height - 3, width - 3, 1);

            pixmap.drawLine(hgc.gc(),
                            1, 1, width - 3, 1);
            pixmap.drawLine(hgc.gc(),
                            1, height - 3, 1, 1);
        } else if (texture.type() & Texture::SUNKEN) {
            pixmap.drawLine(hgc.gc(),
                            1, height - 3, width - 3, height - 3);
            pixmap.drawLine(hgc.gc(),
                            width - 3, height - 3, width - 3, 1);

            pixmap.drawLine(lgc.gc(),
                            1, 1, width - 3, 1);
            pixmap.drawLine(lgc.gc(),
                            1, height - 3, 1, 1);
        }
    }

    return pixmap.release();
}


Pixmap TextureRender::renderGradient(const FbTk::Texture &texture) {

    bool inverted = false;

    using namespace FbTk;

    interlaced = texture.type() & Texture::INTERLACED;

    if (texture.type() & Texture::SUNKEN) {
        from = &(texture.colorTo());
        to = &(texture.color());

        if (! (texture.type() & Texture::INVERT)) 
            inverted = true;
    } else {
        from = &(texture.color());
        to = &(texture.colorTo());

        if (texture.type() & Texture::INVERT) 
            inverted = true;
    }

    control.getGradientBuffers(width, height, &xtable, &ytable);

    if (texture.type() & Texture::DIAGONAL) 
        dgradient();
    else if (texture.type() & Texture::ELLIPTIC) 
        egradient();
    else if (texture.type() & Texture::HORIZONTAL) 
        hgradient();
    else if (texture.type() & Texture::PYRAMID) 
        pgradient();
    else if (texture.type() & Texture::RECTANGLE) 
        rgradient();
    else if (texture.type() & Texture::VERTICAL) 
        vgradient();
    else if (texture.type() & Texture::CROSSDIAGONAL) 
        cdgradient();
    else if (texture.type() & Texture::PIPECROSS) 
        pcgradient();

    if (texture.type() & Texture::BEVEL1)
        bevel1();
    else if (texture.type() & Texture::BEVEL2)
        bevel2();

    if (inverted)
        invert();

    return renderPixmap();

}

Pixmap TextureRender::renderPixmap(const FbTk::Texture &src_texture) {
    if (width != src_texture.pixmap().width() ||
        height != src_texture.pixmap().height()) {
        // copy src_texture's pixmap and 
        // scale to fit our size
        FbPixmap new_pm(src_texture.pixmap());
        // if not tiled then scale it
        if (! (src_texture.type() & Texture::TILED))
            new_pm.scale(width, height);

        return new_pm.release();
    }
    // return copy of pixmap
    return FbPixmap(src_texture.pixmap()).release();
}

XImage *TextureRender::renderXImage() {
    Display *disp = FbTk::App::instance()->display();
    XImage *image =
        XCreateImage(disp,
                     DefaultVisual(disp, control.screenNum()), control.depth(), ZPixmap, 0, 0,
                     width, height, 32, 0);

    if (! image) {
        cerr<<"FbTk::TextureRender::renderXImage(): error creating XImage"<<endl;
        return 0;
    }

    image->data = 0;

    unsigned char *d = new unsigned char[image->bytes_per_line * (height + 1)];
    register unsigned int x, y, dithx, dithy, r, g, b, o, er, eg, eb, offset;

    unsigned char *pixel_data = d, *ppixel_data = d;
    unsigned long pixel;

    o = image->bits_per_pixel + ((image->byte_order == MSBFirst) ? 1 : 0);

    if (control.doDither() && width > 1 && height > 1) {
        unsigned char dither4[4][4] = { 
            {0, 4, 1, 5},
            {6, 2, 7, 3},
            {1, 5, 0, 4},
            {7, 3, 6, 2} };

#ifdef ORDEREDPSEUDO
        unsigned char dither8[8][8] = { 
            { 0, 32,  8, 40, 2,	34, 10, 42 },
            { 48, 16, 56, 24, 50, 18, 58, 26 },
            { 12, 44,  4, 36, 14, 46, 6, 38 },
            { 60, 28, 52, 20, 62, 30, 54, 22 },
            { 3, 35, 11, 43, 1,	33, 9, 41 },
            { 51, 19, 59, 27, 49, 17, 57, 25 },
            { 15, 47,  7, 39, 13, 45, 5, 37 },
            { 63, 31, 55, 23, 61, 29, 53, 21 } };
#endif // ORDEREDPSEUDO

        switch (control.visual()->c_class) {
        case TrueColor:
            // algorithm: ordered dithering... many many thanks to rasterman
            // (raster@rasterman.com) for telling me about this... portions of this
            // code is based off of his code in Imlib
            for (y = 0, offset = 0; y < height; y++) {
                dithy = y & 0x3;

                for (x = 0; x < width; x++, offset++) {
                    dithx = x & 0x3;
                    r = red[offset];
                    g = green[offset];
                    b = blue[offset];

                    er = r & (red_bits - 1);
                    eg = g & (green_bits - 1);
                    eb = b & (blue_bits - 1);

                    r = red_table[r];
                    g = green_table[g];
                    b = blue_table[b];

                    if ((dither4[dithy][dithx] < er) && (r < red_table[255])) r++;
                    if ((dither4[dithy][dithx] < eg) && (g < green_table[255])) g++;
                    if ((dither4[dithy][dithx] < eb) && (b < blue_table[255])) b++;

                    pixel = (r << red_offset) | (g << green_offset) | (b << blue_offset);

                    switch (o) {
                    case	8: //	8bpp
                        *pixel_data++ = pixel;
                        break;

                    case 16: // 16bpp LSB
                        *pixel_data++ = pixel;
                        *pixel_data++ = pixel >> 8;
                        break;

                    case 17: // 16bpp MSB
                        *pixel_data++ = pixel >> 8;
                        *pixel_data++ = pixel;
                        break;

                    case 24: // 24bpp LSB
                        *pixel_data++ = pixel;
                        *pixel_data++ = pixel >> 8;
                        *pixel_data++ = pixel >> 16;
                        break;

                    case 25: // 24bpp MSB
                        *pixel_data++ = pixel >> 16;
                        *pixel_data++ = pixel >> 8;
                        *pixel_data++ = pixel;
                        break;

                    case 32: // 32bpp LSB
                        *pixel_data++ = pixel;
                        *pixel_data++ = pixel >> 8;
                        *pixel_data++ = pixel >> 16;
                        *pixel_data++ = pixel >> 24;
                        break;

                    case 33: // 32bpp MSB
                        *pixel_data++ = pixel >> 24;
                        *pixel_data++ = pixel >> 16;
                        *pixel_data++ = pixel >> 8;
                        *pixel_data++ = pixel;
                        break;
                    }
                }

                pixel_data = (ppixel_data += image->bytes_per_line);
            }

            break;

        case StaticColor:
        case PseudoColor: {
#ifndef	 ORDEREDPSEUDO
            short *terr,
                *rerr = new short[width + 2],
                *gerr = new short[width + 2],
                *berr = new short[width + 2],
                *nrerr = new short[width + 2],
                *ngerr = new short[width + 2],
                *nberr = new short[width + 2];
            int rr, gg, bb, rer, ger, ber;
            int dd = 255 / control.colorsPerChannel();

            for (x = 0; x < width; x++) {
                *(rerr + x) = *(red + x);
                *(gerr + x) = *(green + x);
                *(berr + x) = *(blue + x);
            }

            *(rerr + x) = *(gerr + x) = *(berr + x) = 0;
#endif // ORDEREDPSEUDO

            for (y = 0, offset = 0; y < height; y++) {
#ifdef		ORDEREDPSEUDO
                dithy = y & 7;

                for (x = 0; x < width; x++, offset++) {
                    dithx = x & 7;

                    r = red[offset];
                    g = green[offset];
                    b = blue[offset];

                    er = r & (red_bits - 1);
                    eg = g & (green_bits - 1);
                    eb = b & (blue_bits - 1);

                    r = red_table[r];
                    g = green_table[g];
                    b = blue_table[b];

                    if ((dither8[dithy][dithx] < er) && (r < red_table[255])) r++;
                    if ((dither8[dithy][dithx] < eg) && (g < green_table[255])) g++;
                    if ((dither8[dithy][dithx] < eb) && (b < blue_table[255])) b++;

                    pixel = (r * cpccpc) + (g * cpc) + b;
                    *(pixel_data++) = colors[pixel].pixel;
                }

                pixel_data = (ppixel_data += image->bytes_per_line);
            }
#else // !ORDEREDPSEUDO
            if (y < (height - 1)) {
                int i = offset + width;
                for (x = 0; x < width; x++, i++) {
                    *(nrerr + x) = *(red + i);
                    *(ngerr + x) = *(green + i);
                    *(nberr + x) = *(blue + i);
                }

                *(nrerr + x) = *(red + (--i));
                *(ngerr + x) = *(green + i);
                *(nberr + x) = *(blue + i);
            }

            for (x = 0; x < width; x++) {
                rr = rerr[x];
                gg = gerr[x];
                bb = berr[x];

                if (rr > 255) rr = 255; else if (rr < 0) rr = 0;
                if (gg > 255) gg = 255; else if (gg < 0) gg = 0;
                if (bb > 255) bb = 255; else if (bb < 0) bb = 0;

                r = red_table[rr];
                g = green_table[gg];
                b = blue_table[bb];

                rer = rerr[x] - r*dd;
                ger = gerr[x] - g*dd;
                ber = berr[x] - b*dd;

                pixel = (r * cpccpc) + (g * cpc) + b;
                *pixel_data++ = colors[pixel].pixel;

                r = rer >> 1;
                g = ger >> 1;
                b = ber >> 1;
                rerr[x+1] += r;
                gerr[x+1] += g;
                berr[x+1] += b;
                nrerr[x] += r;
                ngerr[x] += g;
                nberr[x] += b;
            }

            offset += width;

            pixel_data = (ppixel_data += image->bytes_per_line);

            terr = rerr;
            rerr = nrerr;
            nrerr = terr;

            terr = gerr;
            gerr = ngerr;
            ngerr = terr;

            terr = berr;
            berr = nberr;
            nberr = terr;
        }

            delete [] rerr;
            delete [] gerr;
            delete [] berr;
            delete [] nrerr;
            delete [] ngerr;
            delete [] nberr;
#endif // ORDEREDPSUEDO

        } break;

        /*
          case StaticGray:
          case GrayScale:
          for (y = 0, offset = 0; y < height; y++) {
          dithy = y & 0x3;

          for (x = 0; x < width; x++, offset++) {
          dithx = x & 0x3;

          r = *(red + offset);
          g = *(green + offset);
          b = *(blue + offset);

          er = r & 0x7;
          eg = g & 0x7;
          eb = b & 0x7;

          if ((dither[dithy][dithx] < er) && (r < (256 - 8)))
          r += 8;
          if ((dither[dithy][dithx] < (eg << 1)) && (g < (256 - 4)))
          g += 4;
          if ((dither[dithy][dithx] < eb) && (b < (256 - 8)))
          b += 8;

          r = *(red_table + r);
          g = *(green_table + g);
          b = *(blue_table + b);

          g = ((r * 30) + (g * 59) + (b * 11)) / 100;
          *pixel_data++ = colors[g].pixel;
          }

          pixel_data = (ppixel_data += image->bytes_per_line);
          }

          break;
        */

    default:
        cerr<<"TextureRender::renderXImage(): unsupported visual"<<endl;
        delete [] d;
        XDestroyImage(image);
        return (XImage *) 0;
    }
} else {
    switch (control.visual()->c_class) {
    case StaticColor:
    case PseudoColor:
        for (y = 0, offset = 0; y < height; y++) {
            for (x = 0; x < width; x++, offset++) {
                r = red_table[red[offset]];
                g = green_table[green[offset]];
                b = blue_table[blue[offset]];

                pixel = (r * cpccpc) + (g * cpc) + b;
                *pixel_data++ = colors[pixel].pixel;
            }

            pixel_data = (ppixel_data += image->bytes_per_line);
        }

        break;

    case TrueColor:
        for (y = 0, offset = 0; y < height; y++) {
            for (x = 0; x < width; x++, offset++) {
                r = red_table[red[offset]];
                g = green_table[green[offset]];
                b = blue_table[blue[offset]];

                pixel = (r << red_offset) | (g << green_offset) | (b << blue_offset);

                switch (o) {
                case	8: //	8bpp
                    *pixel_data++ = pixel;
                    break;

                case 16: // 16bpp LSB
                    *pixel_data++ = pixel;
                    *pixel_data++ = pixel >> 8;
                    break;

                case 17: // 16bpp MSB
                    *pixel_data++ = pixel >> 8;
                    *pixel_data++ = pixel;
                    break;

                case 24: // 24bpp LSB
                    *pixel_data++ = pixel;
                    *pixel_data++ = pixel >> 8;
                    *pixel_data++ = pixel >> 16;
                    break;

                case 25: // 24bpp MSB
                    *pixel_data++ = pixel >> 16;
                    *pixel_data++ = pixel >> 8;
                    *pixel_data++ = pixel;
                    break;

                case 32: // 32bpp LSB
                    *pixel_data++ = pixel;
                    *pixel_data++ = pixel >> 8;
                    *pixel_data++ = pixel >> 16;
                    *pixel_data++ = pixel >> 24;
                    break;

                case 33: // 32bpp MSB
                    *pixel_data++ = pixel >> 24;
                    *pixel_data++ = pixel >> 16;
                    *pixel_data++ = pixel >> 8;
                    *pixel_data++ = pixel;
                    break;
                }
            }

            pixel_data = (ppixel_data += image->bytes_per_line);
        }

        break;

    case StaticGray:
    case GrayScale:
        for (y = 0, offset = 0; y < height; y++) {
            for (x = 0; x < width; x++, offset++) {
                r = *(red_table + *(red + offset));
                g = *(green_table + *(green + offset));
                b = *(blue_table + *(blue + offset));

                g = ((r * 30) + (g * 59) + (b * 11)) / 100;
                *pixel_data++ = colors[g].pixel;
            }

            pixel_data = (ppixel_data += image->bytes_per_line);
        }

	break;

    default:
        cerr<<"TextureRender::renderXImage(): unsupported visual"<<endl;
        delete [] d;
        XDestroyImage(image);
        return (XImage *) 0;
    }
}

image->data = (char *) d;
return image;
}


Pixmap TextureRender::renderPixmap() {
    Display *disp = FbTk::App::instance()->display();
    FbPixmap pixmap(RootWindow(disp, control.screenNum()), 
                    width, height, control.depth());

    if (pixmap.drawable() == None) {
        cerr<<"TextureRender::renderPixmap(): error creating pixmap"<<endl;
        return None;
    }

    XImage *image = renderXImage();

    if (! image) {
        return None;
    } else if (! image->data) {
        XDestroyImage(image);
        return None;
    }

    XPutImage(disp, pixmap.drawable(),
              DefaultGC(disp, control.screenNum()),
              image, 0, 0, 0, 0, width, height);

    if (image->data != 0) {
        delete [] image->data;
        image->data = 0;
    }

    XDestroyImage(image);

    return pixmap.release();
}


void TextureRender::bevel1() {
    if (! (width > 2 && height > 2))
        return;

    unsigned char *pr = red, *pg = green, *pb = blue;

    register unsigned char r, g, b, rr ,gg ,bb;
    register unsigned int w = width, h = height - 1, wh = w * h;

    while (--w) {
        r = *pr;
        rr = r + (r >> 1);
        if (rr < r) rr = ~0;
        g = *pg;
        gg = g + (g >> 1);
        if (gg < g) gg = ~0;
        b = *pb;
        bb = b + (b >> 1);
        if (bb < b) bb = ~0;

        *pr = rr;
        *pg = gg;
        *pb = bb;

        r = *(pr + wh);
        rr = (r >> 2) + (r >> 1);
        if (rr > r) rr = 0;
        g = *(pg + wh);
        gg = (g >> 2) + (g >> 1);
        if (gg > g) gg = 0;
        b = *(pb + wh);
        bb = (b >> 2) + (b >> 1);
        if (bb > b) bb = 0;

        *((pr++) + wh) = rr;
        *((pg++) + wh) = gg;
        *((pb++) + wh) = bb;
    }

    r = *pr;
    rr = r + (r >> 1);
    if (rr < r) rr = ~0;
    g = *pg;
    gg = g + (g >> 1);
    if (gg < g) gg = ~0;
    b = *pb;
    bb = b + (b >> 1);
    if (bb < b) bb = ~0;

    *pr = rr;
    *pg = gg;
    *pb = bb;

    r = *(pr + wh);
    rr = (r >> 2) + (r >> 1);
    if (rr > r) rr = 0;
    g = *(pg + wh);
    gg = (g >> 2) + (g >> 1);
    if (gg > g) gg = 0;
    b = *(pb + wh);
    bb = (b >> 2) + (b >> 1);
    if (bb > b) bb = 0;

    *(pr + wh) = rr;
    *(pg + wh) = gg;
    *(pb + wh) = bb;

    pr = red + width;
    pg = green + width;
    pb = blue + width;

    while (--h) {
        r = *pr;
        rr = r + (r >> 1);
        if (rr < r) rr = ~0;
        g = *pg;
        gg = g + (g >> 1);
        if (gg < g) gg = ~0;
        b = *pb;
        bb = b + (b >> 1);
        if (bb < b) bb = ~0;

        *pr = rr;
        *pg = gg;
        *pb = bb;

        pr += width - 1;
        pg += width - 1;
        pb += width - 1;

        r = *pr;
        rr = (r >> 2) + (r >> 1);
        if (rr > r) rr = 0;
        g = *pg;
        gg = (g >> 2) + (g >> 1);
        if (gg > g) gg = 0;
        b = *pb;
        bb = (b >> 2) + (b >> 1);
        if (bb > b) bb = 0;

        *(pr++) = rr;
        *(pg++) = gg;
        *(pb++) = bb;
    }

    r = *pr;
    rr = r + (r >> 1);
    if (rr < r) rr = ~0;
    g = *pg;
    gg = g + (g >> 1);
    if (gg < g) gg = ~0;
    b = *pb;
    bb = b + (b >> 1);
    if (bb < b) bb = ~0;

    *pr = rr;
    *pg = gg;
    *pb = bb;

    pr += width - 1;
    pg += width - 1;
    pb += width - 1;

    r = *pr;
    rr = (r >> 2) + (r >> 1);
    if (rr > r) rr = 0;
    g = *pg;
    gg = (g >> 2) + (g >> 1);
    if (gg > g) gg = 0;
    b = *pb;
    bb = (b >> 2) + (b >> 1);
    if (bb > b) bb = 0;

    *pr = rr;
    *pg = gg;
    *pb = bb;
}


void TextureRender::bevel2() {
    if (! (width > 4 && height > 4))
        return;

    unsigned char r, g, b, rr ,gg ,bb, *pr = red + width + 1,
        *pg = green + width + 1, *pb = blue + width + 1;
    unsigned int w = width - 2, h = height - 1, wh = width * (height - 3);

    while (--w) {
        r = *pr;
        rr = r + (r >> 1);
        if (rr < r) rr = ~0;
        g = *pg;
        gg = g + (g >> 1);
        if (gg < g) gg = ~0;
        b = *pb;
        bb = b + (b >> 1);
        if (bb < b) bb = ~0;

        *pr = rr;
        *pg = gg;
        *pb = bb;

        r = *(pr + wh);
        rr = (r >> 2) + (r >> 1);
        if (rr > r) rr = 0;
        g = *(pg + wh);
        gg = (g >> 2) + (g >> 1);
        if (gg > g) gg = 0;
        b = *(pb + wh);
        bb = (b >> 2) + (b >> 1);
        if (bb > b) bb = 0;

        *((pr++) + wh) = rr;
        *((pg++) + wh) = gg;
        *((pb++) + wh) = bb;
    }

    pr = red + width;
    pg = green + width;
    pb = blue + width;

    while (--h) {
        r = *pr;
        rr = r + (r >> 1);
        if (rr < r) rr = ~0;
        g = *pg;
        gg = g + (g >> 1);
        if (gg < g) gg = ~0;
        b = *pb;
        bb = b + (b >> 1);
        if (bb < b) bb = ~0;

        *(++pr) = rr;
        *(++pg) = gg;
        *(++pb) = bb;

        pr += width - 3;
        pg += width - 3;
        pb += width - 3;

        r = *pr;
        rr = (r >> 2) + (r >> 1);
        if (rr > r) rr = 0;
        g = *pg;
        gg = (g >> 2) + (g >> 1);
        if (gg > g) gg = 0;
        b = *pb;
        bb = (b >> 2) + (b >> 1);
        if (bb > b) bb = 0;

        *(pr++) = rr;
        *(pg++) = gg;
        *(pb++) = bb;

        pr++; pg++; pb++;
    }
}


void TextureRender::invert() {
    register unsigned int i, j, wh = (width * height) - 1;
    unsigned char tmp;

    for (i = 0, j = wh; j > i; j--, i++) {
        tmp = *(red + j);
        *(red + j) = *(red + i);
        *(red + i) = tmp;

        tmp = *(green + j);
        *(green + j) = *(green + i);
        *(green + i) = tmp;

        tmp = *(blue + j);
        *(blue + j) = *(blue + i);
        *(blue + i) = tmp;
    }
}


void TextureRender::dgradient() {
    // diagonal gradient code was written by Mike Cole <mike@mydot.com>
    // modified for interlacing by Brad Hughes

    float drx, dgx, dbx, dry, dgy, dby, yr = 0.0, yg = 0.0, yb = 0.0,
        xr = (float) from->red(),
        xg = (float) from->green(),
        xb = (float) from->blue();
    unsigned char *pr = red, *pg = green, *pb = blue;
    unsigned int w = width * 2, h = height * 2;
    unsigned int *xt = xtable, *yt = ytable;

    register unsigned int x, y;

    dry = drx = (float) (to->red() - from->red());
    dgy = dgx = (float) (to->green() - from->green());
    dby = dbx = (float) (to->blue() - from->blue());

    // Create X table
    drx /= w;
    dgx /= w;
    dbx /= w;

    for (x = 0; x < width; x++) {
        *(xt++) = (unsigned char) (xr);
        *(xt++) = (unsigned char) (xg);
        *(xt++) = (unsigned char) (xb);

        xr += drx;
        xg += dgx;
        xb += dbx;
    }

    // Create Y table
    dry /= h;
    dgy /= h;
    dby /= h;

    for (y = 0; y < height; y++) {
        *(yt++) = ((unsigned char) yr);
        *(yt++) = ((unsigned char) yg);
        *(yt++) = ((unsigned char) yb);

        yr += dry;
        yg += dgy;
        yb += dby;
    }

    // Combine tables to create gradient


    if (! interlaced) {


        // normal dgradient
        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                *(pr++) = *(xt++) + *(yt);
                *(pg++) = *(xt++) + *(yt + 1);
                *(pb++) = *(xt++) + *(yt + 2);
            }
        }

    } else {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                if (y & 1) {
                    channel = *(xt++) + *(yt);
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pr++) = channel2;

                    channel = *(xt++) + *(yt + 1);
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pg++) = channel2;

                    channel = *(xt++) + *(yt + 2);
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pb++) = channel2;
                } else {
                    channel = *(xt++) + *(yt);
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pr++) = channel2;

                    channel = *(xt++) + *(yt + 1);
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pg++) = channel2;

                    channel = *(xt++) + *(yt + 2);
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pb++) = channel2;
                }
            }
        }
    }


}


void TextureRender::hgradient() {
    float drx, dgx, dbx,
        xr = (float) from->red(),
        xg = (float) from->green(),
        xb = (float) from->blue();
    unsigned char *pr = red, *pg = green, *pb = blue;

    register unsigned int x, y;

    drx = (float) (to->red() - from->red());
    dgx = (float) (to->green() - from->green());
    dbx = (float) (to->blue() - from->blue());

    drx /= width;
    dgx /= width;
    dbx /= width;

    if (interlaced && height > 2) {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (x = 0; x < width; x++, pr++, pg++, pb++) {
            channel = (unsigned char) xr;
            channel2 = (channel >> 1) + (channel >> 2);
            if (channel2 > channel) channel2 = 0;
            *pr = channel2;

            channel = (unsigned char) xg;
            channel2 = (channel >> 1) + (channel >> 2);
            if (channel2 > channel) channel2 = 0;
            *pg = channel2;

            channel = (unsigned char) xb;
            channel2 = (channel >> 1) + (channel >> 2);
            if (channel2 > channel) channel2 = 0;
            *pb = channel2;


            channel = (unsigned char) xr;
            channel2 = channel + (channel >> 3);
            if (channel2 < channel) channel2 = ~0;
            *(pr + width) = channel2;

            channel = (unsigned char) xg;
            channel2 = channel + (channel >> 3);
            if (channel2 < channel) channel2 = ~0;
            *(pg + width) = channel2;

            channel = (unsigned char) xb;
            channel2 = channel + (channel >> 3);
            if (channel2 < channel) channel2 = ~0;
            *(pb + width) = channel2;

            xr += drx;
            xg += dgx;
            xb += dbx;
        }

        pr += width;
        pg += width;
        pb += width;

        int offset;

        for (y = 2; y < height; y++, pr += width, pg += width, pb += width) {
            if (y & 1) offset = width; else offset = 0;

            memcpy(pr, (red + offset), width);
            memcpy(pg, (green + offset), width);
            memcpy(pb, (blue + offset), width);
        }
    } else {

        // normal hgradient
        for (x = 0; x < width; x++) {
            *(pr++) = (unsigned char) (xr);
            *(pg++) = (unsigned char) (xg);
            *(pb++) = (unsigned char) (xb);

            xr += drx;
            xg += dgx;
            xb += dbx;
        }

        for (y = 1; y < height; y++, pr += width, pg += width, pb += width) {
            memcpy(pr, red, width);
            memcpy(pg, green, width);
            memcpy(pb, blue, width);
        }

    }

}


void TextureRender::vgradient() {
    float dry, dgy, dby,
        yr = (float) from->red(),
        yg = (float) from->green(),
        yb = (float) from->blue();
    unsigned char *pr = red, *pg = green, *pb = blue;

    register unsigned int y;

    dry = (float) (to->red() - from->red());
    dgy = (float) (to->green() - from->green());
    dby = (float) (to->blue() - from->blue());

    dry /= height;
    dgy /= height;
    dby /= height;

    if (interlaced) {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (y = 0; y < height; y++, pr += width, pg += width, pb += width) {
            if (y & 1) {
                channel = (unsigned char) yr;
                channel2 = (channel >> 1) + (channel >> 2);
                if (channel2 > channel) channel2 = 0;
                memset(pr, channel2, width);

                channel = (unsigned char) yg;
                channel2 = (channel >> 1) + (channel >> 2);
                if (channel2 > channel) channel2 = 0;
                memset(pg, channel2, width);

                channel = (unsigned char) yb;
                channel2 = (channel >> 1) + (channel >> 2);
                if (channel2 > channel) channel2 = 0;
                memset(pb, channel2, width);
            } else {
                channel = (unsigned char) yr;
                channel2 = channel + (channel >> 3);
                if (channel2 < channel) channel2 = ~0;
                memset(pr, channel2, width);

                channel = (unsigned char) yg;
                channel2 = channel + (channel >> 3);
                if (channel2 < channel) channel2 = ~0;
                memset(pg, channel2, width);

                channel = (unsigned char) yb;
                channel2 = channel + (channel >> 3);
                if (channel2 < channel) channel2 = ~0;
                memset(pb, channel2, width);
            }

            yr += dry;
            yg += dgy;
            yb += dby;
        }
    } else {

        // normal vgradient
        for (y = 0; y < height; y++, pr += width, pg += width, pb += width) {
            memset(pr, (unsigned char) yr, width);
            memset(pg, (unsigned char) yg, width);
            memset(pb, (unsigned char) yb, width);

            yr += dry;
            yg += dgy;
            yb += dby;
        }
    }


}


void TextureRender::pgradient() {
    // pyramid gradient -	based on original dgradient, written by
    // Mosfet (mosfet@kde.org)
    // adapted from kde sources for Blackbox by Brad Hughes

    float yr, yg, yb, drx, dgx, dbx, dry, dgy, dby,
        xr, xg, xb;
    int rsign, gsign, bsign;
    unsigned char *pr = red, *pg = green, *pb = blue;
    unsigned int tr = to->red(), tg = to->green(), tb = to->blue();
    unsigned int *xt = xtable, *yt = ytable;

    register unsigned int x, y;

    dry = drx = (float) (to->red() - from->red());
    dgy = dgx = (float) (to->green() - from->green());
    dby = dbx = (float) (to->blue() - from->blue());

    rsign = (drx < 0) ? -1 : 1;
    gsign = (dgx < 0) ? -1 : 1;
    bsign = (dbx < 0) ? -1 : 1;

    xr = yr = (drx / 2);
    xg = yg = (dgx / 2);
    xb = yb = (dbx / 2);

    // Create X table
    drx /= width;
    dgx /= width;
    dbx /= width;

    for (x = 0; x < width; x++) {
        *(xt++) = (unsigned char) ((xr < 0) ? -xr : xr);
        *(xt++) = (unsigned char) ((xg < 0) ? -xg : xg);
        *(xt++) = (unsigned char) ((xb < 0) ? -xb : xb);

        xr -= drx;
        xg -= dgx;
        xb -= dbx;
    }

    // Create Y table
    dry /= height;
    dgy /= height;
    dby /= height;

    for (y = 0; y < height; y++) {
        *(yt++) = ((unsigned char) ((yr < 0) ? -yr : yr));
        *(yt++) = ((unsigned char) ((yg < 0) ? -yg : yg));
        *(yt++) = ((unsigned char) ((yb < 0) ? -yb : yb));

        yr -= dry;
        yg -= dgy;
        yb -= dby;
    }

    // Combine tables to create gradient


    if (! interlaced) {

        // normal pgradient
        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                *(pr++) = (unsigned char) (tr - (rsign * (*(xt++) + *(yt))));
                *(pg++) = (unsigned char) (tg - (gsign * (*(xt++) + *(yt + 1))));
                *(pb++) = (unsigned char) (tb - (bsign * (*(xt++) + *(yt + 2))));
            }
        }

    } else {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                if (y & 1) {
                    channel = (unsigned char) (tr - (rsign * (*(xt++) + *(yt))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pr++) = channel2;

                    channel = (unsigned char) (tg - (gsign * (*(xt++) + *(yt + 1))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pg++) = channel2;

                    channel = (unsigned char) (tb - (bsign * (*(xt++) + *(yt + 2))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pb++) = channel2;
                } else {
                    channel = (unsigned char) (tr - (rsign * (*(xt++) + *(yt))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pr++) = channel2;

                    channel = (unsigned char) (tg - (gsign * (*(xt++) + *(yt + 1))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pg++) = channel2;

                    channel = (unsigned char) (tb - (bsign * (*(xt++) + *(yt + 2))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pb++) = channel2;
                }
            }
        }
    }

}


void TextureRender::rgradient() {
    // rectangle gradient -	based on original dgradient, written by
    // Mosfet (mosfet@kde.org)
    // adapted from kde sources for Blackbox by Brad Hughes

    float drx, dgx, dbx, dry, dgy, dby, xr, xg, xb, yr, yg, yb;
    int rsign, gsign, bsign;
    unsigned char *pr = red, *pg = green, *pb = blue;
    unsigned int tr = to->red(), tg = to->green(), tb = to->blue();
    unsigned int *xt = xtable, *yt = ytable;

    register unsigned int x, y;

    dry = drx = (float) (to->red() - from->red());
    dgy = dgx = (float) (to->green() - from->green());
    dby = dbx = (float) (to->blue() - from->blue());

    rsign = (drx < 0) ? -2 : 2;
    gsign = (dgx < 0) ? -2 : 2;
    bsign = (dbx < 0) ? -2 : 2;

    xr = yr = (drx / 2);
    xg = yg = (dgx / 2);
    xb = yb = (dbx / 2);

    // Create X table
    drx /= width;
    dgx /= width;
    dbx /= width;

    for (x = 0; x < width; x++) {
        *(xt++) = (unsigned char) ((xr < 0) ? -xr : xr);
        *(xt++) = (unsigned char) ((xg < 0) ? -xg : xg);
        *(xt++) = (unsigned char) ((xb < 0) ? -xb : xb);

        xr -= drx;
        xg -= dgx;
        xb -= dbx;
    }

    // Create Y table
    dry /= height;
    dgy /= height;
    dby /= height;

    for (y = 0; y < height; y++) {
        *(yt++) = ((unsigned char) ((yr < 0) ? -yr : yr));
        *(yt++) = ((unsigned char) ((yg < 0) ? -yg : yg));
        *(yt++) = ((unsigned char) ((yb < 0) ? -yb : yb));

        yr -= dry;
        yg -= dgy;
        yb -= dby;
    }

    // Combine tables to create gradient


    if (! interlaced) {

        // normal rgradient
        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                *(pr++) = (unsigned char) (tr - (rsign * std::max(*(xt++), *(yt))));
                *(pg++) = (unsigned char) (tg - (gsign * std::max(*(xt++), *(yt + 1))));
                *(pb++) = (unsigned char) (tb - (bsign * std::max(*(xt++), *(yt + 2))));
            }
        }

    } else {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                if (y & 1) {
                    channel = (unsigned char) (tr - (rsign * std::max(*(xt++), *(yt))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pr++) = channel2;

                    channel = (unsigned char) (tg - (gsign * std::max(*(xt++), *(yt + 1))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pg++) = channel2;

                    channel = (unsigned char) (tb - (bsign * std::max(*(xt++), *(yt + 2))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pb++) = channel2;
                } else {
                    channel = (unsigned char) (tr - (rsign * std::max(*(xt++), *(yt))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pr++) = channel2;

                    channel = (unsigned char) (tg - (gsign * std::max(*(xt++), *(yt + 1))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pg++) = channel2;

                    channel = (unsigned char) (tb - (bsign * std::max(*(xt++), *(yt + 2))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pb++) = channel2;
                }
            }
        }
    }

}


void TextureRender::egradient() {
    // elliptic gradient -	based on original dgradient, written by
    // Mosfet (mosfet@kde.org)
    // adapted from kde sources for Blackbox by Brad Hughes

    float drx, dgx, dbx, dry, dgy, dby, yr, yg, yb, xr, xg, xb;
    int rsign, gsign, bsign;
    unsigned char *pr = red, *pg = green, *pb = blue;
    unsigned int *xt = xtable, *yt = ytable;
    unsigned int tr = (unsigned long) to->red(),
        tg = (unsigned long) to->green(),
        tb = (unsigned long) to->blue();

    register unsigned int x, y;

    dry = drx = (float) (to->red() - from->red());
    dgy = dgx = (float) (to->green() - from->green());
    dby = dbx = (float) (to->blue() - from->blue());

    rsign = (drx < 0) ? -1 : 1;
    gsign = (dgx < 0) ? -1 : 1;
    bsign = (dbx < 0) ? -1 : 1;

    xr = yr = (drx / 2);
    xg = yg = (dgx / 2);
    xb = yb = (dbx / 2);

    // Create X table
    drx /= width;
    dgx /= width;
    dbx /= width;

    for (x = 0; x < width; x++) {
        *(xt++) = (unsigned long) (xr * xr);
        *(xt++) = (unsigned long) (xg * xg);
        *(xt++) = (unsigned long) (xb * xb);

        xr -= drx;
        xg -= dgx;
        xb -= dbx;
    }

    // Create Y table
    dry /= height;
    dgy /= height;
    dby /= height;

    for (y = 0; y < height; y++) {
        *(yt++) = (unsigned long) (yr * yr);
        *(yt++) = (unsigned long) (yg * yg);
        *(yt++) = (unsigned long) (yb * yb);

        yr -= dry;
        yg -= dgy;
        yb -= dby;
    }

    // Combine tables to create gradient
    if (! interlaced) {
        // normal egradient
        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                *(pr++) = (unsigned char)
                    (tr - (rsign * control.getSqrt(*(xt++) + *(yt))));
                *(pg++) = (unsigned char)
                    (tg - (gsign * control.getSqrt(*(xt++) + *(yt + 1))));
                *(pb++) = (unsigned char)
                    (tb - (bsign * control.getSqrt(*(xt++) + *(yt + 2))));
            }
        }

    } else {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                if (y & 1) {
                    channel = (unsigned char)
                        (tr - (rsign * control.getSqrt(*(xt++) + *(yt))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pr++) = channel2;

                    channel = (unsigned char)
                        (tg - (gsign * control.getSqrt(*(xt++) + *(yt + 1))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pg++) = channel2;

                    channel = (unsigned char)
                        (tb - (bsign * control.getSqrt(*(xt++) + *(yt + 2))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pb++) = channel2;
                } else {
                    channel = (unsigned char)
                        (tr - (rsign * control.getSqrt(*(xt++) + *(yt))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pr++) = channel2;

                    channel = (unsigned char)
                        (tg - (gsign * control.getSqrt(*(xt++) + *(yt + 1))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pg++) = channel2;

                    channel = (unsigned char)
                        (tb - (bsign * control.getSqrt(*(xt++) + *(yt + 2))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pb++) = channel2;
                }
            }
        }
    }

}


void TextureRender::pcgradient() {
    // pipe cross gradient -	based on original dgradient, written by
    // Mosfet (mosfet@kde.org)
    // adapted from kde sources for Blackbox by Brad Hughes

    float drx, dgx, dbx, dry, dgy, dby, xr, xg, xb, yr, yg, yb;
    int rsign, gsign, bsign;
    unsigned char *pr = red, *pg = green, *pb = blue;
    unsigned int *xt = xtable, *yt = ytable;
    unsigned int tr = to->red(),
        tg = to->green(),
        tb = to->blue();

    register unsigned int x, y;

    dry = drx = (float) (to->red() - from->red());
    dgy = dgx = (float) (to->green() - from->green());
    dby = dbx = (float) (to->blue() - from->blue());

    rsign = (drx < 0) ? -2 : 2;
    gsign = (dgx < 0) ? -2 : 2;
    bsign = (dbx < 0) ? -2 : 2;

    xr = yr = (drx / 2);
    xg = yg = (dgx / 2);
    xb = yb = (dbx / 2);

    // Create X table
    drx /= width;
    dgx /= width;
    dbx /= width;

    for (x = 0; x < width; x++) {
        *(xt++) = (unsigned char) ((xr < 0) ? -xr : xr);
        *(xt++) = (unsigned char) ((xg < 0) ? -xg : xg);
        *(xt++) = (unsigned char) ((xb < 0) ? -xb : xb);

        xr -= drx;
        xg -= dgx;
        xb -= dbx;
    }

    // Create Y table
    dry /= height;
    dgy /= height;
    dby /= height;

    for (y = 0; y < height; y++) {
        *(yt++) = ((unsigned char) ((yr < 0) ? -yr : yr));
        *(yt++) = ((unsigned char) ((yg < 0) ? -yg : yg));
        *(yt++) = ((unsigned char) ((yb < 0) ? -yb : yb));

        yr -= dry;
        yg -= dgy;
        yb -= dby;
    }

    // Combine tables to create gradient
    if (! interlaced) {

        // normal pcgradient
        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                *(pr++) = (unsigned char) (tr - (rsign * std::min(*(xt++), *(yt))));
                *(pg++) = (unsigned char) (tg - (gsign * std::min(*(xt++), *(yt + 1))));
                *(pb++) = (unsigned char) (tb - (bsign * std::min(*(xt++), *(yt + 2))));
            }
        }

    } else {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                if (y & 1) {
                    channel = (unsigned char) (tr - (rsign * std::min(*(xt++), *(yt))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pr++) = channel2;

                    channel = (unsigned char) (tg - (bsign * std::min(*(xt++), *(yt + 1))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pg++) = channel2;

                    channel = (unsigned char) (tb - (gsign * std::min(*(xt++), *(yt + 2))));
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pb++) = channel2;
                } else {
                    channel = (unsigned char) (tr - (rsign * std::min(*(xt++), *(yt))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pr++) = channel2;

                    channel = (unsigned char) (tg - (gsign * std::min(*(xt++), *(yt + 1))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pg++) = channel2;

                    channel = (unsigned char) (tb - (bsign * std::min(*(xt++), *(yt + 2))));
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pb++) = channel2;
                }
            }
        }
    }

}


void TextureRender::cdgradient() {
    // cross diagonal gradient -	based on original dgradient, written by
    // Mosfet (mosfet@kde.org)
    // adapted from kde sources for Blackbox by Brad Hughes

    float drx, dgx, dbx, dry, dgy, dby, yr = 0.0, yg = 0.0, yb = 0.0,
        xr = (float) from->red(),
        xg = (float) from->green(),
        xb = (float) from->blue();
    unsigned char *pr = red, *pg = green, *pb = blue;
    unsigned int w = width * 2, h = height * 2, *xt, *yt;

    register unsigned int x, y;

    dry = drx = (float) (to->red() - from->red());
    dgy = dgx = (float) (to->green() - from->green());
    dby = dbx = (float) (to->blue() - from->blue());

    // Create X table
    drx /= w;
    dgx /= w;
    dbx /= w;

    for (xt = (xtable + (width * 3) - 1), x = 0; x < width; x++) {
        *(xt--) = (unsigned char) xb;
        *(xt--) = (unsigned char) xg;
        *(xt--) = (unsigned char) xr;

        xr += drx;
        xg += dgx;
        xb += dbx;
    }

    // Create Y table
    dry /= h;
    dgy /= h;
    dby /= h;

    for (yt = ytable, y = 0; y < height; y++) {
        *(yt++) = (unsigned char) yr;
        *(yt++) = (unsigned char) yg;
        *(yt++) = (unsigned char) yb;

        yr += dry;
        yg += dgy;
        yb += dby;
    }

    // Combine tables to create gradient

    if (! interlaced) {
        // normal cdgradient
        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                *(pr++) = *(xt++) + *(yt);
                *(pg++) = *(xt++) + *(yt + 1);
                *(pb++) = *(xt++) + *(yt + 2);
            }
        }

    } else {
        // faked interlacing effect
        unsigned char channel, channel2;

        for (yt = ytable, y = 0; y < height; y++, yt += 3) {
            for (xt = xtable, x = 0; x < width; x++) {
                if (y & 1) {
                    channel = *(xt++) + *(yt);
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pr++) = channel2;

                    channel = *(xt++) + *(yt + 1);
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pg++) = channel2;

                    channel = *(xt++) + *(yt + 2);
                    channel2 = (channel >> 1) + (channel >> 2);
                    if (channel2 > channel) channel2 = 0;
                    *(pb++) = channel2;
                } else {
                    channel = *(xt++) + *(yt);
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pr++) = channel2;

                    channel = *(xt++) + *(yt + 1);
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pg++) = channel2;

                    channel = *(xt++) + *(yt + 2);
                    channel2 = channel + (channel >> 3);
                    if (channel2 < channel) channel2 = ~0;
                    *(pb++) = channel2;
                }
            }
        }
    }

}

}; // end namespace FbTk
