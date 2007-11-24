ACLOCAL_FLAGS = -I m4
SUBDIRS = libpdp12 libpdp12utils frontend
EXTRA_DIST = pdp12-emu.spec

rpm: dist-gzip
	rpmbuild -ta $(distdir).tar.gz
