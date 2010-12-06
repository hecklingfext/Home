fluxspacepythondir = $(pkgdatadir)/lib/fluxspace
fluxspacepython_PYTHON = __init__.py ideskutil.py config.py process.py
fluxspacepython_DATA = version.py
noinst_DATA = version.in
EXTRA_DIST = version.in

CLEANFILES = *.pyc version.py

version.py: version.in ../../version.h
	cat version.in > version.py
	echo version = \"$(VERSION)\" >> version.py

install-data-hook:
	$(py_compile) "$(fluxspacepythondir)/version.py"

uninstall-hook:
	if [ -d $(fluxspacepythondir) ]; then rmdir $(fluxspacepythondir); fi

# vim: noet
