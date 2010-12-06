#!/usr/bin/env python
# Copyright (c) 2004 Steve Cooper

import os
import re
from SOAPpy import SOAPProxy

class Dispenser(SOAPProxy):
    def __init__(self, url, namespaceIn = None):
        # Check for a web proxy definition in environment
        try:
            host, port = re.search('http://([^:]+):([0-9]+)', os.environ['http_proxy']).group(1,2)
            proxy = '%s:%s' % (host, port)
        except KeyError:
            proxy = None
        SOAPProxy.__init__(self, url, namespace = namespaceIn, http_proxy = proxy)
    def getObject(self, namespace):
        return self._ns('ns1', namespace)
