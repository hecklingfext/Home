SUBDIRS = src lib fluxfeed doc tools examples contrib test .

textdir = $(pkgdatadir)
text_DATA = AUTHORS BUGS ChangeLog COPYING NEWS README

PYSCRIPTS = fluxspace

AUX_DIST = config.guess config.log config.sub depcomp install-sh libtool ltmain.sh \
	missing mkinstalldirs py-compile python-needs version.h
EXTRA_DIST = bootstrap AUTHORS BUGS COPYING ChangeLog NEWS README TODO \
	xft.m4 \
	$(PYSCRIPTS)
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 build-stamp configure config-h.in \
	install-stamp stamp-h.in $(AUX_DIST)

scriptdir = $(pkgdatadir)
script_SCRIPTS = $(PYSCRIPTS)

install-exec-hook:
	mkdir -p -- @prefix@/bin || echo "\"@prefix@/bin\" exists"
	for f in $(PYSCRIPTS); do \
		rm -fv @prefix@/bin/$$f 2> /dev/null; \
		ln -s $(pkgdatadir)/$$f @prefix@/bin/; \
	done

uninstall-local:
	rm $(pkgdatadir)/lib/*.pyc || echo "No compiled Python files to remove"

uninstall-hook:
	rmdir $(pkgdatadir)/lib || echo "Not removing \"$(pkgdatadir)/lib\" since it's not empty"
	rmdir $(pkgdatadir) || echo "Not removing \"$(pkgdatadir)\" since it's not empty"
	for f in $(PYSCRIPTS); do \
		rm -fv @prefix@/bin/$$f || echo "\"@prefix@/bin/$$f\" was not found for removal"; \
	done
	rmdir @prefix@/bin || echo "Not removing \"@prefix@/bin\" since it's not empty"

# vim: noet:
