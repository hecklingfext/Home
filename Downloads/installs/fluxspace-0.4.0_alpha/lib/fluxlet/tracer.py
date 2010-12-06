#!/usr/bin/env python

from fluxspacecore import FluxletHelper

helper = FluxletHelper('tracer')

class Handler:
    def __init__(self):
        helper.Message(0, 'Main event broker created')
    def Initialize(self):
        helper.Message(0, 'Initialize')
    def WorkspaceIn(self, n):
        helper.Message(0, 'WorkspaceIn')
    def WorkspaceOut(self, n):
        helper.Message(0, 'WorkspaceOut')
    def WorkspaceChangeCount(self, count):
        helper.Message(0, 'WorkspaceChangeCount(%d)' % count)
    def WindowCreate(self, w, parent):
        helper.Message(0, 'WindowCreate(0x%08x, parent=0x%08x)' % (w, parent))
    def WindowDestroy(self, w):
        helper.Message(0, 'WindowDestroy(0x%08x)' % w)
    def WindowAdd(self, w, workspace):
        helper.Message(0, 'WindowAdd(0x%08x, workspace=%d)' % (w, workspace))
    def WindowRemove(self, w):
        helper.Message(0, 'WindowRemove(0x%08x)' % w)
    def WindowMap(self, w):
        helper.Message(0, 'WindowMap(0x%08x)' % w)
    def WindowUnmap(self, w):
        helper.Message(0, 'WindowUnmap(0x%08x)' % w)
    def WindowChangeAttributes(self, w):
        helper.Message(0, 'WindowChangeAttributes(0x%08x)' % w)
    def WindowFocus(self, w):
        helper.Message(0, 'WindowFocus(0x%08x)' % w)
    def WindowRaise(self, w):
        helper.Message(0, 'WindowRaise(0x%08x)' % w)
    def WindowLower(self, w):
        helper.Message(0, 'WindowLower(0x%08x)' % w)
    def WindowConfigure(self, w):
        helper.Message(0, 'WindowConfigure(0x%08x)' % w)
    def WindowChangeParent(self, w, parent):
        helper.Message(0, 'WindowChangeParent(0x%08x, parent=0x%08x)' % (w, parent))
    def Tick(self):
        helper.Message(0, 'Tick')

def fluxlet_main(config):
    helper.AddHandler(Handler())
