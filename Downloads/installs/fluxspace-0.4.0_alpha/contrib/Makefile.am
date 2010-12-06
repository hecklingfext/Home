contribdir = $(pkgdatadir)/contrib
contrib_DATA = randpaint.py randpaint-recursive.patch
EXTRA_DIST = randpaint.py randpaint-recursive.patch

uninstall-hook:
	rmdir $(contribdir) || echo "Leaving \"$(contribdir)\" since it's not empty"
