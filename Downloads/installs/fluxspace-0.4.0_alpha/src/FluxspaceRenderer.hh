//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// FluxspaceDisplay.hh
//
//  Fluxspace rendering support class
//
//  Fluxspace is a workspace and slit manager for fluxbox
//
//  Copyright (c) 2002-2003 Steve Cooper, stevencooper@isomedia.com
//
//  Copyright (c) 1998-2000 by John Kennis, jkennis@chello.nl
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

#ifndef __FLUXSPACERENDERER_HH
#define __FLUXSPACERENDERER_HH

#include <string>
#include <memory>
#include <vector>
#include "FluxspaceTheme.hh"
#include <FbTk/Color.hh>
#include <FbTk/Font.hh>
#include <FbTk/FbPixmap.hh>
#include <FbTk/FbWindow.hh>
#include <FbTk/ImageControl.hh>

class BitMask;

//////////////////////////////////////////////////////////////////////
class FluxspaceFrame
//////////////////////////////////////////////////////////////////////
{
  public:
    FluxspaceFrame(unsigned int l, unsigned int t, unsigned int r, unsigned int b)
      : left(l), top(t), right(r), bottom(b) {}

    FluxspaceFrame(const FluxspaceFrame& src)
      : left(src.left), top(src.top), right(src.right), bottom(src.bottom) {}

    const FluxspaceFrame& operator=(const FluxspaceFrame& src)
    {
        left   = src.left;
        top    = src.top;
        right  = src.right;
        bottom = src.bottom;
    }

    void update(unsigned int l, unsigned int t, unsigned int r, unsigned int b)
    {
        left   = l;
        top    = t;
        right  = r;
        bottom = b;
    }

    unsigned int left, top, right, bottom;
};

//////////////////////////////////////////////////////////////////////
class FluxspaceButtonPixmap
//////////////////////////////////////////////////////////////////////
{
  public:
    FluxspaceButtonPixmap(unsigned int w, unsigned int h);

    unsigned int width, height;
    Pixmap pixmapUp;
    Pixmap pixmapDown;
};

//////////////////////////////////////////////////////////////////////
class FluxspaceButtonPixmapList : public std::vector<FluxspaceButtonPixmap*>
//////////////////////////////////////////////////////////////////////
{
  public:
    virtual ~FluxspaceButtonPixmapList();
};

//////////////////////////////////////////////////////////////////////
class FluxspaceRenderer
//////////////////////////////////////////////////////////////////////
{
  public:
    FluxspaceRenderer(int                   screen_,
                      Display*              display_,
                      Window                rootWindow_,
                      unsigned int          width_,
                      unsigned int          height_,
                      unsigned int          margin_,
                      const FluxspaceFrame& frame_,
                      int                   colorsPerChannel_,
                      bool                  imageDither_,
                      bool                  antialias_,
                      const char*           normalFontName_,
                      const char*           smallFontName_,
                      const char*           largeFontName_);
    virtual ~FluxspaceRenderer();

    void reconfigure(FbTk::FbWindow& window);

    FbTk::ImageControl& imageControl()
        { return *ptrImageControl.get(); }

    unsigned int bevelWidth() const
        { return 1; }   // Hard-coded for now

    void drawText(FbTk::FbWindow&    window,
                  const std::string& text,
                  int                x,
                  int                y,
                  int                fontSize) const;
    void drawTextColor(FbTk::FbWindow&    window,
                       const std::string& text,
                       int                x,
                       int                y,
                       int                fontSize,
                       unsigned long      color) const;
    void clearArea(FbTk::FbWindow& window,
                   int x, int y,
                   unsigned int width, unsigned int height) const;

    void drawHighlight(FbTk::FbWindow& window,
                       int             x,
                       int             y,
                       unsigned int    width,
                       unsigned int    height) const;

    enum ButtonState {ButtonUp, ButtonDown};
    void drawButton(FbTk::FbWindow& window,
                    int                x,
                    int                y,
                    unsigned int       width,
                    unsigned int       height,
                    const std::string& label,
                    int                fontSize,
                    ButtonState        state) const;

    unsigned int width() const;
    unsigned int height() const;
    unsigned int fullWidth() const { return outerWidth; }
    unsigned int fullHeight() const { return outerHeight; }
    unsigned int textHeight(int fontSize) const;
    unsigned int textWidth(const char* text, size_t length, int fontSize) const;
    unsigned int numWidth(int fontSize) const;

    const FbTk::Font& getFont(int fontSize) const;

    void setInnerWidth(unsigned int w);
    void setInnerHeight(unsigned int h);
    void fixXY(int x, int y, int& fixx, int& fixy) const;
    void fixHW(unsigned int w, unsigned int h, unsigned int& fixw, unsigned int& fixh) const;
    void updateFrame(Window window);

    int            screen;
    Display*       display;
    Window         rootWindow;
    FluxspaceTheme theme;
    unsigned int   textHeightNormal;
    unsigned int   textHeightSmall;
    unsigned int   textHeightLarge;
    unsigned int   numWidthNormal;
    unsigned int   numWidthSmall;
    unsigned int   numWidthLarge;
    unsigned int   margin;
    FluxspaceFrame frame;
    int            colorsPerChannel;
    bool           imageDither;
    bool           antialias;

  protected:
    Pixmap getButtonPixmap(unsigned int w, unsigned int h, ButtonState state) const;

  private:
    unsigned int   outerWidth;
    unsigned int   outerHeight;
    unsigned int   cacheLife;
    unsigned int   cacheMax;
    BitMask*       frameMask;
    Pixmap         framePixmap;
    FbTk::FbPixmap frameFbPixmap;
    FbTk::Font     normalFont, smallFont, largeFont;
    FbTk::Color    highlightColor;
    bool           normalFontLoaded, smallFontLoaded, largeFontLoaded;

    std::auto_ptr<FbTk::ImageControl> ptrImageControl;
    FluxspaceButtonPixmapList         buttonPixmaps;
};

#endif // __FLUXSPACERENDERER_HH

// vim: ts=4: et: sw=4: softtabstop=0
