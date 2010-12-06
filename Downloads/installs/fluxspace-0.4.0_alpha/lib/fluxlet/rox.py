#!/usr/bin/env python

from fluxspacecore import FluxletHelper
import os

rox = 'rox'

helper = FluxletHelper('rox')

class Option:
    def __init__(self, string, appendWorkspace):
        self.string = string
        self.appendWorkspace = appendWorkspace
    def Arg(self, workspace):
        if self.appendWorkspace:
            return self.string + str(workspace)
        return self.string

class Handler:
    def __init__(self):
        self.options = []
    def Initialize(self):
        helper.Trace('Initialize')
    def WorkspaceIn(self, n):
        helper.Trace('WorkspaceIn %d' % n)
        command = rox
        for option in self.options:
            command += ' ' + option.Arg(n)
        helper.Trace('Execute: "%s"' % command)
        os.system(command)
    def WorkspaceOut(self, n):
        helper.Trace('WorkspaceOut %d' % n)
    def AddWorkspaceOption(self, option):
        helper.Trace('AddWorkspaceOption("%s")' % option)
        self.options.append(Option(option, 1))

def fluxlet_main(config):

    handler = Handler()

    panel     = None
    pinboard  = None
    panels    = config.getElementsByTagName('panel')
    pinboards = config.getElementsByTagName('pinboard')
    if len(panels) > 0:
        panel = panels[0]
    if len(panels) > 1:
        helper.Message(1, 'Multiple panels found.  All but first ignored')
    if len(pinboards) > 0:
        pinboard = pinboards[0]
    if len(pinboards) > 1:
        helper.Message(1, 'Multiple pinboards found.  All but first ignored')

    if panel is None and pinboard is None:
        return

    if panel is not None:
        name = panel.getAttribute('name')
        if not name:
            name = 'panel'
        pos = panel.getAttribute('position')
        if pos == 'right':
            handler.AddWorkspaceOption('-r=%s' % name)
        elif pos == 'top':
            handler.AddWorkspaceOption('-t=%s' % name)
        elif pos == 'bottom':
            handler.AddWorkspaceOption('-b=%s' % name)
        else:
            handler.AddWorkspaceOption('-l=%s' % name)
    if pinboard is not None:
        name = panel.getAttribute('name')
        if not name:
            name = 'desktop'
        handler.AddWorkspaceOption('-p=%s' % name)

    helper.AddHandler(handler)
