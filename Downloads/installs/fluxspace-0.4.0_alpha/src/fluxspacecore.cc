//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// fluxspacecore.cc
//
//  Fluxspace Python support classes (translated by SWIG)
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
#include "PythonInterface.hh"
#include "FluxspaceInterface.hh"
#include "Esetroot.hh"
#include "FluxboxUtility.hh"
#include "OptionalData.hh"
#include <stdio.h>
#include <sstream>
#include <vector>
#include <string>

class ControllerImplementation
{
    // Used internally as singleton for Controller class
    friend class Controller;

  private:
    ControllerImplementation(bool verbose_)
      : verbose(verbose_),
        startedEvents(false),
        display(NULL),
        fluxspace(NULL),
        pythonInterface(NULL),
        esetroot(NULL),
        window(None),
        fluxbox(NULL)
    {
        pythonInterface = new PythonInterface();
    }

    void run(const char* displayName)
    {
        if (startedEvents)
            throw ErrorEventLoopRunning();

        // We may have no registered fluxlets
        if (pythonInterface->GetModuleCount() == 0)
        {
            printf("* No fluxlets are registered - exiting *\n");
            return;
        }

        if (displayName == NULL)
            displayName = getenv("DISPLAY");
        if (displayName == NULL)
            throw ErrorNoDisplay();
        if (verbose)
            printf("display=%s\n", displayName);

        fluxbox = new FluxboxUtility(displayName);

        display = fluxbox->getApp().display();
        if (display == NULL)
            throw ErrorNoDisplay(); //TODO: More specific exception?

        fluxspace = new FluxspaceDisplay(display, displayName, verbose);
        if (width.isSet ) fluxspace->setWidth(width.value);
        if (margin.isSet) fluxspace->setMargin(margin.value);
        if (frameWidth.isSet) fluxspace->setFrame(frameWidth.value,
                                                  frameWidth.value,
                                                  frameWidth.value,
                                                  frameWidth.value);
        fluxspace->setColorsPerChannel(colorsPerChannel.Get(fluxbox->getColorsPerChannel()));
        fluxspace->setImageDither(imageDither.Get(fluxbox->getImageDither()));
        fluxspace->setAntialias(antialias.Get(fluxbox->getAntialias()));
        if (normalFont.isSet) fluxspace->setNormalFontName(normalFont.value.c_str());
        if (smallFont.isSet ) fluxspace->setSmallFontName(smallFont.value.c_str());
        if (largeFont.isSet ) fluxspace->setLargeFontName(largeFont.value.c_str());

        window = fluxspace->createWindow().window();
        if (window == None)
            throw ErrorNoWindow();

        esetroot = new Esetroot(display);

        // We're ready to roll
        startedEvents = true;
        FluxspaceInterface fluxspaceInterface(*fluxspace, *pythonInterface, verbose);
        fluxspace->go(fluxspaceInterface);
    }

    ~ControllerImplementation()
    {
        delete fluxspace;
        delete pythonInterface;
        delete esetroot;
        delete fluxbox;
    }

  public:

    int AddRow(
        unsigned int height,
        const char*  unit,
        const char*  defaultColorName,
        const char*  button)
    {
        return fluxspace->addRow(height, unit, defaultColorName, button);
    }

    int AddCommand(
        int         rowfrom,
        int         rowto,
        const char* label,
        PyObject*   handler,
        const char* data)
    {
        // The Python callback mechanism looks up handlers by row #
        for (int row = rowfrom; row <= rowto; row++)
            pythonInterface->AddCommandHandler(handler, row);
        return fluxspace->addCommand(rowfrom, rowto, label, data);
    }

    void RemoveCommand(int commandid)
    {
        return fluxspace->removeCommand(commandid);
    }

    void DrawTextLeft(int row, const char* text, int offset, int font, const char* colorName)
    {
        fluxspace->addText(row, text, FluxspaceDisplay::LEFTJUSTIFY, offset, font, colorName);
    }

    void DrawTextRight(int row, const char* text, int offset, int font, const char* colorName)
    {
        fluxspace->addText(row, text, FluxspaceDisplay::RIGHTJUSTIFY, offset, font, colorName);
    }

    void DrawTextCenter(int row, const char* text, int offset, int font, const char* colorName)
    {
        fluxspace->addText(row, text, FluxspaceDisplay::CENTERJUSTIFY, offset, font, colorName);
    }

    void ClearRow(int row)
    {
        fluxspace->clearRow(row);
    }

    void SetWidth(int width_)
    {
        width.Set(width_);
    }

    void SetMargin(int margin_)
    {
        margin.Set(margin_);
    }

    void SetFrameWidth(int frameWidth_)
    {
        frameWidth.Set(frameWidth_);
    }

    void SetAntialias(bool antialias_)
    {
        antialias.Set(antialias_);
    }

