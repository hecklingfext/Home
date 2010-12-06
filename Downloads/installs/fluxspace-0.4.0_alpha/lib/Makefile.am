SUBDIRS= fluxlet fluxspace SOAPpy

# Grab the core modules (python and binary) just for testing
CORE_MODULE_LIBRARY = _fluxspacecore.so
SRC_CORE_MODULE_LIBRARY = ../src/.libs/libfluxspace-$(VERSION).so
CORE_MODULE = fluxspacecore.py
SRC_CORE_MODULE = ../src/$(CORE_MODULE)

libpythondir = $(pkgdatadir)/lib
libpython_PYTHON = fluxfeed.py soapdispenser.py

CLEANFILES = $(CORE_MODULE) *.pyc $(CORE_MODULE_LIBRARY)

# noinst = don't install them
noinst_DATA = $(CORE_MODULE_LIBRARY) $(CORE_MODULE)

$(CORE_MODULE_LIBRARY): $(SRC_CORE_MODULE_LIBRARY)
	cp $(SRC_CORE_MODULE_LIBRARY) $(CORE_MODULE_LIBRARY)

$(CORE_MODULE): $(SRC_CORE_MODULE)
	cp $(SRC_CORE_MODULE) $(CORE_MODULE)

uninstall-hook:
	if [ -d $(libpythondir) ]; then rmdir $(libpythondir); fi

# vim: noet
