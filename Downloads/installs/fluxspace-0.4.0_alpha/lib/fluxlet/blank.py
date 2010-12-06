#!/usr/bin/env python

from fluxspacecore import FluxletHelper

helper = FluxletHelper('blank')

class Handler:
    def __init__(self, height, unit, color):
        helper.Trace('Event handler created')
        self.height = height
        self.unit   = unit
        self.color  = color
    def Initialize(self):
        helper.AddRow(self.height, self.unit, self.color)

def fluxlet_main(config):
    try:
        heightAttr = config.getAttribute('height')
        if heightAttr == '':
            helper.Message(0, 'Missing height attribute, assuming one')
            height = 1
        else:
            height = int(heightAttr)
    except ValueError, e:
        helper.Message(2, 'Bad height value "%s"' % config.getAttribute('height'))
        return
    unit   = config.getAttribute('unit')
    color  = config.getAttribute('color')
    helper.AddHandler(Handler(height, unit, color), 1)
