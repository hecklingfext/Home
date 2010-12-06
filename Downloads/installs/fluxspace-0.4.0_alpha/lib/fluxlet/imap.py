#!/usr/bin/env python
######################################################################
######################################################################
# Fluxlet: imap
#
# Monitors IMAP mailboxes
#
# Important note: Only use the "imap" fluxlet when you are sure that
# the access times will be very short, i.e. when it is on the local
# machine.  All other monitors will be blocked during mailbox access.
# For remote monitoring of IMAP mailboxes prefer to use the "mailimap"
# fluxfeed which runs as a background process.  Configuration is
# identical, except for the tag name "mailimap", instead of "imap".
# Feel free to test your configuration using this fluxlet and then
# switch to the background feeder by changing the start and end tags
# to "mailimap" in fluxspace.conf.
#
# Copyright (c) 2004 Steve Cooper - All rights reserved
######################################################################
######################################################################

from fluxspacecore import FluxletHelper
import imaplib
import socket
import os

helper = FluxletHelper('imap')

class Mailbox:
    def __init__(self, folder, name, size, color, command):
        self.folder  = folder
        self.name    = name
        self.row     = -1
        self.size    = size
        self.color   = color
        self.command = command
        if not self.name:
            self.name = self.folder
        # Default to normal font
        if not self.size:
            self.size = 'normal'

class Handler:
    def __init__(self, host, login, password, mailboxes, newcolor, seencolor, unseencolor, command):
        self.host         = host
        self.login        = login
        self.password     = password
        self.mailboxes    = mailboxes
        self.newcolor     = newcolor
        self.seencolor    = seencolor
        self.unseencolor  = unseencolor
        self.command      = command
        self.state        = None
        helper.Trace('Event handler created')
    def Initialize(self):
        helper.Trace('Initialize')
        for mailbox in self.mailboxes:
            mailbox.row = helper.AddRow(1, mailbox.size, mailbox.color)
            command = self.command
            if mailbox.command:
                command = mailbox.command
            helper.AddCommand(mailbox.row, mailbox.row, mailbox.name, self, command)
        self.Tick()
    def ScanMail(self):
        self.state = 'Connect to IMAP host "%s"' % self.host
        imap = imaplib.IMAP4(self.host)
        self.state = 'Log in to IMAP server "%s" with login="%s", password="%s"' % (self.host,
                                                                                    self.login,
                                                                                    self.password)
        helper.Trace('Logging in... %s/%s' % (self.login, self.password))
        imap.login(self.login, self.password)
        helper.Trace('Logged in')
        for mailbox in self.mailboxes:
            self.state = 'scan mailbox "%s"' % mailbox.name
            helper.Trace('Checking "%s" ("%s")...' % (mailbox.folder, mailbox.name))
            if mailbox.folder:
                imap.select(mailbox.folder, True)
                seen    = len(imap.search(None, 'SEEN'   )[1][0].split())
                unseen  = len(imap.search(None, 'UNSEEN' )[1][0].split())
                new     = len(imap.search(None, 'NEW'    )[1][0].split())
                deleted = len(imap.search(None, 'DELETED')[1][0].split())
                helper.Trace('"%s" seen=%d unseen=%d new=%d deleted=%d' % (mailbox.name,
                                                                           seen,
                                                                           unseen,
                                                                           new,
                                                                           deleted))
                helper.ClearRow(mailbox.row)
                helper.DrawTextLeft(mailbox.row, mailbox.name)
                if new > 0:
                    if self.newcolor:
                        helper.DrawTextRight(mailbox.row, '%d' % new, 0, self.newcolor)
                    else:
                        helper.DrawTextRight(mailbox.row, '%d*' % new, 0)
                elif unseen > 0:
                    if self.unseencolor:
                        helper.DrawTextRight(mailbox.row, '%d' % unseen, 0, self.unseencolor)
                    else:
                        helper.DrawTextRight(mailbox.row, '%d+' % unseen, 0)
                else:
                    if seen - deleted > 0:
                        scount = '%d' % (seen - deleted)
                    else:
                        scount = '--'
                    if self.seencolor:
                        helper.DrawTextRight(mailbox.row, scount, 0, self.seencolor)
                    else:
                        helper.DrawTextRight(mailbox.row, scount, 0)
        helper.Trace('Logging out...')
        self.state = 'log out of IMAP server'
        imap.logout()
        helper.Trace('Logged out')
    def DisplayError(self):
        for mailbox in self.mailboxes:
            helper.ClearRow(mailbox.row)
            helper.DrawTextLeft(mailbox.row, mailbox.name)
            helper.DrawTextRight(mailbox.row, '**')
    def Tick(self):
        if len(self.mailboxes) == 0:
            return
        try:
            self.ScanMail()
        except socket.error, e:
            helper.Message(2, 'Socket error: %s\n   Action: %s' % (e, self.state))
            self.DisplayError()
        except imaplib.IMAP4.error, e:
            helper.Message(2, 'IMAPLIB error: %s\n   Action: %s' % (e, self.state))
            self.DisplayError()
    def Command(self, command, button, dataCommand):
        if dataCommand:
            if os.fork() == 0:
                os.execlp('sh', 'sh', '-c', dataCommand)

def fluxlet_main(config):
    host        = config.getAttribute('host')
    login       = config.getAttribute('login')
    password    = config.getAttribute('password')
    newcolor    = config.getAttribute('newcolor')
    seencolor   = config.getAttribute('seencolor')
    unseencolor = config.getAttribute('unseencolor')
    command     = config.getAttribute('command')
    mailboxes = []
    for mailboxNode in config.getElementsByTagName('mailbox'):
        folder = mailboxNode.getAttribute('folder')
        name   = mailboxNode.getAttribute('name')
        size   = mailboxNode.getAttribute('size')
        color  = mailboxNode.getAttribute('color')
        mbcmd  = mailboxNode.getAttribute('command')
        mailboxes.append(Mailbox(folder, name, size, color, mbcmd))
    handler = Handler(host, login, password, mailboxes, newcolor, seencolor, unseencolor, command)
    helper.AddHandler(handler, 30)
