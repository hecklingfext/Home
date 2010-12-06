//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// FluxspaceDisplay.cc
//
//  Fluxspace window/display support classes
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
#include "FluxspaceDisplay.hh"
#include "FluxspaceInterface.hh"
#include "../version.h"
#include <stdio.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <FbTk/RefCount.hh>
#include <FbTk/SimpleCommand.hh>
#include <FbTk/Texture.hh>

//////////////////////////////////////////////////////////////////////
// X error handler for any and all runtime X errors
static int handleXErrors(Display *d, XErrorEvent *e)
//////////////////////////////////////////////////////////////////////
{
  #ifdef DEBUG  
    char errtxt[128];
    XGetErrorText(d, e->error_code, errtxt, 128);
    fprintf(stderr,
            "fluxspace:  [ X Error event received. ]\n"
            "  X Error of failed request:  %d %s\n"
            "  Major/minor opcode of failed request:  %d / %d\n"
            "  Resource id in failed request:  0x%lx\n", e->error_code, errtxt,
            e->request_code, e->minor_code, e->resourceid);
  #endif
    return(False);
}

//////////////////////////////////////////////////////////////////////
class RowListIteratorConst
//////////////////////////////////////////////////////////////////////
{
  public:
    RowListIteratorConst(const AbstractRowManager& rowMgr_, int startRow = 0)
      : rowMgr(rowMgr_),
        rowList(rowMgr_.getRowList()),
        nRow(-1),
        offsetY(0)
    {
        // Move to the start row, if specified.
        while (nRow < startRow - 1 && next())
            ;
    }

    bool next()
    {
        if (nRow < int(rowList.size()))
        {
            // Initialize or update the offset
            if (nRow >= 0)
                offsetY += rowList[nRow]->height;
            nRow++;
        }
        return (nRow < rowList.size());
    }

    const Row* getRow() const
    {
        return (nRow >= 0 && nRow < rowList.size() ? rowList[nRow] : NULL);
    }

    unsigned int getRowNumber() const
    {
        return nRow;
    }

    int getOffsetY() const
    {
        return offsetY;
    }

  protected:
    const AbstractRowManager& rowMgr;
    const RowList&            rowList;
    int                       nRow;
    int                       offsetY;
};

//////////////////////////////////////////////////////////////////////
class RowListIterator
// TODO: Hackey way to implement non-const iterator based on const
// iterator.  But at least the ugly const casting is localized to this
// class.  Should probably use a template instead.
//////////////////////////////////////////////////////////////////////
{
  public:
    RowListIterator(AbstractRowManager& rowMgr_, int startRow = 0)
    {
        constIter = new RowListIteratorConst(rowMgr_, startRow);
    }

    virtual ~RowListIterator()
    {
        delete constIter;
    }

    bool next()
    {
        return constIter->next();
    }

    Row* getRow()
    {
        Row* retRow = const_cast<Row*>(constIter->getRow());   //TODO: :-P
        return retRow;
    }

    unsigned int getRowNumber() const
    {
        return constIter->getRowNumber();
    }

    int getOffsetY() const
    {
        return constIter->getOffsetY();
    }

  private:
    RowListIteratorConst* constIter;
};

//////////////////////////////////////////////////////////////////////
// FluxspaceDisplay
//////////////////////////////////////////////////////////////////////

FluxspaceDisplay::FluxspaceDisplay(Display* display, const char* displayName, bool verbose_)
  : BaseDisplay(display, "Fluxspace", displayName, verbose_),
    initialized(false),
    verbose(verbose_),
    fluxspaceInterface(NULL),
    renderer(NULL),
    window(NULL),
    width(64),
    height(1),
    margin(1),
    colorsPerChannel(4),
    imageDither(false),
    antialias(false),
    frame(4, 4, 4, 4),
    startButtonRow(-1),
    endButtonRow(-1),
    buttonState(FluxspaceRenderer::ButtonUp)
{
}

FluxspaceDisplay::~FluxspaceDisplay()
{
    delete window;
    delete renderer;
}

void FluxspaceDisplay::setFrame(
    unsigned int left,
    unsigned int top,
    unsigned int right,
    unsigned int bottom)
{
    frame.update(left, top, right, bottom);
}

