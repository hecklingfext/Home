#!/usr/bin/env python

import time
from fluxspacecore import FluxletHelper
from fluxspace.config import GetString, GetInteger

helper = FluxletHelper('system')

class Monitor:
    def __init__(self, procPath, labelColor, textColor, size, command):
        self.procPath       = procPath
        self.labelColor     = labelColor
        self.textColor      = textColor
        self.size           = size
        if self.size == '':
            self.size = 'normal'
        self.command        = command
        self.lastUpdateTime = 0.0
        self.timeDelta      = 0.0
        self.updateTime     = 0.0
        self.rows           = []
        self.Update()
    def AddRows(self, quantity):
        for i in range(quantity):
            self.rows.append(helper.AddRow(height = 1, unit = self.size, color = self.labelColor))
    def Initialize(self):
        self.DoInitialize()
    def Update(self):
        self.updateTime = time.time()
        if self.lastUpdateTime != 0.0:
            self.timeDelta = self.updateTime - self.lastUpdateTime
        f = open(self.procPath)
        lines = f.readlines()
        f.close()
        self.DoUpdate(lines)
        self.lastUpdateTime = self.updateTime
    def Display(self):
        self.DoDisplay()

class CPU(Monitor):
    def __init__(self, labelColor, textColor, size, command):
        self.oldStats    = None
        self.user        = 0
        self.nice        = 0
        self.sys         = 0
        self.idle        = 0
        self.total       = 0
        self.usedPercent = 0
        Monitor.__init__(self, '/proc/stat', labelColor, textColor, size, command)
    def DoInitialize(self):
        self.AddRows(1)
    def DoUpdate(self, lines):
        newStats = lines[1].split()
        if self.oldStats is not None:
            self.user  = int(newStats[1]) - self.user
            self.nice  = int(newStats[2]) - self.nice
            self.sys   = int(newStats[3]) - self.sys
            self.idle  = int(newStats[4]) - self.idle
            self.used  = self.user + self.nice + self.sys
            self.total = self.used + self.idle
            if self.total != 0:
                self.usedPercent = int((self.used * 100.0 + 0.5) / self.total)
            else:
                self.usedPercent = 0
        self.oldStats = newStats
    def DoDisplay(self):
        helper.ClearRow(self.rows[0])
        helper.DrawTextLeft(self.rows[0], 'CPU')
        helper.DrawTextRight(self.rows[0], '%d%%' % self.usedPercent, color = self.textColor)

class Memory(Monitor):
    def __init__(self, labelColor, textColor, size, command):
        self.total     = 0
        self.used      = 0
        self.free      = 0
        self.shared    = 0
        self.buffers   = 0
        self.cached    = 0
        self.available = 0
        self.committed = 0
        self.swapTotal = 0
        self.swapUsed  = 0
        self.swapFree  = 0
        Monitor.__init__(self, '/proc/meminfo', labelColor, textColor, size, command)
    def DoInitialize(self):
        self.AddRows(1)
    def DoUpdate(self, lines):
        memDone = swapDone = False
        for line in lines:
            if line[:4].lower() == 'mem:':
                stats = line.split()
                self.total     = int(stats[1]) / 1048576
                self.used      = int(stats[2]) / 1048576
                self.free      = int(stats[3]) / 1048576
                self.shared    = int(stats[4]) / 1048576
                self.buffers   = int(stats[5]) / 1048576
                self.cached    = int(stats[6]) / 1048576
                self.available = self.free + self.buffers + self.cached
                self.committed = self.total - self.available
                memDone = True
            elif line[:5].lower() == 'swap:':
                stats = line.split()
                self.swapTotal = int(stats[1]) / 1048576
                self.swapUsed  = int(stats[2]) / 1048576
                self.swapFree  = int(stats[3]) / 1048576
                swapDone = True
            if memDone and swapDone:
                return
    def DoDisplay(self):
        helper.ClearRow(self.rows[0])
        helper.DrawTextLeft(self.rows[0], 'MEM')
        helper.DrawTextRight(self.rows[0], '%dM' % self.committed, color = self.textColor)

