//  FluxspaceInterface.cc for fluxspace.
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
//

#include "PythonInterface.hh"
#include "FluxspaceInterface.hh"
#include "FluxspaceDisplay.hh"
#include <stdio.h>

FluxspaceInterface::FluxspaceInterface(
    FluxspaceDisplay& display_,
    PythonInterface&  pyinterface_,
    bool              verbose_)
  : workspace(-1),
    nWorkspaces(-1),
    pyinterface(pyinterface_),
    verbose(verbose_),
    display(display_),
    slitWindow(None)
{
}

FluxspaceInterface::~FluxspaceInterface()
{
}

void FluxspaceInterface::onStartup()
{
    if (verbose)
        printf(">>Startup\n");
    pyinterface.Initialize();
}

void FluxspaceInterface::onWindowCreate(Window w, Window parent)
{
    if (verbose)
        printf(">>WindowCreate: 0x%08x parent=0x%08x\n", (int)w, (int)parent);
    pyinterface.WindowCreate(w, parent);
}

void FluxspaceInterface::onWindowDestroy(Window w)
{
    if (verbose)
        printf(">>WindowDestroy: 0x%08x\n", (int)w);
    pyinterface.WindowDestroy(w);
}

void FluxspaceInterface::onWindowAdd(Window win,int desktop_nr)
{
    if (verbose)
        printf(">>WindowAdd: 0x%08x workspace=%d\n", (int)win, desktop_nr);
    pyinterface.WindowAdd(win, desktop_nr);
}

void FluxspaceInterface::onWindowDelete(Window win)
{
    if (verbose)
        printf(">>WindowRemove: 0x%08x\n", (int)win);
    pyinterface.WindowRemove(win);
}

void FluxspaceInterface::onWindowChangeAttributes(Window win)
{
    if (verbose)
        printf(">>WindowChangeAttributes: 0x%08x\n", (int)win);
    pyinterface.WindowChangeAttributes(win);
}

void FluxspaceInterface::onWindowFocus(Window win)
{
    if (window.focus.set(win))
    {
        if (verbose)
            printf(">>WindowFocus: 0x%08x\n", (int)win);
        pyinterface.WindowFocus(win);
    }
}

void FluxspaceInterface::onWorkspaceChangeCurrent(int curWorkspace)
{
    if (curWorkspace != workspace)
    {
        if (workspace >= 0)
            pyinterface.WorkspaceOut(workspace);
        if (verbose)
            printf(">>Workspace=%d\n", curWorkspace);
        workspace = curWorkspace;
        pyinterface.WorkspaceIn(curWorkspace);
    }
}

void FluxspaceInterface::onWindowRaise(Window win)
{
    if (verbose)
        printf(">>WindowRaise: 0x%08x\n", (int)win);
    pyinterface.WindowRaise(win);
}

void FluxspaceInterface::onWindowLower(Window win)
{
    if (verbose)
        printf(">>WindowLower: 0x%08x\n", (int)win);
    pyinterface.WindowLower(win);
}

void FluxspaceInterface::onWindowConfigure(Window win)
{
    if (verbose)
        printf(">>WindowConfigure: 0x%08x\n", (int)win);
    pyinterface.WindowConfigure(win);
}

void FluxspaceInterface::onWindowChangeParent(Window win, Window parent)
{
    if (verbose)
        printf(">>WindowChangeParent: 0x%08x  parent=0x%08x\n", (int)win, (int)parent);
    pyinterface.WindowChangeParent(win, parent);
}

void FluxspaceInterface::onWindowMap(Window win)
{
    if (verbose)
        printf(">>WindowMap: 0x%08x\n", (int)win);
    pyinterface.WindowMap(win);
}

void FluxspaceInterface::onWindowUnmap(Window win)
{
    if (verbose)
        printf(">>WindowUnmap: 0x%08x\n", (int)win);
    pyinterface.WindowUnmap(win);
}

void FluxspaceInterface::onWorkspaceChangeCount(int count)
{
    if (count != nWorkspaces)
    {
        if (verbose)
            printf(">>WorkspaceCount=%d\n", count);
        nWorkspaces = count;
        pyinterface.WorkspaceChangeCount(nWorkspaces);
    }
}

void FluxspaceInterface::onCommand(int row, const string& command, int button, const string& data)
{
    pyinterface.Command(row, command, button, data);
}

void FluxspaceInterface::onTick()
{
    pyinterface.Tick();
}
