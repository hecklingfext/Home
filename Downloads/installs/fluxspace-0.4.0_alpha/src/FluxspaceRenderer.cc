//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// FluxspaceRenderer.cc
//
//  Fluxspace rendering support class
//
//  Fluxspace is a workspace and slit manager for fluxbox
//
//  Copyright (c) 2002 Steve Cooper, stevencooper@isomedia.com
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

#include "fluxspacecore.hh"
#include "FluxspaceRenderer.hh"
#include "BitMask.hh"
#include <X11/extensions/shape.h>
#include "FbTk/Color.hh"

//////////////////////////////////////////////////////////////////////
// FluxspaceRenderer
//////////////////////////////////////////////////////////////////////

FluxspaceRenderer::FluxspaceRenderer(
    int                   screen_,
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
    const char*           largeFontName_)
  : screen(screen_),
    display(display_),
    rootWindow(rootWindow_),
    theme(screen_),
    textHeightNormal(0),
    textHeightSmall(0),
    textHeightLarge(0),
    numWidthNormal(0),
    numWidthSmall(0),
    numWidthLarge(0),
    margin(margin_),
    frame(frame_),
    colorsPerChannel(colorsPerChannel_),
    imageDither(imageDither_),
    antialias(antialias_),
    outerWidth(width_),
    outerHeight(height_),
    cacheLife(5),
    cacheMax(200),
    frameMask(NULL),
    framePixmap(None),
    normalFont(NULL, antialias_),
    smallFont(NULL, antialias_),
    largeFont(NULL, antialias_),
    highlightColor("yellow", screen_),
    normalFontLoaded(false),
    smallFontLoaded(false),
    largeFontLoaded(false)
{
    if (normalFontName_ != NULL && *normalFontName_ != '\0')
        normalFontLoaded = normalFont.load(normalFontName_);
    if (smallFontName_ != NULL && *smallFontName_ != '\0')
        smallFontLoaded = smallFont.load(smallFontName_);
    if (largeFontName_ != NULL && *largeFontName_ != '\0')
        largeFontLoaded = largeFont.load(largeFontName_);
}

FluxspaceRenderer::~FluxspaceRenderer()
{
    delete frameMask;
    if (framePixmap)
        imageControl().removeImage(framePixmap);
    for (FluxspaceButtonPixmapList::iterator iter = buttonPixmaps.begin();
         iter != buttonPixmaps.end();
         iter++)
    {
        if ((*iter) != NULL)
        {
            if ((*iter)->pixmapUp != None)
                imageControl().removeImage((*iter)->pixmapUp);
            if ((*iter)->pixmapDown != None)
                imageControl().removeImage((*iter)->pixmapDown);
        }
    }
}

void FluxspaceRenderer::reconfigure(FbTk::FbWindow& window)
{
    // Set up image cache engine
    FbTk::ImageControl* ic = new FbTk::ImageControl(screen,
                                                    true,
                                                    colorsPerChannel,
                                                    cacheLife * 60000,
                                                    cacheMax);
    ptrImageControl.reset(ic);
    imageControl().installRootColormap();
    imageControl().setDither(imageDither);

    // Cache backround pixmap
    Pixmap prevFramePixmap = framePixmap;
    if (theme.frameTexture().type() == (FbTk::Texture::FLAT | FbTk::Texture::SOLID))
        framePixmap = None;
    else
        framePixmap = imageControl().renderImage(outerWidth, outerHeight, theme.frameTexture());        
    if (prevFramePixmap)
        imageControl().removeImage(prevFramePixmap);

    FbTk::GContext gc(window.window());

    // Create background pixmap object
    XWindowAttributes attr;
    XGetWindowAttributes(display, window.window(), &attr);
    frameFbPixmap = FbTk::FbPixmap(window.window(), outerWidth, outerHeight, attr.depth);
    if (frameFbPixmap.drawable() == 0)
        throw InternalError("Failed to create background pixmap");
    window.setBackgroundPixmap(frameFbPixmap.drawable());

    // Display pixmap
    if (framePixmap != None)
        frameFbPixmap.copyArea(framePixmap, gc.gc(), 0, 0, 0, 0, outerWidth, outerHeight);
    else
    {
        gc.setForeground(theme.frameTexture().color());
        frameFbPixmap.fillRectangle(gc.gc(), 0, 0, outerWidth, outerHeight);
    }

    // Display the bevel
    int left   = frame.left;
    int top    = frame.top;
    int right  = outerWidth  - frame.right - 1;
    int bottom = outerHeight - frame.bottom - 1;
    gc.setForeground(FbTk::Color("lightgrey", screen));
    frameFbPixmap.drawLine(gc.gc(), left,  bottom, right, bottom);
    frameFbPixmap.drawLine(gc.gc(), right, top,    right, bottom);
    gc.setForeground(FbTk::Color("grey20", screen));
    frameFbPixmap.drawLine(gc.gc(), left, top, right, top);
    frameFbPixmap.drawLine(gc.gc(), left, top, left,  bottom);

    theme.setAntialias(antialias);

    theme.reconfigTheme();

    // Calculate text height/width for all loaded fonts
    FbTk::Font& normalFontPick = (normalFontLoaded ? normalFont : theme.frameFont());
    textHeightNormal = normalFontPick.height();
    numWidthNormal   = normalFontPick.textWidth("0", 1);
    textHeightSmall  = (smallFontLoaded ? smallFont.height() : textHeightNormal);
    numWidthSmall    = (smallFontLoaded ? smallFont.textWidth("0", 1) : numWidthNormal);
    textHeightLarge  = (largeFontLoaded ? largeFont.height() : textHeightNormal);
    numWidthLarge    = (largeFontLoaded ? largeFont.textWidth("0", 1) : numWidthNormal);
}

const FbTk::Font& FluxspaceRenderer::getFont(int fontSize) const
{
    if (fontSize < 0 && smallFontLoaded)
        return smallFont;
    if (fontSize > 0 && largeFontLoaded)
        return largeFont;
    if (normalFontLoaded)
        return normalFont;
    return theme.frameFont();
}

unsigned int FluxspaceRenderer::textWidth(const char* text, size_t length, int fontSize) const
{
    return getFont(fontSize).textWidth(text, length);
}

unsigned int FluxspaceRenderer::width() const
{
    return outerWidth - frame.left - frame.right - (bevelWidth() * 2);
}

unsigned int FluxspaceRenderer::height() const
{
    return outerHeight - frame.top - frame.bottom - (bevelWidth() * 2);
}

void FluxspaceRenderer::setInnerWidth(unsigned int w)
{
    outerWidth = w + frame.left + frame.right + (bevelWidth() * 2);
}

void FluxspaceRenderer::setInnerHeight(unsigned int h)
{
    outerHeight = h + frame.top + frame.bottom + (bevelWidth() * 2);
}

void FluxspaceRenderer::fixXY(int x, int y, int& fixx, int& fixy) const
{
    unsigned int bw = bevelWidth();
    fixx = x + frame.left + bw;
    fixy = y + frame.top  + bw;
}

void FluxspaceRenderer::fixHW(
    unsigned int  w,
    unsigned int  h,
    unsigned int& fixw,
    unsigned int& fixh) const
{
    unsigned int bw = bevelWidth();
    fixw = w - frame.left - frame.right  - bw;
    fixh = h - frame.top  - frame.bottom - bw;
}

void FluxspaceRenderer::drawText(
    FbTk::FbWindow&    window,
    const std::string& text,
    int                x,
    int                y,
    int                fontSize) const
{
    y += textHeight(fontSize);
    GC gc = theme.frameTextGC().gc();
    Window w = window.window();
    fixXY(x, y, x, y);
    getFont(fontSize).drawText(w, screen, gc, text.c_str(), text.length(), x, y);
}

void FluxspaceRenderer::drawTextColor(
    FbTk::FbWindow&    window,
    const std::string& text,
    int                x,
    int                y,
    int                fontSize,
    unsigned long      color) const
{
    y += textHeight(fontSize);
    GC gc = theme.frameTextGC().gc();
    Window w = window.window();
    XSetForeground(display, gc, color);
    fixXY(x, y, x, y);
    getFont(fontSize).drawText(w, screen, gc, text.c_str(), text.length(), x, y);
    XSetForeground(display, gc, theme.frameTextColor().pixel());
}

void FluxspaceRenderer::clearArea(
    FbTk::FbWindow& window,
    int             x,
    int             y,
    unsigned int    width,
    unsigned int    height) const
{
    fixXY(x, y, x, y);
    window.clearArea(x, y, width, height);
}

void FluxspaceRenderer::drawHighlight(
    FbTk::FbWindow& window,
    int             x,
    int             y,
    unsigned int    width,
    unsigned int    height) const
{
    fixXY(x, y, x, y);
    FbTk::GContext gc(window.window());
    XSetForeground(display, gc.gc(), highlightColor.pixel());
    window.drawRectangle(gc.gc(), x, y, width, height-1);
}

void FluxspaceRenderer::drawButton(
    FbTk::FbWindow&    window,
    int                xin,
    int                yin,
    unsigned int       width,
    unsigned int       height,
    const std::string& label,
    int                fontSize,
    ButtonState        state) const
{
    int x, y;
    fixXY(xin, yin, x, y);
    //x FluxspaceRenderer* me = const_cast<FluxspaceRenderer*>(this);
    Pixmap pixmap = getButtonPixmap(width, height, state);
    if (pixmap != ParentRelative)   //???
    {
        FbTk::GContext gc(window.window());
        if (pixmap != None)
            //x me->frameFbPixmap.copyArea(pixmap, gc.gc(), 0, 0, x, y, width, height);
            window.copyArea(pixmap, gc.gc(), 0, 0, x, y, width, height);
        else            
            //x me->frameFbPixmap.fillRectangle(gc.gc(), x, y, width, height);
            window.fillRectangle(gc.gc(), x, y, width, height);
        if (label.length() > 0)
        {
            int xtext = (width - textWidth(label.c_str(), label.length(), fontSize)) / 2;
            int ytext = yin + ((height - textHeight(fontSize)) / 2);
            unsigned long color = theme.highlightTextColor().pixel();
            drawTextColor(window, label, xtext, ytext, fontSize, color);
        }
    }
}

unsigned int FluxspaceRenderer::textHeight(int fontSize) const
{
    if (fontSize < 0)
        return textHeightSmall;
    if (fontSize > 0)
        return textHeightLarge;
    return textHeightNormal;
}

unsigned int FluxspaceRenderer::numWidth(int fontSize) const
{
    if (fontSize < 0)
        return numWidthSmall;
    if (fontSize > 0)
        return numWidthLarge;
    return numWidthNormal;
}

void FluxspaceRenderer::updateFrame(Window window)
{
    delete frameMask;
    frameMask = new BitMask(display, window, outerWidth, outerHeight);
    if (   frame.left + frame.right  < outerWidth
        && frame.top  + frame.bottom < outerHeight)
    {
        unsigned int innerWidth  = outerWidth  - frame.left   - frame.right;
        unsigned int innerHeight = outerHeight - frame.bottom - frame.top;
        frameMask->setRectangle(frame.left, frame.top, innerWidth, innerHeight);
    }
    Pixmap pixmask = frameMask->getPixmask();
	XShapeCombineMask(display, window, ShapeBounding, 0, 0, pixmask, ShapeSet);
}

Pixmap FluxspaceRenderer::getButtonPixmap(
    unsigned    int w,
    unsigned    int h,
    ButtonState state) const    // Pretend to be const since it's just a caching mechanism
{
    FluxspaceButtonPixmap* buttonPixmap = NULL;
    FluxspaceRenderer* me = const_cast<FluxspaceRenderer*>(this);

    // Look for existing pixmap with matching dimensions
    for (FluxspaceButtonPixmapList::iterator iter = me->buttonPixmaps.begin();
         iter != me->buttonPixmaps.end() && buttonPixmap == NULL;
         iter++)
    {
        if ((*iter) != NULL && (*iter)->width == w && (*iter)->height == h)
            buttonPixmap = (*iter);
    }

    // If this size button wasn't previously created create it now.
    if (buttonPixmap == NULL)
    {
        buttonPixmap = new FluxspaceButtonPixmap(w, h);
        if (theme.hiliteTexture().type() != (FbTk::Texture::FLAT | FbTk::Texture::SOLID))
            buttonPixmap->pixmapUp = me->imageControl().renderImage(w, h, theme.hiliteTexture());
        if (theme.frameTexture().type() != (FbTk::Texture::FLAT | FbTk::Texture::SOLID))
            buttonPixmap->pixmapDown = me->imageControl().renderImage(w, h, theme.frameTexture());
        me->buttonPixmaps.push_back(buttonPixmap);
    }

    return (state == ButtonDown ? buttonPixmap->pixmapDown : buttonPixmap->pixmapUp);
}

FluxspaceButtonPixmap::FluxspaceButtonPixmap(unsigned int w, unsigned int h)
  : width(w), height(h), pixmapUp(None), pixmapDown(None)
{
}

FluxspaceButtonPixmapList::~FluxspaceButtonPixmapList()
{
    for (iterator iter = begin(); iter != end(); iter++)
        delete (*iter);
}

// vim: ts=4: et: sw=4: softtabstop=0
