//  FluxspaceInterface.hh for fluxspace.
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
#ifndef __FLUXSPACEINTERFACE_HH
#define __FLUXSPACEINTERFACE_HH

class PythonInterface;
class FluxspaceDisplay;

class FluxspaceInterface
{
  public:
    FluxspaceInterface(FluxspaceDisplay& display_,
                       PythonInterface&  pyinterface_,
                       bool              verbose_ = false);
    virtual ~FluxspaceInterface();

    // Event handling methods
    virtual void onStartup();
    virtual void onWindowCreate(Window w, Window parent);
    virtual void onWindowDestroy(Window w);
    virtual void onWindowAdd(Window w, int workspace);
    virtual void onWindowDelete(Window w);
    virtual void onWindowChangeAttributes(Window w);
    virtual void onWindowChangeParent(Window w, Window parent);
    virtual void onWindowConfigure(Window w);
    virtual void onWindowMap(Window w);
    virtual void onWindowUnmap(Window w);
    virtual void onWindowFocus(Window w);
    virtual void onWindowRaise(Window w);
    virtual void onWindowLower(Window w);
    virtual void onWorkspaceChangeCurrent(int workspace);
    virtual void onWorkspaceChangeCount(int nWorkspaces);
	virtual void onCommand(int row, const string& command, int button, const string& data);
	virtual void onTick();

    Window getSlitWindow() const    { return slitWindow; }

  private:
    int              workspace;
    int              nWorkspaces;
    PythonInterface& pyinterface;
    bool             verbose;

    class TrackedWindow
    {
      public:
        TrackedWindow()
          : w(None)
        {}
        operator Window()
        {
            return w;
        }
        const TrackedWindow& operator=(Window w_)
        {
            w = w_;
            return *this;
        }
        bool set(Window w_)
        {
            if (w != w_)
            {
                w = w_;
                return true;
            }
            return false;
        }
        Window w;
    };

    class Windows
    {
      public:
        TrackedWindow focus;
    } window;

    FluxspaceDisplay& display;
    Window            slitWindow;
};

#endif // __FLUXSPACEINTERFACE_HH
