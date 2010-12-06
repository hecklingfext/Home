#!/usr/bin/env python

# Modified from original painter module by 
#  Sarah Walters (sarah@walters.homeunix.org)

# <!-- Painter module with random wallpapers -->
#  <randpaint enable="yes">
#    <random directory="~/.kde3.1/share/wallpapers"/>
#  </randpaint>

import fluxspace
import os.path
from random import randint

wallpapers = {}
randpapers = []
randlen = 0

helper = fluxspace.FluxletHelper('randpaint')

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
	w = None
        key = str(n)
        if wallpapers.has_key(key):
            w = wallpapers[key]
        elif randlen:
            w = get_rand_paper()
        else:
            key = 'default'
            if wallpapers.has_key(key):
                w = wallpapers[key]

        if w:
            helper.Trace('WorkspaceIn %d "%s"' % (n, w.fname))
            helper.SetRoot(fname=w.fname, bgcolor=w.bgcolor, scale=w.scale,
                           center=w.center, fit=w.fit, mirror=w.mirror)
        else:
            helper.Trace('No wallpaper for workspace %d' % n)
    def WorkspaceOut(self, n):
        helper.Trace('WorkspaceOut %d' % (n))

def get_rand_paper():
    rand = randint(0, randlen)
    return randpapers[rand]

def fluxlet_main(config):
    global randlen
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
    for randNode in config.getElementsByTagName('random'):
        rndir   = os.path.expanduser(randNode.getAttribute('directory')) # Directory containing images
        bgcolor = randNode.getAttribute('bgcolor')
        scale   = (randNode.getAttribute('scale').lower()  == 'yes')
        center  = (randNode.getAttribute('center').lower() == 'yes')
        fit     = (randNode.getAttribute('fit').lower()    == 'yes')
        mirror  = (randNode.getAttribute('mirror').lower() == 'yes')
        # Is faster to do as an exception if user uses appropriate directory
        try:
            imglist = os.listdir(rndir)
        except:
            helper.Trace('Error listing directory %s, skipping' % rndir)
            continue # Okay, user was silly.
        # We assume all files are images
        for img in imglist:
            rw = Wallpaper(os.path.join(rndir, img), bgcolor, scale, center, fit, mirror)
            randpapers.append(rw)
    randlen = len(randpapers) - 1
    helper.AddHandler(Handler())
