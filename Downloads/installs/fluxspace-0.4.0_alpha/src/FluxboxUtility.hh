// FluxboxUtility.hh
//
//  Utility class to access Fluxbox information
//
//  Fluxspace is a workspace and slit manager for fluxbox
//
// Copyright (c) 2003 - 2004 Steve Cooper (stevencooper at users.sourceforge.net)
//
// Heavily modified and pared down version of fluxbox.hh from the Fluxbox project.
//
// Copyright (c) 2001 - 2003 Henrik Kinnunen (fluxgen at users.sourceforge.net)
//
// blackbox.hh for Blackbox - an X11 Window manager
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
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef FLUXBOXUTILITY_HH
#define FLUXBOXUTILITY_HH

#include <string>
#include <FbTk/App.hh>

class FluxboxUtility
{
  public:
    FluxboxUtility(const char* displayName);
    virtual ~FluxboxUtility();

    const std::string &getStyleFilename() const { return styleFile; }
    const std::string &getMenuFilename() const  { return menuFile; }

    int  getColorsPerChannel() const { return colorsPerChannel; }
    bool getImageDither() const      { return imageDither; }
    bool getAntialias() const        { return antialias; }

    FbTk::App& getApp() { return *app; }

    void getDefaultDataFilename(const char* name, std::string& fileName);

  private:
    std::string getRCFilename();

    // Resources values
    std::string styleFile, menuFile, rcFile;
    int  colorsPerChannel;
    bool imageDither;
    bool antialias;
    const char *m_RC_PATH;
    const char *m_RC_INIT_FILE;

    static FbTk::App* app;
};

#endif // FLUXBOXUTILITY_HH
