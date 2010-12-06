#####################################################################
#####################################################################
# fluxspace.process - Fluxspace process management module
#####################################################################
#####################################################################

import sys
import os
import os.path
import re
import stat
import string
import signal
import socket
import select
import time

from SOAPpy import SOAPServer, SOAPProxy

from fluxspace.config import Config

(myDir, myName) = os.path.split(os.path.realpath(sys.argv[0]))
libDir           = os.path.join(myDir, 'lib')
fluxfeedDir      = os.path.join(myDir, 'fluxfeed')
userFluxspaceDir = os.path.expanduser('~/.fluxspace')
userFluxboxDir   = os.path.expanduser('~/.fluxbox')
userLibDir       = os.path.join(userFluxspaceDir, 'lib')
userCacheDir     = os.path.join(userFluxspaceDir, 'cache')
userFluxfeedDir  = os.path.join(userFluxspaceDir, 'fluxfeed')
userFluxletDir   = os.path.join(userLibDir, 'fluxlet')
libDirs          = (userLibDir, libDir)
fluxfeedDirs     = (userFluxfeedDir, fluxfeedDir)

# Prepend ~/.fluxspace/lib and <myDir>/lib to Python module path.
# Enable running both from a self-contained hierarchy and also from
# the build directory.
sys.path[0:0] = libDirs

# Add the directory of this script to the execution path.
os.environ['PATH'] = '%s:%s' % (myDir, os.environ['PATH'])

# Add the library directories to PYTHONPATH so that spawned fluxfeeds
# will find Fluxspace modules.
pythonpathDirs = list(libDirs)
pythonpath = os.environ.get('PYTHONPATH', '')
if len(pythonpath) > 0:
    pythonpathDirs.extend(pythonpath.split(':'))
os.environ['PYTHONPATH'] = string.join(pythonpathDirs, ':')

pid = os.getpid()

reps = re.compile('^[ \t]*([0-9]+)[ \t]+([0-9]+)[ \t]+(.+)[ \t\r\n]*$')

reCommandArg = re.compile('[ \t]*([^ \t"]+)|"([^"]*)"')
reNumDir = re.compile('^[0-9]+$')
rePPID   = re.compile('^PPid:[ \t]+([0-9]+)[ \t]*$')

def sigintHandler(signum, frame):
    print '<%s interrupted - exiting>' % myName
    sys.exit(0)

######################################################################
class RunningInstanceBase:
# Tracks current Fluxspace instances through cached files.
# Files are ~/.fluxspace/fsi<display#>
# Each file has the display #, PID, and TCP port (for SOAP).
######################################################################
    def __init__(self, display, pid, port):
        self.display = display
        self.file    = os.path.expanduser('~/.fluxspace/cache/fsi%s' % self.display)
        self.pid     = pid
        self.host    = 'localhost'  # hard-coded for now
        self.port    = port
        # If we're given the info write it out
        if self.pid:
            self.Save()
        # Otherwise try to load it
        else:
            self.Load()
    def Save(self):
        f = open(self.file, 'w')
        f.write('%s %d %d' % (self.display, self.pid, self.port))
        f.close()
    def Load(self):
        if os.path.isfile(self.file):
            f = open(self.file)
            s = f.read()
            f.close()
            fields = s.split()
            self.pid  = int(fields[1])
            self.port = int(fields[2])
        else:
            self.pid  = self.port = 0
    def IsAlive(self):
        alive = False
        if self.pid != 0:
            try:
                os.getpgid(self.pid)
                alive = True
            except OSError:
                pass
        return alive

######################################################################
class FluxspaceInstance(RunningInstanceBase):
######################################################################
    def __init__(self, display = None, port = None):
        # Create a SOAP server
        # May need to search for an open port
        self.soapServer = None
        if display is None:
            display = os.environ["DISPLAY"]
        if port is None:
            port = 8888     # Arbitrary choice of starting port #
        while self.soapServer is None:
            try:
                self.soapServer = SOAPServer(('localhost', port))
            except socket.error, e:
                port += 1
                print 'Trying port %d...' % port
        RunningInstanceBase.__init__(self, display, pid, port)
    def SOAPPoll(self):
        # Handle all pending requests
        if self.soapServer is not None:
            timeout = 0.0   # Start by just polling
            while len(select.select([self.soapServer.fileno()], [], [], timeout)[0]) > 0:
                self.soapServer.handle_request()
                timeout = 0.5   # Give a series of calls more chance to come through
    def RegisterSOAPObject(self, object, ns):
        if self.soapServer is None:
            print 'ERROR: SOAP Server is unavailable.  Unable to register "%s"' % ns
            return False
        self.soapServer.registerObject(object, ns)
        return True