FbTk::FbWindow& FluxspaceDisplay::createWindow()
{
    Display*    display = getXDisplay();
    ScreenInfo* screen  = getScreenInfo(DefaultScreen(display));
    renderer = new FluxspaceRenderer(screen->getScreenNumber(), display, screen->getRootWindow(),
                                     width, height, margin, frame,
                                     colorsPerChannel, imageDither, antialias,
                                     normalFontName.c_str(),
                                     smallFontName.c_str(),
                                     largeFontName.c_str());

    XSetErrorHandler((XErrorHandler)handleXErrors);
    wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);

    makeWindow();

    return *window;
}

void FluxspaceDisplay::go(FluxspaceInterface& fluxspaceInterface_)
{
    timer.setTimeout(1000);
    FbTk::RefCount<FbTk::Command> timerCmd(
            new FbTk::SimpleCommand<FluxspaceDisplay>(*this, &FluxspaceDisplay::handleTick));
    timer.setCommand(timerCmd);

    fluxspaceInterface = &fluxspaceInterface_;

    reconfigure();

    eventLoop();
}

void FluxspaceDisplay::shutdown(int signal)
{
    // Display message if had not previously begun a shutdown
    if (!doShutdown())
    {
        if (signal != SIGINT)
            printf("= Shutdown =\n");
        else if (verbose)
            printf("= Shutdown (signal=%d) =\n", signal);
    }
    
    BaseDisplay::shutdown(signal);
}

void FluxspaceDisplay::reconfigure()
{
    renderer->reconfigure(*window);
}

void FluxspaceDisplay::makeWindow()
{
    window = new FbTk::FbWindow(renderer->screen, 0, 0,
                                renderer->fullWidth(),
                                renderer->fullHeight(),
                                  KeyPressMask
                                | ButtonPressMask
                                | ButtonReleaseMask
                                | ButtonMotionMask
                                | PointerMotionMask
                                | ExposureMask
                                | EnterWindowMask
                                | LeaveWindowMask);

    XWMHints wmhints;
    memset(&wmhints, 0, sizeof wmhints);
    wmhints.initial_state = WithdrawnState;
    wmhints.flags = StateHint | InputHint;
    wmhints.input = False;
 
    XClassHint classhints;
    classhints.res_name=FLUXSPACE;
    classhints.res_class="fluxspace";
          
    char* name = FLUXSPACE;
    XTextProperty wname;
    XStringListToTextProperty(&name, 1, &wname);
    XSetWMProperties(renderer->display,
                     window->window(),
                     &wname,
                     NULL,
                     NULL,
                     0,
                     NULL,
                     &wmhints,
                     &classhints);

    Atom wmproto[2];
    wmproto[0] = wmDeleteWindow;
    wmproto[1] = getBlackboxStructureMessagesAtom();
    XSetWMProtocols(renderer->display, window->window(), wmproto, 2);

    // Create the frame
    renderer->updateFrame(window->window());

    window->show();
}

void FluxspaceDisplay::process_event(XEvent* event)
{
    // Start the timer?  Should happen only once.
    if (!timer.isTiming())
        timer.start();

    switch(event->type)
    {
      case CreateNotify:
        fluxspaceInterface->onWindowCreate(event->xcreatewindow.window, (long unsigned int)-1);
        break;
      case DestroyNotify:
        fluxspaceInterface->onWindowDestroy(event->xdestroywindow.window);
        break;
      case MapNotify:
        fluxspaceInterface->onWindowMap(event->xmap.window);
        break;
      case UnmapNotify:
        fluxspaceInterface->onWindowUnmap(event->xunmap.window);
        break;
      case ReparentNotify:
        fluxspaceInterface->onWindowChangeParent(event->xreparent.window, event->xreparent.parent);
        break;
      case ConfigureNotify:
        fluxspaceInterface->onWindowConfigure(event->xconfigure.window);
        break;
      case PropertyNotify:
        handlePropertyNotifyEvent(event->xproperty);
        break;
      case ClientMessage:
        if ((unsigned)event->xclient.data.l[0] == wmDeleteWindow)
            shutdown(0);
        handleClientMessageEvent(event->xclient);
        break;
      case KeyPress:
        handleKeyPressEvent(event->xkey);
        break;
      case ButtonPress:
        handleButtonPressEvent(event->xbutton);
        break;
      case ButtonRelease:
        handleButtonReleaseEvent(event->xbutton);
        break;
      case MotionNotify:
        handleMotionEvent(event->xmotion);
        break;
      case Expose:
        handleExposeEvent(event->xexpose);
        break;
      case EnterNotify:
        handleEnterWindowEvent(event->xcrossing);
        break;
      case LeaveNotify:
        handleLeaveWindowEvent(event->xcrossing);
        break;
    }
}

