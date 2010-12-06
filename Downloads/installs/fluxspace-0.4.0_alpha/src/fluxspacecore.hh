//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// fluxspacecore.hh
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

#include <Python.h>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <X11/Xlib.h>

class Error
{
  public:
    Error(const char* msg_);
    virtual ~Error();
    std::string msg;

  protected:
    Error();
    virtual void SetMessage(const char* fmt, ...);
};

class InternalError : public Error
{
  public:
    InternalError(const char* msg_);

  protected:
    InternalError();
    virtual void SetMessage(const char* fmt, ...);
};

class ErrorImportModule : public Error
{
  public:
    ErrorImportModule(const char* module)
    {
        SetMessage("Unable to import \"%s\" module", module);
    }
};

class ErrorNoDisplay : public Error
{
  public:
    ErrorNoDisplay() : Error("X Display has not been established") {}
};

class ErrorAccessFluxbox : public Error
{
  public:
    ErrorAccessFluxbox() : Error("Unable to access fluxbox configuration") {}
};

class ErrorAppExists : public Error
{
  public:
    ErrorAppExists() : Error("An FbTk::App object already exists") {}
};

class ErrorControllerExists : public Error
{
  public:
    ErrorControllerExists() : Error("A Controller already exists") {}
};

class ErrorNoController : public Error
{
  public:
    ErrorNoController() : Error("A Controller must be created") {}
};

class ErrorEventLoopRunning : public Error
{
  public:
    ErrorEventLoopRunning() : Error("Event loop has already been started") {}
};

class ErrorBadController : public Error
{
  public:
    ErrorBadController() : Error("Controller has not been properly initialized") {}
};

class ErrorNoWindow : public Error
{
  public:
    ErrorNoWindow() : Error("Window does not exist") {}
};

class ErrorWindowProperty : public Error
{
  public:
    ErrorWindowProperty(const char* propName, const char* op);
};

class Controller
{
  public:
    Controller(const char* displayName_, bool verbose_ = false);
    virtual ~Controller();
    void SetSOAPHandler(PyObject* object);
    bool RegisterFluxlet(const char* moduleName, PyObject* config);
    void StartEvents();
    void SetWidth(int width);
    void SetFrameWidth(int frameWidth);
    void SetRowGap(int rowGap);
    void SetAntialias(bool antialias);
    void SetNormalFont(const char* font);
    void SetSmallFont(const char* font);
    void SetLargeFont(const char* font);

    // debug-only message
    void Trace(const char* message);
    // severity: 0=information, 1=warning, 2=error
    void Message(int severity, const char* message);

    std::string displayName;
    bool verbose;
};

const int NOFONT = -9999;

class FluxletHelper
{
  public:

    FluxletHelper(const char* name);
    virtual ~FluxletHelper();

    // Specify fluxlet handler object to receive events.
    void AddHandler(PyObject* handler, int tickSeconds = 1);

    // Register a SOAP object
    void RegisterSOAPObject(PyObject* object, const char* subNameSpace = NULL);

    // Paint the root window (desktop)
    void SetRoot(const char* fname,
                 const char* bgcolor = NULL,
                 bool        scale   = false,
                 bool        center  = false,
                 bool        fit     = false,
                 bool        mirror  = false);

    // Allocate a row the width of the window and the specified height.
    // The unit argument specifies the unit of measure, as follows:
    //      normal = multiple of normal text font height
    //      small  = multiple of small text font height
    //      large  = multiple of large text font height
    //      pixel  = number of pixels
    // Returns a row id.
    int AddRow(unsigned int height,
               const char*  unit   = "normal",
               const char*  color  = NULL,
               const char*  button = NULL);

    // Command labels must be unique.  They can represent submenu items
    // if specified with slash ('/') separators.
    // If only a single command exists for a row or row range a button is
    // displayed that spans the range.  Otherwise a menu is displayed.
    int AddCommand(int         rowfrom,
                   int         rowto,
                   const char* label,
                   PyObject*   handler,
                   const char* data);
    void RemoveCommand(int commandid);

    // Display text in a row with optional offset and font size overrides.
    // Choose from methods below based on desired justification.
    void DrawTextLeft(int         row,
                      const char* text,
                      int         offset = 0,
                      const char* color  = NULL,
                      int         font   = NOFONT);
    void DrawTextRight(int         row,
                       const char* text,
                       int         offset = 0,
                       const char* color  = NULL,
                       int         font   = NOFONT);
    void DrawTextCenter(int         row,
                        const char* text,
                        int         offset = 0,
                        const char* color  = NULL,
                        int         font   = NOFONT);

    // Clear a row
    void ClearRow(int row);

    // Display a debug-only trace message
    void Trace(const char* message);

    // Display an error, warning or informative message.
    //  severity: 0=information, 1=warning, 2=error
    void Message(int severity, const char* message);

    bool verbose;
    std::string name;
};
