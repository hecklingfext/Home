// FluxspaceTheme.cc
//
// Copyright (c) 2003 Steve Cooper (stevencooper at users.sourceforge.net)
//
// Based on the fluxbox IconbarTheme source
//
// Copyright (c) 2003 Henrik Kinnunen (fluxgen at users.sourceforge.net)
//                and Simon Bowden    (rathnor at users.sourceforge.net)
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

#include "FluxspaceTheme.hh"

FluxspaceTheme::FluxspaceTheme(int screenNum)
  : FbTk::MenuTheme(screenNum),
    antialias(false)
{
    FbTk::ThemeManager::instance().loadTheme(*this);
}

FluxspaceTheme::~FluxspaceTheme()
{
}

void FluxspaceTheme::reconfigTheme()
{
    titleFont().setAntialias(antialias);
    frameFont().setAntialias(antialias);
}
