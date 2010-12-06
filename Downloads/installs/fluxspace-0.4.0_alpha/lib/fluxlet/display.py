#!/usr/bin/env python
# Copyright (c) 2004 Steve Cooper

from fluxspacecore import FluxletHelper
import os

helper = FluxletHelper('display')

# Pass-through interface provided for asynchronous processes gathering
# information to display.
class DisplaySOAPServer:
    def __init__(self):
        self.nCommands = 0
    def SetRoot(self,
                fname,
                bgcolor = '',
                scale   = False,
                center  = False,
                fit     = False,
                mirror  = False):
        helper.SetRoot(fname, bgcolor, scale, center, fit, mirror)
    def AddRow(self, height, unit = 'normal', color = '', button = ''):
        return helper.AddRow(height, unit, color, button)
    # Support bulk row definition to assure that asynchronous processes can
    # receive contiguous blocks of rows.
    # Each argument is an array (height, unit, color, label, command).
    # Any element of the array can be omitted by passing a shorter array.
    def AddRows(self, rows):
        firstRow = -1
        for row in rows:
            height  = 1
            unit    = 'normal'
            color   = ''
            label   = ''
            command = ''
            nElements = len(row)
            if nElements > 0:
                height = row[0]
            if nElements > 1 and row[1]:
                unit = row[1]
            if nElements > 2 and row[2]:
                color = row[2]
            if nElements > 3 and row[3]:
                label = row[3]
            if nElements > 4 and row[4]:
                command = row[4]
            nRow = helper.AddRow(height, unit, color, label)
            if firstRow < 0:
                firstRow = nRow
            if command:
                self.AddCommand(nRow, nRow, label, command)
        return firstRow
    def AddCommand(self, rowfrom, rowto, label, command):
        self.nCommands += 1
        return helper.AddCommand(rowfrom, rowto, label, self, command)
    def RemoveCommand(self, commandid):
        helper.RemoveCommand(commandid)
    def GetCommandCount(self):
        return self.nCommands
    def DrawTextLeft(self, row, text, offset = 0, color = '', font = -9999):
        helper.DrawTextLeft(row, text, offset, color, font)
    def DrawTextRight(self, row, text, offset = 0, color = '', font = -9999):
        helper.DrawTextRight(row, text, offset, color, font)
    def DrawTextCenter(self, row, text, offset = 0, color = '', font = -9999):
        helper.DrawTextCenter(row, text, offset, color, font)
    def ClearRow(self, row):
        helper.ClearRow(row)
    def Trace(self, message):
        helper.Trace(message)
    def Message(self, severity, message):
        helper.Message(severity, message)
    def Command(self, label, button, dataCommand):
        if dataCommand:
            if os.fork() == 0:
                os.execlp('sh', 'sh', '-c', dataCommand)

def fluxlet_main(config):
    server = DisplaySOAPServer()
    helper.RegisterSOAPObject(server)
