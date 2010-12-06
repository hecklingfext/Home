"""
GSIServer - Contributed by Ivan R. Judson <judson@mcs.anl.gov>


################################################################################
#
# SOAPpy - Cayce Ullman       (cayce@actzero.com)
#          Brian Matthews     (blm@actzero.com)
#          Gregory Warnes     (gregory_r_warnes@groton.pfizer.com)
#          Christopher Blunck (blunck@gst.com)
#
################################################################################
# Copyright (c) 2003, Pfizer
# Copyright (c) 2001, Cayce Ullman.
# Copyright (c) 2001, Brian Matthews.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# Neither the name of actzero, inc. nor the names of its contributors may
# be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
################################################################################
"""

from __future__ import nested_scopes

#import xml.sax
import re
import socket
import sys
import SocketServer
from types import *
import BaseHTTPServer

# SOAPpy modules
from Parser      import parseSOAPRPC
from Config      import Config
from Types       import faultType, voidType, simplify
from NS          import NS
from SOAPBuilder import buildSOAP
from Utilities   import debugHeader, debugFooter

try: from M2Crypto import SSL
except: pass

ident = '$Id: GSIServer.py,v 1.1 2003/12/18 22:40:57 warnes Exp $'

from version import __version__

#####

from Server import *

from pyGlobus.io import GSITCPSocketServer, ThreadingGSITCPSocketServer
from pyGlobus import ioc

Config.channel_mode = ioc.GLOBUS_IO_SECURE_CHANNEL_MODE_GSI_WRAP
Config.delegation_mode = ioc.GLOBUS_IO_SECURE_DELEGATION_MODE_FULL_PROXY
Config.tcpAttr = None
Config.authMethod = "_authorize"

class GSISOAPServer(GSITCPSocketServer, SOAPServerBase):
    def __init__(self, addr = ('localhost', 8000),
                 RequestHandler = SOAPRequestHandler, log = 1,
                 encoding = 'UTF-8', config = Config, namespace = None):

        # Test the encoding, raising an exception if it's not known
        if encoding != None:
            ''.encode(encoding)

        self.namespace          = namespace
        self.objmap             = {}
        self.funcmap            = {}
        self.encoding           = encoding
        self.config             = config
        self.log                = log
        
        self.allow_reuse_address= 1
        
        GSITCPSocketServer.__init__(self, addr, RequestHandler,
                                    self.config.channel_mode,
                                    self.config.delegation_mode,
                                    tcpAttr = self.config.tcpAttr)
        
    def get_request(self):
        sock, addr = GSITCPSocketServer.get_request(self)

        return sock, addr
       
class ThreadingGSISOAPServer(ThreadingGSITCPSocketServer, SOAPServerBase):

    def __init__(self, addr = ('localhost', 8000),
                 RequestHandler = SOAPRequestHandler, log = 1,
                 encoding = 'UTF-8', config = Config, namespace = None):
        
        # Test the encoding, raising an exception if it's not known
        if encoding != None:
            ''.encode(encoding)

        self.namespace          = namespace
        self.objmap             = {}
        self.funcmap            = {}
        self.encoding           = encoding
        self.config             = config
        self.log                = log
        
        self.allow_reuse_address= 1
        
        ThreadingGSITCPSocketServer.__init__(self, addr, RequestHandler,
                                             self.config.channel_mode,
                                             self.config.delegation_mode,
                                             tcpAttr = self.config.tcpAttr)

    def get_request(self):
        sock, addr = ThreadingGSITCPSocketServer.get_request(self)

        return sock, addr