void FluxspaceDisplay::handlePropertyNotifyEvent(const XPropertyEvent& event)
{
    if (event.state != PropertyNewValue)
        return;
    if (event.atom == getNETCurrentDesktopAtom())
    {
        long value;
        if (getCardinalProperty(event.window, event.atom, 1, &value))
            fluxspaceInterface->onWorkspaceChangeCurrent(value);
    }
}

bool FluxspaceDisplay::getCardinalProperty(Window w, const Atom& atom, long length, long* values)
{
    Atom actualType;
    int actualFormat;
    unsigned long actualLength, remaining;
    unsigned char *data;
    XGetWindowProperty(getXDisplay(), w, atom, 0L, length, False, XA_CARDINAL,
                       &actualType, &actualFormat, &actualLength, &remaining, &data);
    if (data)
    {
        if (values != NULL)
        {
            long* pData = reinterpret_cast<long*>(data);
            for (unsigned long i = 0; i < static_cast<unsigned long>(length); i++)
            {
                if (i < actualLength)
                    values[i] = pData[i];
                else
                    values[i] = 0;
            }
        }
        XFree(data);
        return true;
    }
    else
        return false;
}

void FluxspaceDisplay::handleKeyPressEvent(const XKeyEvent& event)
{
}

void FluxspaceDisplay::handleClientMessageEvent(const XClientMessageEvent& event)
{
    if (event.message_type != getBlackboxStructureMessagesAtom())
        return;

    if (!initialized)
    {
        fluxspaceInterface->onStartup();
        initialized = true;
    }

    unsigned eventAtom = event.data.l[0];
    if (eventAtom == getBlackboxNotifyWindowRaiseAtom())
    {
        fluxspaceInterface->onWindowRaise(event.data.l[1]);
    }
    else if (eventAtom == getBlackboxNotifyWindowLowerAtom())
    {
        fluxspaceInterface->onWindowLower(event.data.l[1]);
    }
    else if (eventAtom == getBlackboxNotifyWindowAddAtom())
    {
        fluxspaceInterface->onWindowAdd(event.data.l[1],event.data.l[2]);
    }
    else if (eventAtom == getBlackboxNotifyWindowDelAtom())
    {
        fluxspaceInterface->onWindowDelete(event.data.l[1]);
    }
    else if (eventAtom == getBlackboxAttributesAtom())
    {
        fluxspaceInterface->onWindowChangeAttributes(event.data.l[1]);
    }
    else if (eventAtom == getBlackboxNotifyWindowFocusAtom())
    {
        fluxspaceInterface->onWindowFocus(event.data.l[1]);
    }
    if (eventAtom == getBlackboxNotifyCurrentWorkspaceAtom())
    {
        fluxspaceInterface->onWorkspaceChangeCurrent(event.data.l[1]);
    }
    else if (eventAtom == getBlackboxNotifyWorkspaceCountAtom())
    {
        fluxspaceInterface->onWorkspaceChangeCount(event.data.l[1]);
    }
}

void FluxspaceDisplay::handleButtonPressEvent(const XButtonEvent& event)
{
    // Make sure the active button is in the correct place, given the new
    // pointer location.  Display it as pressed.
    // Note that button press and release are the only events that affect
    // the button state.
    if (buttonActive())
        displayButtonForPosition(event.y, FluxspaceRenderer::ButtonDown);
}

void FluxspaceDisplay::handleButtonReleaseEvent(const XButtonEvent& event)
{
    // First change an exist button to show it released.
    // Note that button press and release are the only events that affect
    // the button state.
    if (buttonActive())
        displayButtonForPosition(event.y, FluxspaceRenderer::ButtonUp);
    // If we still have a button after checking the position invoke
    // the ButtonPress handler.
    if (buttonActive())
    {
        //TODO: Handle menu (command list length > 1)
        int nRow = getRowByPosition(event.y);
        if (buttonCommands.size() == 1)
        {
            FluxspaceCommand* command = buttonCommands[0];
            if (command != NULL)
                fluxspaceInterface->onCommand(nRow, command->label, event.button, command->data);
        }
    }
}