    void SetSmallFont(const char* font)
    {
        smallFont.Set(font);
    }

    void SetNormalFont(const char* font)
    {
        normalFont.Set(font);
    }

    void SetLargeFont(const char* font)
    {
        largeFont.Set(font);
    }

    bool              verbose;
    bool              startedEvents;
    Display*          display;
    FluxspaceDisplay* fluxspace;
    PythonInterface*  pythonInterface;
    Esetroot*         esetroot;
    Window            window;
    FluxboxUtility*   fluxbox;

    OptionalInteger width;
    OptionalInteger margin;
    OptionalInteger frameWidth;
    OptionalInteger colorsPerChannel;
    OptionalBoolean imageDither;
    OptionalBoolean antialias;
    OptionalString  smallFont;
    OptionalString  normalFont;
    OptionalString  largeFont;
};

namespace
{
    static ControllerImplementation* imp = NULL;

    void CheckHealth(bool partial = false)
    {
        if (imp == NULL
#ifdef DEBUG
            || (   !partial
                && (   imp->display         == NULL
                    || imp->fluxspace       == NULL
                    || imp->pythonInterface == NULL
                    || imp->esetroot        == NULL))
#endif
           )
            throw ErrorBadController();
    }

    void CheckHealthAndWindow(Window window)
    {
        if (window == None)
            throw ErrorNoWindow();
        CheckHealth();
    }

    void GetProperty(
        Window      window,
        const Atom& propertyName,
        void*       buf,
        size_t      bufSize,
        const char* name)
    {
        CheckHealth();

        if (buf == NULL || bufSize <= 0)
            throw InternalError("Null or empty buffer provided to retrieve property");

        long offset = 0;
        long size32 = (bufSize + 3) / 4;
        Atom retType;
        int retFormat;
        unsigned long retCount, retRemaining;
        unsigned char* tmpPropBuf;

        if (XGetWindowProperty(imp->display,
                               window,
                               propertyName,
                               offset,
                               size32,
                               false,
                               AnyPropertyType,
                               &retType,
                               &retFormat,
                               &retCount,
                               &retRemaining,
                               &tmpPropBuf) != Success)
            throw ErrorWindowProperty(name, "get");

        size_t retPropSize = ( retFormat / 8 ) * retCount;
        if (tmpPropBuf == NULL || retPropSize <= 0)
        {
            memset(buf, 0, bufSize);
            return;
        }
        if (retPropSize > bufSize)
        {
            std::ostringstream os;
            os << bufSize << " byte buffer is too small for " << retPropSize << " byte \"" << name << "\" property";
            throw InternalError(os.str().c_str());
        }

        memcpy(buf, tmpPropBuf, retPropSize);
        if (retPropSize < bufSize)
            memset(((char*)buf)+retPropSize, 0, bufSize - retPropSize);

        XFree(tmpPropBuf);
    }

    void SetProperty(
        Window      window,
        const Atom& propertyName,
        const Atom& propertyType,
        int         format,
        void*       buf,
        size_t      bufSize,
        const char* name)
    {
        CheckHealth();

        if (buf == NULL || bufSize <= 0)
            throw InternalError("Null or empty buffer provided to retrieve property");

        size_t nElements = bufSize / (format / 8);
        if (XChangeProperty(imp->display,
                            window,
                            propertyName,
                            propertyType,
                            format,
                            PropModeReplace,
                            (unsigned char*)buf,
                            nElements) != Success)
            throw ErrorWindowProperty(name, "set");
    }
};

Error::Error()
{
}

Error::Error(const char* msg_)
  : msg(msg_)
{
}

Error::~Error()
{
}

