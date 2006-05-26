SUBDIRS = libpdp12 libdecutils liblog frontend
EXTRA_DIST = pdp12-emu.spec

rpm: dist-gzip
	rpmbuild -ta $(distdir).tar.gz