######################################################################
class FluxspaceProxy(RunningInstanceBase):
######################################################################

    def __init__(self, display = None):

        # Force a clean exit when interrupted
        signal.signal(signal.SIGINT, sigintHandler)
        if display is None:
            display = os.environ["DISPLAY"]
        RunningInstanceBase.__init__(self, display, 0, 0)
        self.soapProxy = None
        if self.host and self.port:
            # Check for a web proxy definition in environment
            try:
                host, port = re.search('http://([^:]+):([0-9]+)',
                                       os.environ['http_proxy']).group(1,2)
                httpProxy = '%s:%s' % (host, port)
            except KeyError:
                httpProxy = None
            url = 'http://%s:%d/' % (self.host, self.port)
            self.soapProxy = SOAPProxy(url, httpProxy)

    def GetSOAPObject(self, namespace):
        ns = 'urn:fluxspace.%s' % namespace
        if self.soapProxy is not None:
            return self.soapProxy._ns('NS1', ns)
        return None

    def Kill(self):
        if self.pid == 0:
            return False
        process = Process(self.pid)
        process.Kill(True)
        return True

######################################################################
class FluxfeedProxy(FluxspaceProxy):
######################################################################

    def __init__(self, moduleName, display = None):

        FluxspaceProxy.__init__(self, display)

        # Load the fluxspace configuration
        self.fluxspaceConfig = Config()
        self.fluxspaceConfig.Load()

        # Gather module configurations
        self.moduleName    = moduleName
        self.moduleConfigs = self.fluxspaceConfig.GetModuleConfigurations(self.moduleName)
        if len(self.moduleConfigs) == 0:
            print 'No "%s" configurations are enabled' % self.moduleName
            sys.exit(0)

        # Assume we need the display server
        self.display = self.GetSOAPObject('display')

    def UpdateForever(self, updates):
        # Update all configurations
        pendingUpdates = updates
        nextUpdateTime = 0.0

        # Main loop retrieves quotes and sleeps
        while True:

            # Update the IMAP configurations that are ready
            for pendingUpdate in pendingUpdates:
                pendingUpdate.UpdateTime()
                pendingUpdate.Update(self.display)

            # Determine what updates next
            pendingUpdates = []
            for update in updates:
                if len(pendingUpdates) == 0:
                    pendingUpdates.append(update)
                elif config.nextUpdateTime == pendingUpdates[0].nextUpdateTime:
                    pendingUpdates.append(update)
                elif config.nextUpdateTime < pendingUpdates[0].nextUpdateTime:
                    pendingUpdates = [update]

            # Take a nap
            # All configs awaiting update have the same update time.
            # Grab the time from the first one.
            sleepAmt = pendingUpdates[0].nextUpdateTime - time.time()
            if sleepAmt <= 0.0:     # CYA
                sleepAmt = 1.0
            time.sleep(sleepAmt)

######################################################################
class FluxfeedUpdateBase:
# Base class for fluxfeed objects that handle updates based on an
# individual configuration
######################################################################
    def __init__(self, config, display, rows, label):
        # Just get the universal configuration attributes
        self.label     = label
        self.rows      = rows
        self.command   = config.getAttribute('command')
        self.sinterval = config.getAttribute('interval')
        self.interval  = 30.0     # default interval is 30 seconds
        if self.sinterval:
            try:
                self.interval = float(self.sinterval)
                # Protect from bad interval values
                if self.interval <= 10.0:
                    self.interval = 10.0
            except ValueError:
                pass

        # Prepare the display area
        self.firstRow = display.AddRows(self.rows)

        # If no individual commands are present use one button if specified.
        lastRow = self.firstRow + len(rows) - 1
        if display.GetCommandCount() == 0 and self.label and self.command:
            display.AddCommand(self.firstRow, lastRow, self.label, self.command)

        # An immediate update will occur
        self.nextUpdateTime = 0.0

    def UpdateTime(self):
        self.nextUpdateTime = time.time() + self.interval

######################################################################
def GetProcessPath(pid = 0, file = ''):
######################################################################
	path = '/proc'
	if pid:
		path = path + ( '/%s' % pid )
	if file:
		path = path + ( '/%s' % file )
	return path

######################################################################
def GetUserPIDs(matchUser):
######################################################################
    pids = []
    for subdir in os.listdir(GetProcessPath()):
        procdir = GetProcessPath(subdir)
        statInfo = os.stat(procdir)
        mode  = statInfo[stat.ST_MODE]
        isDir = stat.S_ISDIR(mode)
        user  = statInfo[stat.ST_UID]
        if isDir and reNumDir.search(subdir) != None:
            if user == matchUser:
                pids.append(int(subdir))
    return pids

######################################################################
def GetProcessCommand(pid):
######################################################################
	cmdlinefile = GetProcessPath(pid, 'cmdline')
	cmdline = open(cmdlinefile, 'r').readline()
	chunks = string.split(cmdline, '\0')
	return chunks

######################################################################
def GetProcessUser(pid):
######################################################################
    return os.stat(GetProcessPath(pid))[stat.ST_UID]

######################################################################
def GetProcessParent(pid):
######################################################################
    parent = 0
    for line in open(GetProcessPath(pid, 'status')).readlines():
        matchPPID = rePPID.match(line)
        if matchPPID != None:
            parent = int(matchPPID.group(1))
    return parent

######################################################################
class Process:
######################################################################

    def __init__(self, pid):
        self.pid      = pid
        self.uid      = GetProcessUser(pid)
        self.ppid     = GetProcessParent(pid)
        self.parent   = None
        self.children = []

        cmdParts  = self.GetProcessCommand()
        args = ""
        for i in range(1, len(cmdParts)):
            if i > 0:
                args = args + " "
            args = args + cmdParts[i]

        self.command  = cmdParts[0]
        self.args     = args
        pathParts = string.split(self.command, '/')
        self.commandName = pathParts[len(pathParts)-1]

        self.GetStatus()

    def GetStatus(self):
        self.name  = '???'
        statusfile = self.GetPath('status')
        for line in open(statusfile).readlines():
            fields = line.split()
            if fields[0].lower() == 'name:':
                self.name = fields[1]

    def GetProcessCommand(self):
        cmdlinefile = self.GetPath('cmdline')
        cmdline = open(cmdlinefile, 'r').readline()
        chunks = string.split(cmdline, '\0')
        return chunks

    def GetPath(self, file = ''):
        return GetProcessPath(self.pid, file)

    def AddChild(self, child):
        self.children.append(child)
        child.parent = self

    def Kill(self, children = False):
        if children:
            for child in self.children:
                child.Kill(1)
        os.kill(self.pid, signal.SIGINT)

    def IsCurrent(self):
        if self.pid == os.getpid():
            return 1
        if self.parent is not None:
            return self.parent.IsCurrent()
        return 0

######################################################################
class Processes:
######################################################################
    def __init__(self):
        self.thisUID = os.getuid()
        self.Refresh()

    def Refresh(self):
        self.byPID   = {}
        self.byName  = {}
        self.ordered = []
        self.tree    = []
        # Create the flat list of processes
        # Also build the name dictionary
        for pid in GetUserPIDs(self.thisUID):
            newProcess = Process(pid)
            self.byPID[pid] = newProcess
            self.ordered.append(newProcess)
            if not self.byName.has_key(newProcess.name):
                self.byName[newProcess.name] = []
            self.byName[newProcess.name].append(newProcess)
        # Build the tree structure
        for process in self.ordered:
            if self.byPID.has_key(process.ppid):
                self.byPID[process.ppid].AddChild(process)
            else:
                 self.tree.append(process)

    def Find(self, name):
        processes = []
        if self.byName.has_key(name):
            for process in self.byName[name]:
                if not process.IsCurrent():
                    processes.append(process)
        return processes
        
    def KillByName(self, name, children = 0):
        for process in self.Find(name):
            if not process.IsCurrent():
                process.Kill(1)

######################################################################
def GetArgs(command):
######################################################################
    args = []
    s = command
    m = reCommandArg.search(s)
    while m is not None:
        arg = m.group(1)
        pos = m.end(1)
        if arg is None:
            arg = m.group(2)
            pos = m.end(2)
        args.append(os.path.expanduser(arg))
        m = reCommandArg.search(s, pos+1)
    return args

######################################################################
class Applet:
######################################################################
    def __init__(self, helper, action, command):
        self.helper  = helper
        self.action  = action
        args = GetArgs(command)
        self.command = args[0]
        self.args    = args
        self.pid     = 0
    def Start(self):
        self.helper.Trace('Start %s %s' % (self.command, self.args))
        self.helper.Trace('pid=%d' % self.pid)
        if self.pid == 0 or self.action == 'run':
            self.pid = os.fork()
            if self.pid == 0:
                try:
                    os.execvp(self.command, self.args)
                except:
                    self.helper.Message(2, 'Unable to start "%s"' % self.command)
                    sys.exit(1)
    def Stop(self):
        if self.pid != 0 and self.action == 'manage':
            self.helper.Trace('Stop %d (%s %s)' % (self.pid, self.command, self.args))
            os.system('kill %d' % (self.pid))
            self.pid = 0
    def Restart(self):
        self.Stop()
        self.Start()