void FluxspaceDisplay::handleMotionEvent(const XMotionEvent& event)
{
    // The button may move, change and disappear as the pointer passes over
    // rows with different sets of assigned commands.
    // The current button state shouldn't change.
    displayButtonForPosition(event.y, buttonState);
}

void FluxspaceDisplay::handleExposeEvent(const XExposeEvent& event)
{
    RowListIterator iter(*this);
    while (iter.next())
    {
        if (   int(iter.getOffsetY()) >= event.y
            && int(iter.getOffsetY()) < (event.y + event.height))
        {
            displayRow(iter.getRowNumber(), iter.getRow(), iter.getOffsetY());
        }
    }
}

void FluxspaceDisplay::handleEnterWindowEvent(const XCrossingEvent& event)
{
    // The current button state shouldn't change.
    displayButtonForPosition(event.y, buttonState);
}

void FluxspaceDisplay::handleLeaveWindowEvent(const XCrossingEvent& event)
{
    removeButton();
    buttonCommands.clear();
}

Bool FluxspaceDisplay::handleSignal(int sig)
{
    switch(sig)
    {
      case SIGHUP:
        break;

      case SIGSEGV:
      case SIGFPE:
      case SIGINT:
      case SIGTERM:
        shutdown(sig);

      default:
        return False;
    }

    return True;
}

void FluxspaceDisplay::handleTick()
{
    fluxspaceInterface->onTick();
}

// Create a row object to serve as a viewport for client displays.
int FluxspaceDisplay::addRow(
    unsigned int rowHeight,
    const char*  sUnit,
    const char*  defaultColorName,
    const char*  buttonLabel)
{
    Unit unit;
    int  fontSize;
    decodeUnit(sUnit, unit, fontSize);
    int height = convertMeasurement(rowHeight,
                                    unit,
                                    VERTICAL,
                                    renderer->margin,
                                    renderer->height());
    if (height <= 0)
        throw InternalError("Bad row height");

    OptionalColor defaultColor;
    if (defaultColorName != NULL && *defaultColorName != '\0')
    {
        FbTk::Color tmpColor(defaultColorName, renderer->screen);
        defaultColor.Set(tmpColor.pixel());
    }

    int nRow;
    Row* row = rows.addRow(height, fontSize, defaultColor, buttonLabel, nRow);

    recalculateHeight();

    window->resize(renderer->fullWidth(), renderer->fullHeight());

    return nRow;
}

int FluxspaceDisplay::addCommand(int rowfrom, int rowto, const char* label, const char* data)
{
    int cmdId;
    FluxspaceCommand* command = commands.addCommand(rowfrom, rowto, label, data, cmdId);
    return cmdId;
}

void FluxspaceDisplay::removeCommand(int cmdId)
{
    commands.removeCommand(cmdId);
}

// Create text display item to be processed during next expose event
void FluxspaceDisplay::addText(
    int           nRow,
    const char*   text,
    Justification justification,
    int           offset,
    int           fontSize,
    const char*   colorName)
{
    if (text == NULL)
        return;
    unsigned int offsetY;
    Row* row = getRow(nRow, &offsetY);
    if (row == NULL)
        return;

    if (fontSize == NOFONT)
        fontSize = row->defaultFontSize;

    int x;
    switch(justification)
    {
      case RIGHTJUSTIFY:
        x =   renderer->width()
            - renderer->textWidth(text, strlen(text), fontSize);
        break;
      case CENTERJUSTIFY:
        x =   (renderer->width() / 2)
            - (renderer->textWidth(text, strlen(text), fontSize) / 2)
            + offset;
        break;
      case LEFTJUSTIFY:
      default:
        x = offset;
    }
    if (x < 0)
        x = 0;
    OptionalColor color;
    if (colorName != NULL && *colorName != '\0')
    {
        FbTk::Color tmpColor(colorName, renderer->screen);
        color.Set(tmpColor.pixel());
    }
    row->addText(x, 0, text, fontSize, color);
    displayRow(nRow, row, offsetY);
}

