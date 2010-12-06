#!/usr/bin/env python
######################################################################
######################################################################
# Fluxspace idesk support module
######################################################################
######################################################################

import os
import os.path
import sys
import os.path
from glob import glob
from fluxspace.config import Config

ideskCmd      = 'idesk'
idesktops     = os.path.expanduser('~/.fluxspace/idesktops')
idesktopsOld  = os.path.expanduser('~/.fluxbox/idesktops')
idesktop      = os.path.expanduser('~/.idesktop')

# Use ~/.fluxbox/idesktops from older Fluxspace
if not os.path.exists(idesktops) and os.path.exists(idesktopsOld):
    idesktops = idesktopsOld

desktopFmt    = 'desktop%d'
iconFmt = '''\
table Icon
  Caption: %s
  Command: "%s"
  Icon: %s
  X: %d
  Y: %d
end'''

# Get offsets from configuration
offsetx = offsety = 0
fluxspaceConfig = Config()
fluxspaceConfig.Load()
configs = fluxspaceConfig.GetModuleConfigurations('idesk')
# Arbitrarily use attributes from the last entry if there are multiple.
if len(configs) > 0:
    config = configs[-1]
    if config.getAttribute('offsetx'):
        offsetx = int(config.getAttribute('offsetx'))
    if config.getAttribute('offsety'):
        offsety = int(config.getAttribute('offsety'))

class IdeskDesktopsDirectoryUnavailable(Exception):
    pass
class IdeskDesktopDirectoryUnavailable(Exception):
    pass

######################################################################
def SetDesktop(helper, n = -1, link = 1):
######################################################################
    if n >= 0:
        dir = os.path.join(idesktops, desktopFmt % n)
    else:
        dir = idesktops
    if os.path.exists(dir):
        if not os.path.isdir(dir):
            helper.Message(2, 'Desktop directory "%s" already exists as a non-directory' % dir)
            raise IdeskDesktopsDirectoryUnavailable()
    else:
        helper.Message(0, 'Creating desktop directory "%s"...' % dir)
        os.makedirs(dir)
    # Only link if it's a specific desktop and the caller requests it
    if n >= 0 and link:
        helper.Trace('Linking desktop %d...' % n)
        if os.path.exists(idesktop):
            if not os.path.islink(idesktop):
                helper.Message(2, 'idesk directory "%s" already exists as a non-directory' % idesktop)
                raise IdeskDesktopDirectoryUnavailable()
            os.unlink(idesktop)
        os.symlink(dir, idesktop)
    return dir

######################################################################
class CellSpec:
######################################################################
    def __init__(self, width, height):
        self.width  = width
        self.height = height
        self.midx   = width / 2
        self.midy   = width / 2

######################################################################
class Desktop:
######################################################################
    def __init__(self, helper, workspace):
        global offsetx, offsety
        self.helper = helper
        self.width   = 640
        self.height  = 480
        self.offsetx = offsetx
        self.offsety = offsety
        self.GetScreenDimensions()
        self.Reset(workspace)

    def Reset(self, workspace):
        self.cellSpec  = CellSpec(100, 100)
        self.cells     = {}  # map cell grid usage hashed on (x,y) pairs
        self.workspace = workspace
        self.dir       = SetDesktop(self.helper, self.workspace, 0)
        self.MapIcons()

    def OffsetIcons(self, offsetx, offsety):
        self.offsetx = offsetx
        self.offsety = offsety

    # Deduce the width and height using xwininfo on the root window
    def GetScreenDimensions(self):
        for line in os.popen('xwininfo -root').readlines():
            try:
                (tag, val) = line.split()[:2]
                if tag.lower() == 'width:':
                    self.width = int(val)
                if tag.lower() == 'height:':
                    self.height = int(val)
            except ValueError:
                pass

    def Key(self, x, y):
        return (int(x-self.offsetx/self.cellSpec.width),
                int(y-self.offsety/self.cellSpec.height))

    def Mark(self, x, y):
        key = self.Key(x,y)
        if not self.cells.has_key(key):
            self.cells[key] = 0
        self.cells[key] += 1

    def IsMarked(self, x, y):
        key = self.Key(x,y)
        return (self.cells.has_key(key) and self.cells[key] > 0)

    def FindCell(self):
        for x in range(self.cellSpec.midx+self.offsetx, self.width, self.cellSpec.width):
            for y in range(self.cellSpec.midy+self.offsety, self.height, self.cellSpec.height):
                if not self.IsMarked(x, y):
                    return (x, y)
        return (self.cellSpec.midx+self.offsetx, self.cellSpec.midy+self.offsety)

        # Mark the cells occupied by other icons as used
    def MapIcons(self):
        for lnk in glob(os.path.join(self.dir, '*.lnk')):
            (x, y) = (0, 0)
            lnkf = open(lnk)
            for line in lnkf.readlines():
                try:
                    (tag, val) = line.split()[:2]
                    if tag.lower() == 'x:':
                        x = int(val)
                    if tag.lower() == 'y:':
                        y = int(val)
                except ValueError:
                    pass
            lnkf.close()
            self.Mark(x, y)

    def AddIcon(self, caption, command, iconPath, x = -1, y = -1):
        # Find a reasonable position based on the other desktop icons
        if x < 0 or y < 0:
            (x, y) = self.FindCell()

        # Determine a unique idesk icon file name
        name = os.path.split(command.split()[0])[1]
        path = os.path.join(self.dir, '%s.lnk' % name)
        i = 0
        while os.path.exists(path):
            i += 1
            path = os.path.join(self.dir, '%s%d.lnk' % (name, i))

        # Write the idesk icon file
        lnkf = open(path, 'w')
        lnkf.write(iconFmt % (caption, command, iconPath, x, y))
        lnkf.close()

        self.Mark(x, y)
        return (x, y)

    # Mirror a directory containing Rox AppDirs on a desktop
    def SyncAppDirs(self, directory, clean = 1):
        if clean:
            lnks = glob(os.path.join(self.dir, '*.lnk'))
            for lnk in lnks:
                try:
                    os.remove(lnk)
                except OSError:
                    pass
            self.Reset(self.workspace)
        for f in glob(os.path.join(directory, '*')):
            apprun = os.path.join(f, 'AppRun')
            if os.path.isdir(f) and os.access(apprun, os.X_OK):
                caption = os.path.split(f)[1]
                command = apprun
                iconPath = os.path.join(f, '.DirIcon')
                self.AddIcon(caption, command, iconPath)
