fluxletpythondir = $(pkgdatadir)/lib/fluxlet
fluxletpython_PYTHON = __init__.py \
			painter.py \
			runner.py \
			rox.py \
			tracer.py \
			idesk.py \
			imap.py \
			clock.py \
            blank.py \
			display.py \
			system.py

CLEANFILES = *.pyc

uninstall-hook:
	if [ -d $(fluxletpythondir) ]; then rmdir $(fluxletpythondir); fi

# vim: noet:
