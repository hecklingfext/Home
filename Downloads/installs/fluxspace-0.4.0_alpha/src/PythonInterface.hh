//  PythonInterface.hh for fluxspace.
//
//  Copyright (c) 2002 Steve Cooper, stevencooper@isomedia.com
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
//

#ifndef __PYTHONINTERFACE_HH
#define __PYTHONINTERFACE_HH

#include "MyPy.hh"
#include <X11/Xlib.h>

class PythonInterface
{
  public:
    PythonInterface();
    virtual ~PythonInterface();
    static void AddModule(const string& name, PyObject* config);
    static void AddHandler(PyObject* object, int tickSeconds);
    static void AddCommandHandler(PyObject* object, int nRow);
    void SetSOAPHandler(PyObject* object);
    void RegisterSOAPObject(PyObject* object, const string& nameSpace);
    void SOAPPoll();
    void Initialize();
    void WindowCreate(Window window, Window parent);
    void WindowDestroy(Window window);
    void WindowAdd(Window window, int workspace);
    void WindowRemove(Window window);
    void WindowChangeAttributes(Window window);
    void WindowFocus(Window window);
    void WindowRaise(Window window);
    void WindowLower(Window window);
    void WindowConfigure(Window window);
    void WindowChangeParent(Window window, Window parent);
    void WindowMap(Window window);
    void WindowUnmap(Window window);
    void WorkspaceChangeCount(int nWorkspaces);
    void WorkspaceIn(int workspace);
    void WorkspaceOut(int workspace);
    void Command(int row, const string& command, int button, const string& data);
    void Tick();

    static size_t GetModuleCount();

  private:
    static size_t moduleCount;
    MyPyObjectRef soapServer;
    MyPyMethod*   methRegisterSOAPObject;
    MyPyMethod*   methSOAPPoll;
};

#endif // __PYTHONINTERFACE_HH
