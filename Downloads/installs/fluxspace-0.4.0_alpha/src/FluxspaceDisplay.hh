//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// FluxspaceDisplay.hh
//
//  Fluxspace window/display support classes
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

#ifndef __FLUXSPACEDISPLAY_HH
#define __FLUXSPACEDISPLAY_HH

#include <string>
#include <list>
#include <vector>
#include <memory>
#include "BaseDisplay.hh"
#include "FluxspaceTheme.hh"
#include "FluxspaceRenderer.hh"
#include "OptionalData.hh"
#include <FbTk/Timer.hh>
#include <FbTk/Resource.hh>
#include <FbTk/FbWindow.hh>

class PythonInterface;
class FluxspaceInterface;
class FluxspaceDisplay;

//////////////////////////////////////////////////////////////////////
class DisplayItem
//
// An item in the display list.
// Represents a single rendering command.
//////////////////////////////////////////////////////////////////////
{
  public:
    DisplayItem(int x_, int y_)
      : x(x_), y(y_) {}
    virtual void display(FluxspaceRenderer& renderer,
                         FbTk::FbWindow&    w,
                         unsigned int       offsetY) = 0;
    int x, y;
};

//////////////////////////////////////////////////////////////////////
class TextDisplayItem : public DisplayItem
//////////////////////////////////////////////////////////////////////
{
  public:
    TextDisplayItem(int                  x_,
                    int                  y_,
                    const char*          text_,
                    int                  fontSize_,
                    const OptionalColor& color_);
    virtual ~TextDisplayItem();
    virtual void display(FluxspaceRenderer& renderer,
                         FbTk::FbWindow&    w,
                         unsigned int       offsetY);

    string        text;
    int           fontSize;
    OptionalColor color;
};

//////////////////////////////////////////////////////////////////////
class DisplayList : public std::list<DisplayItem*>
// Encapsulates container used for display lists to allow for
// future flexibility to change container type.
//////////////////////////////////////////////////////////////////////
{
  public:
    virtual ~DisplayList();
    virtual TextDisplayItem* addTextItem(int                  x,
                                         int                  y,
                                         const char*          text,
                                         int                  fontSize,
                                         const OptionalColor& color);
    virtual void display(FluxspaceRenderer& renderer,
                         FbTk::FbWindow&    w,
                         unsigned int       offsetY);
    virtual void clearList();
};

//////////////////////////////////////////////////////////////////////
class Row
// Currently spans the entire window
// Each row manages its own display list
//////////////////////////////////////////////////////////////////////
{
  public:
    Row(unsigned int         height_,
        int                  defaultFontSize_,
        const OptionalColor& defaultColor_,
        const char*          buttonLabel_);
    virtual ~Row();

    virtual void addText(int                  x,
                         int                  y,
                         const char*          text,
                         int                  fontSize,
                         const OptionalColor& color);
    virtual void display(FluxspaceRenderer& renderer,
                         FbTk::FbWindow&    w,
                         unsigned int       offsetY);
    virtual void clear(FluxspaceRenderer& renderer,
                       FbTk::FbWindow&    w,
                       unsigned int       offsetY);
    virtual void clearList();

    unsigned int  height;
    int           defaultFontSize;
    OptionalColor defaultColor;
    std::string   buttonLabel;
    DisplayList   displayList;
};

//////////////////////////////////////////////////////////////////////
class RowList : public std::vector<Row*>
// Encapsulates container used for row lists to allow for
// future flexibility to change container type.
//////////////////////////////////////////////////////////////////////
{
  public:
    virtual ~RowList();
    Row* addRow(unsigned int         height,
                int                  defaultFontSize,
                const OptionalColor& defaultColor,
                const char*          buttonLabel,
                int&                 nRow);
};

//////////////////////////////////////////////////////////////////////
class FluxspaceCommand
// Supports copy semantics so that getCommands() can return a safe
// command list.
//////////////////////////////////////////////////////////////////////
{
  public:
    FluxspaceCommand(int rowFrom_, int rowTo_, const char* label_, const char* data_);
    FluxspaceCommand(const FluxspaceCommand& src);
    const FluxspaceCommand& operator=(const FluxspaceCommand& src);

    int rowFrom, rowTo;
    std::string label;
    std::string data;
};

//////////////////////////////////////////////////////////////////////
class FluxspaceCommandList : public std::vector<FluxspaceCommand*>
//////////////////////////////////////////////////////////////////////
{
  public:
    virtual ~FluxspaceCommandList();
    FluxspaceCommand* addCommand(int         rowFrom,
                                 int         rowTo,
                                 const char* label,
                                 const char* data,
                                 int&        cmdId);
    void removeCommand(int cmdId);
};

//////////////////////////////////////////////////////////////////////
class AbstractRowManager
//////////////////////////////////////////////////////////////////////
{
  public:
    virtual RowList&       getRowList() = 0;
    virtual const RowList& getRowList() const = 0;
    virtual Row*           getRow(int nRow, unsigned int* offsetY) = 0;
    virtual const Row*     getRow(int nRow, unsigned int* offsetY) const = 0;
    virtual int            getRowByPosition(int y) const = 0;
    virtual unsigned int   getRowOffsetY(int nRow) const = 0;
    virtual unsigned int   getTotalRowHeight() const = 0;
};

