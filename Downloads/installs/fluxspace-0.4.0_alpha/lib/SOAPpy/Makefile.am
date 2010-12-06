SUBDIRS= wstools

soappypythondir = $(pkgdatadir)/lib/SOAPpy
soappypython_PYTHON = \
	__init__.py \
	fpconst.py \
	Client.py \
	Config.py \
	Errors.py \
	GSIServer.py \
	NS.py \
	Parser.py \
	SOAP.py \
	SOAPBuilder.py \
	Server.py \
	Types.py \
	URLopener.py \
	Utilities.py \
	WSDL.py \
	version.py

CLEANFILES = *.pyc

uninstall-hook:
	if [ -d $(soappypythondir) ]; then rmdir $(soappypythondir); fi

# vim: : noet
