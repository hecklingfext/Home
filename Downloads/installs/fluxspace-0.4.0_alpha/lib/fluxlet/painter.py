#!/usr/bin/env python

from fluxspacecore import FluxletHelper
import os.path

wallpapers = {}

helper = FluxletHelper('painter')

class Wallpaper:
    def __init__(self, fname, bgcolor = None, scale = 0, center = 0, fit = 0, mirror = 0):
        self.fname   = fname
        self.bgcolor = bgcolor
        self.scale   = scale
        self.center  = center
        self.fit     = fit
        self.mirror  = mirror

class Handler:
    def __init__(self):
        helper.Trace('Event handler created')
    def Initialize(self):
        helper.Trace('Initialize')
    def WorkspaceIn(self, n):
        key = str(n)
        if not wallpapers.has_key(key):
            key = 'default'
        if wallpapers.has_key(key):
            w = wallpapers[key]
            helper.Trace('WorkspaceIn %d "%s"' % (n, w.fname))
            helper.SetRoot(fname=w.fname, bgcolor=w.bgcolor, scale=w.scale,
                           center=w.center, fit=w.fit, mirror=w.mirror)
        else:
            helper.Trace('No wallpaper for workspace %d' % n)
    def WorkspaceOut(self, n):
        helper.Trace('WorkspaceOut %d' % (n))

def fluxlet_main(config):
    for wallpaperNode in config.getElementsByTagName('wallpaper'):
        key     = wallpaperNode.getAttribute('workspace')
        fname   = os.path.expanduser(wallpaperNode.getAttribute('image'))
        bgcolor = wallpaperNode.getAttribute('bgcolor')
        scale   = (wallpaperNode.getAttribute('scale').lower()  == 'yes')
        center  = (wallpaperNode.getAttribute('center').lower() == 'yes')
        fit     = (wallpaperNode.getAttribute('fit').lower()    == 'yes')
        mirror  = (wallpaperNode.getAttribute('mirror').lower() == 'yes')
        if fname:
            if not key:
                key = 'default'
            wallpapers[key] = Wallpaper(fname, bgcolor, scale, center, fit, mirror)
        else:
            helper.Message(2, "You must specify an image for each wallpaper.")
    helper.AddHandler(Handler())