void FluxspaceDisplay::displayRow(int nRow, Row* row, unsigned int offsetY)
{
    if (row != NULL && !buttonOnRow(nRow))
        row->display(*renderer, *window, offsetY);
}

void FluxspaceDisplay::clearRow(int nRow)
{
    unsigned int offsetY;
    Row* row = getRow(nRow, &offsetY);
    if (row != NULL)
    {
        if (buttonOnRow(nRow))
            row->clearList();
        else
            row->clear(*renderer, *window, offsetY);
    }
}

void FluxspaceDisplay::removeButton()
{
    if (buttonActive())
    {
        RowListIterator iter(*this, startButtonRow);
        while (iter.next() && iter.getRowNumber() <= endButtonRow)
            iter.getRow()->display(*renderer, *window, iter.getOffsetY());
    }
    startButtonRow = endButtonRow = -1;
}

void FluxspaceDisplay::displayButtonForPosition(
    int                            y,
    FluxspaceRenderer::ButtonState newButtonState)
{
    // Get the command list and the row range on which to superimpose
    // the action or menu button, depending on whether or not there's
    // more than one command.
    int rowFrom = -1, rowTo = -1;
    int nRow = getRowByPosition(y);
    if (nRow >= 0)
        getCommands(nRow, buttonCommands, rowFrom, rowTo);
    else
        buttonCommands.clear();

    // No button?
    if (buttonCommands.size() == 0)
    {
        removeButton();
        return;
    }
    RowListIterator iter(*this, rowFrom);
    if (!iter.next())
    {
        removeButton();
        return;     // unexpected
    }

    // Same button?
    if (   startButtonRow == rowFrom
        && endButtonRow   == rowTo
        && buttonState    == newButtonState)
        return;

    // Remove the button that the pointer left behind
    removeButton();

    // If it's a menu the label is the row's button label.  All button rows
    // will have the same label, because getCommands() only lets commands
    // span rows that have the same label.
    std::string label;
    if (buttonCommands.size() == 1)
        label = buttonCommands[0]->label;
    else
        label = iter.getRow()->buttonLabel;

    unsigned int offsetY = iter.getOffsetY();
    while (iter.getRowNumber() <= rowTo && iter.next())
        ;

    // Record the button state
    startButtonRow = rowFrom;
    endButtonRow   = rowTo;
    buttonState    = newButtonState;

    // Display the button in the appropriate location and in the correct state.
    renderer->drawButton(*window,
                         0,
                         offsetY,
                         renderer->width(),
                         iter.getOffsetY() - offsetY,
                         label,
                         -1,     //TODO: Font size is hard-coded for now
                         buttonState);
}

Row* FluxspaceDisplay::getRow(int nRow, unsigned int* offsetY)
{
    if (nRow < 0 || nRow >= static_cast<int>(rows.size()))
        throw InternalError("Bad row index");
    if (offsetY != NULL)
        (*offsetY) = getRowOffsetY(nRow);
    return rows[nRow];
}

const Row* FluxspaceDisplay::getRow(int nRow, unsigned int* offsetY) const
{
    Row* row = const_cast<FluxspaceDisplay*>(this)->getRow(nRow, offsetY);
    return row;
}

unsigned int FluxspaceDisplay::getRowOffsetY(int nRow) const
{
    RowListIteratorConst iter(*this);
    while (iter.next() && iter.getRowNumber() != nRow)
        ;
    return iter.getOffsetY();
}

unsigned int FluxspaceDisplay::getTotalRowHeight() const
{
    RowListIteratorConst iter(*this);
    while (iter.next())
        ;
    return iter.getOffsetY();
}

int FluxspaceDisplay::getRowByPosition(int y) const
{
    RowListIteratorConst iter(*this);
    while (iter.next())
    {
        if (y <= iter.getOffsetY() + iter.getRow()->height)
            return iter.getRowNumber();
    }
    return -1;
}

void FluxspaceDisplay::decodeUnit(const char* sUnit, Unit& unit, int& fontSize)
{
    unit = PIXEL;   // Default to pixel unit
    fontSize = 0;   // Default to normal font
    if (strcasecmp(sUnit, "normal") == 0)
        unit = NORMAL;
    else if (strcasecmp(sUnit, "small")  == 0)
    {
        unit = SMALL;
        fontSize = -1;
    }
    else if (strcasecmp(sUnit, "large")  == 0)
    {
        unit = LARGE;
        fontSize = 1;
    }
}

int FluxspaceDisplay::convertMeasurement(
    int          value,
    Unit         unit,
    Orientation  orientation,
    unsigned int offset,
    unsigned int availableDim)
{
    int newValue;
    switch(unit)
    {
      case PERCENT:
        newValue = (value * availableDim) / 100;
        break;
      case NORMAL:
        newValue = value * (orientation == VERTICAL
                                ? renderer->textHeight(0)
                                : renderer->numWidth(0));
        break;
      case SMALL:
        newValue = value * (orientation == VERTICAL
                                ? renderer->textHeight(-1)
                                : renderer->numWidth(-1));
        break;
      case LARGE:
        newValue = value * (orientation == VERTICAL
                                ? renderer->textHeight(1)
                                : renderer->numWidth(1));
        break;
      case PIXEL:
      default:
        newValue = value;
        ;
    }
    if (newValue < 0)
        newValue += availableDim;
    newValue += offset;
    return newValue;
}

void FluxspaceDisplay::recalculateHeight()
{
    renderer->setInnerHeight(getTotalRowHeight());
    renderer->updateFrame(window->window());
    reconfigure();
}

// The returned rowFrom/rowTo values represent the intersection of contiguous
// rows that support identical commands.  This allows some buttons to span
// multiple rows when appropriate.
void FluxspaceDisplay::getCommands(
    int                   row,
    FluxspaceCommandList& cmdList,
    int&                  rowFrom,
    int&                  rowTo)
{
    cmdList.clear();
    rowFrom = rowTo = -1;

    // From the commands that apply to the specified row determine the smallest
    // intersection of rows having identical commands.
    for (FluxspaceCommandList::iterator iter = commands.begin();
         iter != commands.end();
         iter++)
    {
        FluxspaceCommand* command = (*iter);
        if (command != NULL && command->rowFrom <= row && command->rowTo >= row)
        {
            if (rowFrom < 0 || command->rowFrom > rowFrom)
                rowFrom = command->rowFrom;
            if (rowTo < 0 || command->rowTo < rowTo)
                rowTo = command->rowTo;
            cmdList.push_back(new FluxspaceCommand(*command));
        }
    }

    // A single command will generate a single button labeled by the command
    // and spanning the rows determined by the command.
    if (cmdList.size() <= 1)
        return;

    // Multiple commands become a menu, but we now have to limit it to rows
    // having a common button label.
    // Eliminate spanned rows that have different labels from the target row.
    std::string targetLabel = rows[row]->buttonLabel;
    for (int i = row - 1; i >= rowFrom && rowFrom >= 0; i--)
    {
        if (rows[i]->buttonLabel != targetLabel)
        {
            rowFrom = i + 1;
            break;
        }
    }
    for (int i = row + 1; i <= rowTo; i++)
    {
        if (rows[i]->buttonLabel != targetLabel)
        {
            rowFrom = i - 1;
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////
// TextDisplayItem
//////////////////////////////////////////////////////////////////////

TextDisplayItem::TextDisplayItem(
    int                  x_,
    int                  y_,
    const char*          text_,
    int                  fontSize_,
    const OptionalColor& color_)
  : DisplayItem(x_, y_),
    text(text_),
    fontSize(fontSize_),
    color(color_)
{
}

TextDisplayItem::~TextDisplayItem()
{
}

void TextDisplayItem::display(
    FluxspaceRenderer& renderer,
    FbTk::FbWindow&    window,
    unsigned int       offsetY)
{
    if (color.isSet)
        renderer.drawTextColor(window, text.c_str(), x, offsetY+y, fontSize, color.value);
    else
        renderer.drawText(window, text.c_str(), x, offsetY+y, fontSize);
}

//////////////////////////////////////////////////////////////////////
// DisplayList
//////////////////////////////////////////////////////////////////////

DisplayList::~DisplayList()
{
    for (iterator iter = begin(); iter != end(); iter++)
        delete (*iter);
}

TextDisplayItem* DisplayList::addTextItem(
    int                  x,
    int                  y,
    const char*          text,
    int                  fontSize,
    const OptionalColor& color)
{
    TextDisplayItem* item = new TextDisplayItem(x, y, text, fontSize, color);
    push_back(item);
    return item;
}

void DisplayList::display(
    FluxspaceRenderer& renderer,
    FbTk::FbWindow&    window,
    unsigned int       offsetY)
{
    for (iterator iter = begin(); iter != end(); iter++)
    {
        DisplayItem* item = (*iter);
        if (item != NULL)
            item->display(renderer, window, offsetY);
    }
}

void DisplayList::clearList()
{
    clear();
}

//////////////////////////////////////////////////////////////////////
// Row
//////////////////////////////////////////////////////////////////////

Row::Row(
    unsigned int         height_,
    int                  defaultFontSize_,
    const OptionalColor& defaultColor_,
    const char*          buttonLabel_)
  : height(height_),
    defaultFontSize(defaultFontSize_),
    defaultColor(defaultColor_),
    buttonLabel(buttonLabel_ != NULL ? buttonLabel_ : "")
{
}

Row::~Row()
{
}

void Row::addText(
    int                  x,
    int                  y,
    const char*          text,
    int                  fontSize,
    const OptionalColor& color)
{
    int useFontSize = (fontSize != NOFONT ? fontSize : defaultFontSize);
    const OptionalColor& useColor = (color.isSet ? color : defaultColor);
    displayList.addTextItem(x, y, text, useFontSize, useColor);
}

void Row::display(
    FluxspaceRenderer& renderer,
    FbTk::FbWindow&    window,
    unsigned int       offsetY)
{
    renderer.clearArea(window, 0, offsetY, renderer.width(), height);
    displayList.display(renderer, window, offsetY);
}

void Row::clear(
    FluxspaceRenderer& renderer,
    FbTk::FbWindow&    window,
    unsigned int       offsetY)
{
    clearList();
    renderer.clearArea(window, 0, offsetY, renderer.width(), height);
}

void Row::clearList()
{
    displayList.clearList();
}

//////////////////////////////////////////////////////////////////////
// RowList
//////////////////////////////////////////////////////////////////////

RowList::~RowList()
{
    for (iterator iter = begin(); iter != end(); iter++)
        delete (*iter);
}

Row* RowList::addRow(
    unsigned int         height,
    int                  defaultFontSize,
    const OptionalColor& defaultColor,
    const char*          buttonLabel,
    int&                 nRow)
{
    Row* row = new Row(height, defaultFontSize, defaultColor, buttonLabel);
    push_back(row);
    nRow = size() - 1;
    return row;
}

//////////////////////////////////////////////////////////////////////
// class FluxspaceCommand
//////////////////////////////////////////////////////////////////////

FluxspaceCommand::FluxspaceCommand(int rowFrom_, int rowTo_, const char* label_, const char* data_)
  : rowFrom(rowFrom_),
    rowTo(rowTo_),
    label(label_ != NULL ? label_ : ""),
    data(data_ != NULL ? data_ : "")
{
}

FluxspaceCommand::FluxspaceCommand(const FluxspaceCommand& src)
{
    operator=(src);
}

const FluxspaceCommand& FluxspaceCommand::operator=(const FluxspaceCommand& src)
{
    rowFrom = src.rowFrom;
    rowTo   = src.rowTo;
    label   = src.label;
    data    = src.data;
}

//////////////////////////////////////////////////////////////////////
// FluxspaceCommandList
//////////////////////////////////////////////////////////////////////

FluxspaceCommandList::~FluxspaceCommandList()
{
    for (iterator iter = begin(); iter != end(); iter++)
        delete (*iter);
}

FluxspaceCommand* FluxspaceCommandList::addCommand(
    int         rowFrom,
    int         rowTo,
    const char* label,
    const char* data,
    int&        cmdId)
{
    FluxspaceCommand* command = new FluxspaceCommand(rowFrom, rowTo, label, data);
    push_back(command);
    cmdId = size() - 1;
    return command;
}

void FluxspaceCommandList::removeCommand(int cmdId)
{
    // Have to clear out command, rather than removing it, because command id's
    // are indices that rely on command vector positions not changing.
    if (cmdId >= 0 && cmdId < size())
    {
        delete operator[](cmdId);
        operator[](cmdId) = NULL;
    }
}

// vim: ts=4: et: sw=4: softtabstop=0
