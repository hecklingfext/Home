#!/usr/bin/env python

from fluxspacecore import FluxletHelper
from fluxspace.process import Applet
import os
import os.path

# Provides SetDesktop() and the strings idesktops, ideskCmd, etc.
from fluxspace.ideskutil import *

helper = FluxletHelper('idesk')

class Handler:
    def __init__(self):
        self.options = []
        self.applet  = Applet(helper, 'manage', ideskCmd)
    def Initialize(self):
        helper.Trace('Initialize')
    def WorkspaceIn(self, n):
        helper.Trace('WorkspaceIn %d' % n)
        SetDesktop(helper, n)
        self.applet.Restart()
    def WorkspaceOut(self, n):
        helper.Trace('WorkspaceOut %d' % n)
    def AddWorkspaceOption(self, option):
        helper.Trace('AddWorkspaceOption("%s")' % option)
        self.options.append(Option(option, 1))

def fluxlet_main(config):

    # Create root desktop directory separately so that errors are more specific
    SetDesktop(helper)
    if os.path.exists(idesktops):
        if not os.path.isdir(idesktops):
            helper.message(2, 'Desktops root directory "%s" already exists as a non-directory')
            return
    else:
        helper.Message(0, 'Creating desktops root directory "%s"...' % idesktops)
        os.makedirs(idesktops)

    handler = Handler()
    helper.AddHandler(handler)
