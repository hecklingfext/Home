"""Provide a class for loading data from URL's that handles basic
authentication"""

ident = '$Id: URLopener.py,v 1.1 2003/07/18 15:58:28 warnes Exp $'

from Config import Config
from urllib import FancyURLopener

class URLopener(FancyURLopener):

    username = None
    passwd = None


    def __init__(self, username=None, passwd=None, *args, **kw):
        FancyURLopener.__init__( self, *args, **kw)
        self.username = username
        self.passwd = passwd


    def prompt_user_passwd(self, host, realm):
       return self.username, self.passwd
