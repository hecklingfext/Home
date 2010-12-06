#!/usr/bin/env python

from fluxspacecore import FluxletHelper
import os
import time

# Some versions of Xdialog require different --calendar option arguments
defaultCalendar = 'Xdialog --buttons-style text --ok-label Close --title Fluxspace --no-cancel --under-mouse --calendar Calendar 15 32 %d %m %Y'

helper = FluxletHelper('clock')

class Format:
    def __init__(self, format, size, color = ''):
        self.format = format
        self.size   = size
        self.color  = color
        self.row    = 0
        # Default to short blank rows
        if not self.size:
            if self.format:
                self.size = 'normal'
            else:
                self.size = 'small'

class Handler:
    def __init__(self, formats, calendarCommand):
        helper.Trace('Event handler created')
        self.formats = formats
        self.calendarCommand = calendarCommand
    def Initialize(self):
        rowFrom = -1
        rowTo   = -1
        for format in self.formats:
            format.row = helper.AddRow(height = 1,
                                       unit   = format.size,
                                       color  = format.color)
            if rowFrom < 0:
                rowFrom = format.row
            rowTo = format.row
        if rowFrom >= 0:
            helper.AddCommand(rowFrom, rowTo, 'Calendar', self, '')
        self.Tick()
    def Tick(self):
        for format in self.formats:
            if format.row >= 0:
                helper.ClearRow(format.row)
                helper.DrawTextCenter(format.row, time.strftime(format.format))
    def Command(self, command, button, data):
        if self.calendarCommand:
            command = time.strftime(self.calendarCommand)
            if os.fork() == 0:
                os.execlp('sh', 'sh', '-c', command)

def fluxlet_main(config):
    formats = []
    calendar = config.getAttribute('calendar')
    if not calendar:
        calendar = defaultCalendar
    for lineNode in config.getElementsByTagName('line'):
        format = lineNode.getAttribute('format')
        size   = lineNode.getAttribute('size')
        color  = lineNode.getAttribute('color')
        formats.append(Format(format, size, color))
    if len(formats) == 0:
        formats.append(Format('%X', 'large'))
        formats.append(Format('%x', 'normal'))
    helper.AddHandler(Handler(formats, calendar), 1)