class NetworkDevice:
    def __init__(self, name, unit = 'B'):
        self.name   = name
        self.rbytes = 0
        self.tbytes = 0
        self.bytes  = 0
        self.rbps   = 0.0
        self.tbps   = 0.0
        self.bps    = 0.0
        self.unit   = unit
        if not self.unit or self.unit not in 'BKM':
            self.unit = 'B'
    def __cmp__(self, other):
        return cmp(self.name, other.name)
    def Update(self, rbytes, tbytes, timeDelta):
        if timeDelta > 0.0:
            self.rbps = float(rbytes - self.rbytes) / timeDelta
            self.tbps = float(tbytes - self.tbytes) / timeDelta
            self.bps  = self.rbps + self.tbps
        self.rbytes = rbytes
        self.tbytes = tbytes
        self.bytes  = self.rbytes + self.tbytes
        if not self.unit or self.unit not in 'BKM':
            self.unit = 'B'
        if self.unit == 'B':
            if self.bps < 1000.0:
                self.sbps = '%.1f' % self.bps
            else:
                self.unit = 'K'
        if self.unit == 'K':
            if self.bps < 1000000.0:
                self.sbps = '%.1fK' % (self.bps / 1000.0)
            else:
                self.unit = 'M'
        if self.unit == 'M':
            self.sbps = '%.1fM' % (self.bps / 1000000.0)

def DiscoverNetworkDevices():
    devices = []
    f = open('/proc/net/dev')
    for line in f.readlines():
        halves = line.split(':')
        if len(halves) != 2:
            continue
        interfaceName = halves[0].strip()
        device = NetworkDevice(interfaceName)
        devices.append(device)
    f.close()
    devices.sort()
    return devices

class Network(Monitor):
    def __init__(self, devices, labelColor, textColor, size, command):
        self.devices = devices
        Monitor.__init__(self, '/proc/net/dev', labelColor, textColor, size, command)
    def DoInitialize(self):
        self.AddRows(len(self.devices))
    def DoUpdate(self, lines):
        # First gather the raw data
        deviceData = {}
        for line in lines:
            halves = line.split(':')
            if len(halves) == 2:
                name = halves[0].strip()
                data = halves[1].split()
                deviceData[name] = data
        # Correlate the data with monitored devices
        for device in self.devices:
            if device.name in deviceData:
                stats = deviceData[device.name]
                device.Update(int(stats[0]), int(stats[8]), self.timeDelta)
    def DoDisplay(self):
        for i in range(len(self.devices)):
            row    = self.rows[i]
            device = self.devices[i]
            helper.ClearRow(row)
            helper.DrawTextLeft(row, device.name)
            helper.DrawTextRight(row, device.sbps, color = self.textColor)

class Handler:
    def __init__(self, meters, command):
        helper.Trace('Event handler created')
        self.meters = meters
    def Initialize(self):
        for meter in self.meters:
            meter.Initialize()
        self.Tick()
    def Tick(self):
        for meter in self.meters:
            meter.Update()
            meter.Display()

def fluxlet_main(config):
    meters = []
    command  = GetString(config, 'command')
    interval = GetInteger(config, 'interval', default = 5, min = 1)
    for meter in config.getElementsByTagName('meter'):
        type       = GetString(meter, 'type').lower()
        labelColor = GetString(meter, 'labelcolor')
        textColor  = GetString(meter, 'textcolor')
        size       = GetString(meter, 'size')
        metercmd   = GetString(meter, 'command')
        if not type:
            helper.Message(1, 'No type specified for meter, ignored.')
        elif type == 'cpu':
            meters.append(CPU(labelColor, textColor, size, command))
        elif type == 'memory':
            meters.append(Memory(labelColor, textColor, size, command))
        elif type == 'network':
            devices = []
            for deviceElem in config.getElementsByTagName('device'):
                name = deviceElem.getAttribute('name')
                if name:
                    unit = deviceElem.getAttribute('unit')
                    devices.append(NetworkDevice(name, unit))
            if len(devices) == 0:
                devices = DiscoverNetworkDevices()
            if len(devices) > 0:
                meters.append(Network(devices, labelColor, textColor, size, command))
        else:
            helper.Message(1, 'Bad meter type "%s"' % type)
    if len(meters) == 0:
        helper.Message(1, 'No meters specified - using defaults')
        meters.append(CPU('', '', '', ''))
        meters.append(Memory('', '', '', ''))
        devices = DiscoverNetworkDevices()
        meters.append(Network(devices, '', '', '', ''))
    helper.AddHandler(Handler(meters, command), interval)
