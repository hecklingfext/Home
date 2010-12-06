// FluxboxUtility.cc
//
//  Utility class to access Fluxbox information
//
//  Fluxspace is a workspace and slit manager for fluxbox
//
// Copyright (c) 2003 - 2004 Steve Cooper (stevencooper at users.sourceforge.net)
//
// Heavily modified and pared down version of fluxbox.cc from the Fluxbox project.
//
// Copyright (c) 2001 - 2003 Henrik Kinnunen (fluxgen at users.sourceforge.net)
//
// blackbox.cc for blackbox - an X11 Window manager
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

#include "fluxspacecore.hh"
#include "FluxboxUtility.hh"
#include <FbTk/Resource.hh>
#include <FbTk/Theme.hh>
#include <FbTk/StringUtil.hh>

FbTk::App* FluxboxUtility::app = NULL;

template<>
void FbTk::Resource<int>::setFromString(const char* strval)
{
    int val;
    if (sscanf(strval, "%d", &val)==1)
        *this = val;
}

template<>
void FbTk::Resource<std::string>::setFromString(const char *strval)
{
    *this = strval;
}

template<>
void FbTk::Resource<bool>::setFromString(char const *strval)
{
    if (strcasecmp(strval, "true")==0)
        *this = true;
    else
        *this = false;
}

template<>
std::string FbTk::Resource<int>::getString()
{
    char strval[256];
    sprintf(strval, "%d", **this);
    return std::string(strval);
}

template<>
std::string FbTk::Resource<std::string>::getString()
{
    return **this;
}

template<>
std::string FbTk::Resource<bool>::getString()
{
    return std::string(**this == true ? "true" : "false");
}

FluxboxUtility::FluxboxUtility(const char* displayName)
  : rcFile(""),
    colorsPerChannel(4),
    imageDither(false),
    antialias(false),
    m_RC_PATH("fluxbox"),
    m_RC_INIT_FILE("init")
{
    // There can only be one.
    if (app != NULL)
        throw ErrorAppExists();
    app = new FbTk::App(displayName);

    FbTk::ResourceManager resMgr("", true);

    // Resource definitions
    FbTk::Resource<std::string> rcStyleFile(resMgr,
                                            styleFile,
                                            "session.styleFile",
                                            "Session.StyleFile");
    FbTk::Resource<std::string> rcMenuFile(resMgr,
                                           menuFile,
                                           "session.menuFile",
                                           "Session.MenuFile");
    FbTk::Resource<int> rcColorsPerChannel(resMgr,
                                           colorsPerChannel,
                                           "session.colorsPerChannel",
                                           "Session.ColorsPerChannel");
    FbTk::Resource<bool> rcImageDither(resMgr,
                                       imageDither,
                                       "screen0.imageDither",
                                       "Screen0.ImageDither");
    FbTk::Resource<bool> rcAntialias(resMgr,
                                     antialias,
                                     "screen0.antialias",
                                     "Screen0.Antialias");

    // Get the resource filename
    std::string dbfile(getRCFilename());

    // Load the resource file
    bool loaded = false;
    if (dbfile.size() != 0)
        loaded = resMgr.load(dbfile.c_str());
    if (!loaded)
    {
        std::cerr<<"Failed to load database: "<<dbfile<<std::endl;
        throw ErrorAccessFluxbox();
    }

    // Copy the loaded resource values
    menuFile         = *rcMenuFile;
    styleFile        = *rcStyleFile;
    colorsPerChannel = *rcColorsPerChannel;
    imageDither      = *rcImageDither;
    antialias        = *rcAntialias;

    // Expand tilde paths
    styleFile = FbTk::StringUtil::expandFilename(styleFile.c_str());
    menuFile  = FbTk::StringUtil::expandFilename(menuFile.c_str());

    // Enforce between 2 and 6 colors per channel.
    if (colorsPerChannel > 6)
        colorsPerChannel = 6;
    else if (colorsPerChannel < 2)
        colorsPerChannel = 2;

    FbTk::ThemeManager::instance().load(styleFile.c_str());

    resMgr.unlock();
}


FluxboxUtility::~FluxboxUtility()
{
    delete app;     // Assumes only one instance will exist of this class
    app = NULL;     // CYA
}

/// Return filename of resource file
std::string FluxboxUtility::getRCFilename()
{
    if (rcFile.size() == 0)
    {
        std::string defaultfile(   getenv("HOME")
                                + std::string("/.")
                                + m_RC_PATH
                                + std::string("/")
                                + m_RC_INIT_FILE);
        return defaultfile;
    }

    return rcFile;
}

/// Provide default filename of data file
void FluxboxUtility::getDefaultDataFilename(const char* name, std::string& filename)
{
    filename = std::string(   getenv("HOME")
                           + std::string("/.")
                           + m_RC_PATH
                           + std::string("/")
                           + name);
}

