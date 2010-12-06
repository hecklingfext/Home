exampledir = $(pkgdatadir)/examples
example_DATA = fluxspace.conf
EXTRA_DIST = fluxspace.conf

uninstall-hook:
	rmdir $(exampledir) || echo "Leaving \"$(exampledir)\" since it's not empty"
