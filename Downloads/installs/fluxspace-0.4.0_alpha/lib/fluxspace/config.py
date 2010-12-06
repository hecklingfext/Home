#####################################################################
#####################################################################
# fluxspace.config - Fluxspace configuration management module
#####################################################################
#####################################################################

import sys
import os.path
import xml.dom.minidom

# Where to find fluxspace configuration files
defaultConfigPath = [os.path.expanduser('~/.fluxspace')]
defaultConfigFileName = 'fluxspace.conf'
tracer = None

class ConfigException(Exception):
    def __init__(self, message):
        self.message = message

class ConfigBadRoot(ConfigException):
    pass

class ConfigNoneFound(ConfigException):
    pass

class ConfigFormatError(ConfigException):
    def __init__(self, message, xmlException):
        fullMessage = '%s\n\nXML Error: %s' % (message, str(xmlException))
        ConfigException.__init__(self, fullMessage)

def SetTracer(tracer_):
    global tracer
    tracer = tracer_

######################################################################
class Module:
######################################################################
    def __init__(self, name, config):
        self.name   = name
        self.config = config

######################################################################
class Config:
######################################################################
    def __init__(self):
        global defaultConfigPath
        self.configPath = defaultConfigPath
        self.Reset()

    def Reset(self):
        self.configDOM = None
        self.root      = None
        self.modules   = []

    def Close(self):
        if self.configDOM is not None:
            self.configDOM.unlink()
            self.configDOM = None

    def Load(self, fileName = defaultConfigFileName):
        for configPathSegment in self.configPath:
            path = os.path.join(configPathSegment, fileName)
            if os.path.isfile(path):
                if tracer is not None:
                    tracer.Trace('Reading configuration "%s"...' % path)
                try:
                    self.configDOM = xml.dom.minidom.parse(path)
                except xml.parsers.expat.ExpatError, xmlError:
                    raise ConfigFormatError('Configuration file format error', xmlError )
                break

        if self.configDOM is None:
            raise ConfigNoneFound('No configuration loaded')

        self.root = self.configDOM.getElementsByTagName('fluxspace')
        if len(self.root) != 1:
            raise ConfigBadRoot('Bad or missing fluxspace root element')

        for config in self.root[0].childNodes:
            if not config.nodeType == config.ELEMENT_NODE:
                continue
            enable = config.getAttribute('enable')
            if enable and enable.lower() != 'yes':
                continue
            self.modules.append(Module(config.tagName, config))

    def GetModuleConfigurations(self, name):
        configs = []
        for module in self.modules:
            if module.name.lower() == name.lower():
                configs.append(module.config)
        return configs

######################################################################
def GetString(node, name, default = ''):
######################################################################
    s = node.getAttribute(name)
    if not s:
        s = default
    return s

######################################################################
def GetInteger(node, name, default = 0, min = 0, max = sys.maxint):
######################################################################
    s = node.getAttribute(name)
    if s == '':
        return default
    try:
        i = int(s)
        if i < min:
            i = min
        if i > max:
            i = max
    except ValueError:
        i = default
    return i