//////////////////////////////////////////////////////////////////////
class FluxspaceDisplay : public BaseDisplay,
                         public AbstractRowManager
//////////////////////////////////////////////////////////////////////
{
  public:

    class Exception
    {
      public:
        Exception(char* description_)
          : description(description)
        {
        }
        char* description;
    };

    FluxspaceDisplay(Display*    display,
                     const char* displayName,
                     bool        verbose_);
    virtual ~FluxspaceDisplay();

    Window getRootWindow()
    {
        return renderer->rootWindow;
    }

    Window getWindow()
    {
        return window->window();
    }

    int addRow(unsigned int height,
               const char*  sUnit,
               const char*  defaultColorName,
               const char*  button);

    int addCommand(int rowfrom, int rowto, const char* label, const char* data);
    void removeCommand(int cmdId);

    enum Justification {LEFTJUSTIFY, RIGHTJUSTIFY, CENTERJUSTIFY};
    void addText(int           nRow,
                 const char*   text,
                 Justification justification,
                 int           offset,
                 int           fontSize,
                 const char*   colorName);
    void clearRow(int nRow);

    void setColorsPerChannel(int colorsPerChannel_) { colorsPerChannel = colorsPerChannel_; }
    void setImageDither(bool imageDither_)          { imageDither = imageDither_; }
    void setAntialias(bool antialias_)              { antialias = antialias_; }
    void setNormalFontName(const char* fontName)    { normalFontName = fontName; }
    void setSmallFontName(const char* fontName)     { smallFontName = fontName; }
    void setLargeFontName(const char* fontName)     { largeFontName = fontName; }
    void setWidth(unsigned int width_)              { width = width_; }
    void setMargin(unsigned int margin_)            { margin = margin_; }
    void setFrame(unsigned int left,
                  unsigned int top,
                  unsigned int right,
                  unsigned int bottom);

    // AbstractRowManager overrides
    virtual RowList&       getRowList() { return rows; }
    virtual const RowList& getRowList() const { return rows; }
    virtual Row*           getRow(int nRow, unsigned int* offsetY);
    virtual const Row*     getRow(int nRow, unsigned int* offsetY) const;
    virtual int            getRowByPosition(int y) const;
    virtual unsigned int   getRowOffsetY(int nRow) const;
    virtual unsigned int   getTotalRowHeight() const;

    // Finalize the configuration and create the main window
    FbTk::FbWindow& createWindow();

    // Start the machinery
    void go(FluxspaceInterface& fluxspaceInterface_);

    // Override BaseDisplay::shutdown()
    virtual void shutdown(int signal);

  protected:
    virtual void process_event(XEvent *);
    virtual void reconfigure();
    virtual Bool handleSignal(int);
    virtual void handlePropertyNotifyEvent(const XPropertyEvent& event);
    virtual void handleClientMessageEvent(const XClientMessageEvent& event);
    virtual void handleKeyPressEvent(const XKeyEvent& event);
    virtual void handleButtonPressEvent(const XButtonEvent& event);
    virtual void handleButtonReleaseEvent(const XButtonEvent& event);
    virtual void handleMotionEvent(const XMotionEvent& event);
    virtual void handleExposeEvent(const XExposeEvent& event);
    virtual void handleEnterWindowEvent(const XCrossingEvent& event);
    virtual void handleLeaveWindowEvent(const XCrossingEvent& event);
    virtual void handleTick();

    enum Orientation {HORIZONTAL, VERTICAL};
    enum Unit        {NORMAL, SMALL, LARGE, PIXEL, PERCENT};
    void decodeUnit(const char* sUnit, Unit& unit, int& fontSize);
    int  convertMeasurement(int          value,
                            Unit         unit,
                            Orientation  orientation,
                            unsigned int offset,
                            unsigned int availableDim);
    void recalculateHeight();
    bool buttonActive() const { return startButtonRow >= 0 && endButtonRow >= startButtonRow; }
    bool buttonOnRow(int nRow) const { return nRow >= startButtonRow && nRow <= endButtonRow; }
    void getCommands(int nRow, FluxspaceCommandList& cmdList, int& rowFrom, int& rowTo);
    void displayRow(int nRow, Row* row, unsigned int offsetY);
    bool getCardinalProperty(Window w, const Atom& atom, long length, long* values);

  private:
    bool                 initialized;
    Atom                 wmDeleteWindow;
    bool                 verbose;
    FluxspaceInterface*  fluxspaceInterface;
    FluxspaceRenderer*   renderer;
    FbTk::FbWindow*      window;
    RowList              rows;
    FluxspaceCommandList commands;
    FbTk::Timer          timer;
    unsigned int         width, height, margin;
    int                  colorsPerChannel;
    bool                 imageDither;
    bool                 antialias;
    std::string          normalFontName, smallFontName, largeFontName;
    FluxspaceFrame       frame;
    int                  startButtonRow, endButtonRow;
    FluxspaceCommandList buttonCommands;
    FluxspaceRenderer::ButtonState buttonState;

    void makeWindow();
    void displayButtonForPosition(int y, FluxspaceRenderer::ButtonState newButtonState);
    void removeButton();
};

#endif // __FLUXSPACEDISPLAY_HH

// vim: ts=4: et: sw=4: softtabstop=0
