fluxfeeddir = $(pkgdatadir)/fluxfeed
fluxfeed_SCRIPTS = mailimap quote

EXTRA_DIST = $(fluxfeed_SCRIPTS)

uninstall-hook:
	if [ -d $(fluxfeeddir) ]; then rmdir $(fluxfeeddir); fi

# vim: noet:
