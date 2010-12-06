wstoolspythondir = $(pkgdatadir)/lib/SOAPpy/wstools
wstoolspython_PYTHON = \
	__init__.py \
	TimeoutSocket.py \
	UserTuple.py \
	Utility.py \
	WSDLTools.py \
	XMLSchema.py \
	XMLname.py

CLEANFILES = *.pyc

uninstall-hook:
	if [ -d $(wstoolspythondir) ]; then rmdir $(wstoolspythondir); fi

# vim: noet
