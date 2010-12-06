# Nothing to build here
EXTRA_DIST = index.html \
			 workspace0-big.png \
			 workspace0-full.png \
			 workspace0-small.png \
			 workspace1-big.png \
			 workspace1-full.png \
			 workspace1-small.png

docdir = $(pkgdatadir)/html
doc_DATA = $(EXTRA_DIST)

uninstall-hook:
	rmdir $(docdir) || echo "Leaving \"$(docdir)\" since it's not empty"