void Error::SetMessage(const char* fmt, ...)
{
    char formattedMsg[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(formattedMsg, sizeof formattedMsg, fmt, args);
    va_end(args);

    msg = formattedMsg;

    if (msg.length() > 0)
        printf("ERROR: %s\n", msg.c_str());
}

InternalError::InternalError()
{
}

InternalError::InternalError(const char* msg_)
{
    SetMessage(msg_);
}

void InternalError::SetMessage(const char* fmt, ...)
{
    char formattedMsg[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(formattedMsg, sizeof formattedMsg, fmt, args);
    va_end(args);

    msg = formattedMsg;

    printf("INTERNAL ERROR: %s\n", msg.c_str());
}

ErrorWindowProperty::ErrorWindowProperty(const char* propName, const char* op)
  : Error()
{
    SetMessage("Unable to access \"%s\" window property for \"%s\" operation", propName, op);
}

Controller::Controller(const char* displayName_, bool verbose_)
  : displayName(displayName_),
    verbose(verbose_)
{
}

Controller::~Controller()
{
    delete imp;
    imp = NULL;
}

void Controller::SetSOAPHandler(PyObject* object)
{
    if (imp == NULL)
        imp = new ControllerImplementation(verbose);
    imp->pythonInterface->SetSOAPHandler(object);
}

bool Controller::RegisterFluxlet(const char* moduleName, PyObject* config)
{
    if (imp == NULL)
        imp = new ControllerImplementation(verbose);
    try
    {
        imp->pythonInterface->AddModule(moduleName, config);
    }
    catch(MyPyException)
    {
        return false;
    }
    return true;
}

void Controller::StartEvents()
{
    ::CheckHealth(true);
    imp->run(displayName.c_str());
}

void Controller::Trace(const char* message)
{
    if (verbose && message != NULL)
        printf("fluxspace: %s\n", message);
}

void Controller::Message(int severity, const char* message)
{
    char s[50];
    s[0] = '\0';
    switch(severity)
    {
      case 1:
        strcpy(s, " WARNING");
        break;
      case 2:
        strcpy(s, " ERROR");
        break;
    }
    printf("fluxspace%s: %s\n", s, message);
}

void Controller::SetWidth(int width)
{
    ::CheckHealth(true);
    imp->SetWidth(width);
}

void Controller::SetFrameWidth(int frameWidth)
{
    ::CheckHealth(true);
    imp->SetFrameWidth(frameWidth);
}

void Controller::SetRowGap(int rowGap)
{
    ::CheckHealth(true);
    imp->SetMargin(rowGap);
}

void Controller::SetAntialias(bool antialias)
{
    ::CheckHealth(true);
    imp->SetAntialias(antialias);
}

void Controller::SetSmallFont(const char* font)
{
    ::CheckHealth(true);
    imp->SetSmallFont(font);
}

void Controller::SetNormalFont(const char* font)
{
    ::CheckHealth(true);
    imp->SetNormalFont(font);
}

void Controller::SetLargeFont(const char* font)
{
    ::CheckHealth(true);
    imp->SetLargeFont(font);
}

FluxletHelper::FluxletHelper(const char* name_)
  : verbose(false)
{
    ::CheckHealth(true);
    verbose = imp->verbose;
    if (name_ != NULL)
        name = name_;
}

FluxletHelper::~FluxletHelper()
{
}

void FluxletHelper::AddHandler(PyObject* object, int tickSeconds)
{
    ::CheckHealth();
    imp->pythonInterface->AddHandler(object, tickSeconds);
}

void FluxletHelper::RegisterSOAPObject(PyObject* object, const char* subNameSpace)
{
    ::CheckHealth();
    std::string nameSpace = name;
    if (subNameSpace != NULL)
        nameSpace += std::string(".") + subNameSpace;
    imp->pythonInterface->RegisterSOAPObject(object, nameSpace);
}

void FluxletHelper::SetRoot(
    const char* fname,
    const char* bgcolor,
    bool        scale,
    bool        center,
    bool        fit,
    bool        mirror)
{
    ::CheckHealth();
    imp->esetroot->SetRoot(fname, bgcolor, scale, center, fit, mirror);
}

void FluxletHelper::Trace(const char* message)
{
    if (verbose && message != NULL)
        printf("fluxlet.%s: %s\n", name.c_str(), message);
}

void FluxletHelper::Message(int severity, const char* message)
{
    char s[50];
    s[0] = '\0';
    switch(severity)
    {
      case 1:
        strcpy(s, " WARNING");
        break;
      case 2:
        strcpy(s, " ERROR");
        break;
    }
    printf("fluxlet.%s%s: %s\n", name.c_str(), s, message);
}

int FluxletHelper::AddRow(
    unsigned int height,
    const char*  unit,
    const char*  defaultColorName,
    const char*  button)
{
    ::CheckHealth();
    return imp->AddRow(height, unit, defaultColorName, button);
}

int FluxletHelper::AddCommand(
    int         rowfrom,
    int         rowto,
    const char* label,
    PyObject*   handler,
    const char* data)
{
    ::CheckHealth();
    if (rowfrom < 0 || rowto < rowfrom)
        return -1;
    return imp->AddCommand(rowfrom, rowto, label, handler, data);
}

void FluxletHelper::RemoveCommand(int commandid)
{
    ::CheckHealth();
    return imp->RemoveCommand(commandid);
}

void FluxletHelper::DrawTextLeft(
    int         row,
    const char* text,
    int         offset,
    const char* colorName,
    int         font)
{
    imp->DrawTextLeft(row, text, offset, font, colorName);
}

void FluxletHelper::DrawTextRight(
    int         row,
    const char* text,
    int         offset,
    const char* colorName,
    int         font)
{
    imp->DrawTextRight(row, text, offset, font, colorName);
}

void FluxletHelper::DrawTextCenter(
    int         row,
    const char* text,
    int         offset,
    const char* colorName,
    int         font)
{
    imp->DrawTextCenter(row, text, offset, font, colorName);
}

void FluxletHelper::ClearRow(int row)
{
    imp->ClearRow(row);
}
