SUBDIRS= FbTk

INCLUDES=$(PYTHON_INCLUDES)
cxxwrapper = fluxspacecore_wrap.cxx
pywrapper = fluxspacecore.py

MODULE_LIBRARY = libfluxspace-$(VERSION).so

libdir = $(pkgdatadir)/lib
lib_LTLIBRARIES = libfluxspace.la
libfluxspace_la_LIBADD = FbTk/libFbTk.la
libfluxspace_la_LDFLAGS = -release $(VERSION)
libfluxspace_la_SOURCES =	BaseDisplay.cc BaseDisplay.hh \
							FluxspaceDisplay.cc FluxspaceDisplay.hh \
							FluxspaceRenderer.cc FluxspaceRenderer.hh \
							FluxspaceTheme.cc FluxspaceTheme.hh \
							LinkedList.cc LinkedList.hh \
							OptionalData.hh \
							MyPy.cc MyPy.hh \
							PythonInterface.cc PythonInterface.hh \
							FluxspaceInterface.cc FluxspaceInterface.hh \
							Esetroot.cc Esetroot.hh \
							fluxspacecore.i fluxspacecore.cc fluxspacecore.hh \
							FluxboxUtility.cc FluxboxUtility.hh \
                            BitMask.cc BitMask.hh \
							$(cxxwrapper)

SWIG_DIST = $(cxxwrapper) $(pywrapper)
MAINTAINERCLEANFILES = Makefile.in $(SWIG_DIST)
CLEANFILES = *.pyc

modulespythondir = $(libdir)
modulespython_PYTHON = fluxspacecore.py

PYTHON_MODULE = _fluxspacecore.so
PYTHON_MODULE_PATH = $(modulespythondir)/$(PYTHON_MODULE)

PythonInterface.cc: $(cxxwrapper)

$(cxxwrapper) $(pywrapper) fluxspacecore.i: fluxspacecore.hh
	@SWIG@ -c++ -python -keyword -shadow fluxspacecore.i

install-exec-hook:
	(cd $(modulespythondir); ln -sf $(MODULE_LIBRARY) $(PYTHON_MODULE))

uninstall-hook:
	rm $(PYTHON_MODULE_PATH) || echo "Did not remove \"$(PYTHON_MODULE_PATH)\"" 

swig-clean: clean
	rm $(SWIG_DIST)

# vim: noet
