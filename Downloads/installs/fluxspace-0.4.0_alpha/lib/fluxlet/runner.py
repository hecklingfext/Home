#!/usr/bin/env python

from fluxspacecore import FluxletHelper
from fluxspace.process import Applet
import re

reApplet = re.compile('^[ \t]*([0-9]+)[ \t]+([a-z]+)[ \t]+([^ \t].*)[ \t]*$')
reCommandArg = re.compile('[ \t]*([^ \t"]+)|"([^"]*)"')

supportedActions = ['manage', 'run']

helper = FluxletHelper('runner')

class AppletManager:
    def __init__(self, helper):
        self.helper = helper
        self.workspaceApplets = {}
    def AddApplet(self, key, action, command):
        if not self.workspaceApplets.has_key(key):
            self.workspaceApplets[key] = []
        self.helper.Trace('Register workspace "%s" applet "%s"' % (key, command))
        self.workspaceApplets[key].append(Applet(self.helper, action, command))
    def ClearApplets(self, key):
        if self.workspaceApplets.has_key(key):
            self.workspaceApplets[key] = []
        self.helper.Trace('Clear workspace "%s" applets' % key)
    def StartApplets(self, key):
        self.helper.Trace('Start workspace "%s" applets' % key)
        if self.workspaceApplets.has_key(key):
            for applet in self.workspaceApplets[key]:
                applet.Start()
    def StopApplets(self, key):
        self.helper.Trace('Stop workspace "%s" applets' % key)
        if self.workspaceApplets.has_key(key):
            for applet in self.workspaceApplets[key]:
                applet.Stop()

class Handler:
    def __init__(self, appletManager):
        helper.Trace('Event handler created')
        self.appletManager = appletManager
    def Initialize(self):
        self.prevWorkspace = -1
        helper.Trace('Initialize')
    def WorkspaceIn(self, n):
        helper.Trace('WorkspaceIn %d' % n)
        if self.prevWorkspace >= 0:
            self.appletManager.StopApplets('%d' % self.prevWorkspace)
        else:
            self.appletManager.StartApplets('all')
        self.appletManager.StartApplets('%d' % n)
    def WorkspaceOut(self, n):
        helper.Trace('WorkspaceOut %d' % n)
        self.prevWorkspace = n
    def WindowMap(self, w):
        helper.Trace('WindowMap 0x%08x' % w)
    def WindowUnmap(self, w):
        helper.Trace('WindowUnmap 0x%08x' % w)

def fluxlet_main(config):
    appletManager = AppletManager(helper)
    programs = config.getElementsByTagName('program')
    if len(programs) == 0:
        return
    for program in programs:
        key = program.getAttribute('workspace')
        if not key:
            key = 'all'
        action = program.getAttribute('action')
        if not action:
            action = 'manage'
        command = program.getAttribute('command')
        if command:
            appletManager.AddApplet(key, action, command)
        else:
            helper.Message(1, 'Program with missing command skipped')
    helper.AddHandler(Handler(appletManager))
