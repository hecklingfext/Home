# Nothing to build here
EXTRA_DIST = fluxspace-add-idesk-icon \
	fluxspace-sync-idesk-with-appdirs \
	fluxspace-add-rox-app

toolsdir = $(pkgdatadir)/tools
tools_SCRIPTS = $(EXTRA_DIST)

uninstall-hook:
	rmdir $(toolsdir) || echo "Leaving \"$(toolsdir)\" since it's not empty"
