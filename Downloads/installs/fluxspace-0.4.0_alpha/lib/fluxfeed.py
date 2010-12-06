#####################################################################
#####################################################################
# fluxfeed - Fluxspace support module for fluxfeeds
#####################################################################
#####################################################################

import sys
import time
from fluxspace.process import FluxspaceProxy
from fluxspace.config import Config

######################################################################
class Feed(FluxspaceProxy):
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
class Updater:
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
