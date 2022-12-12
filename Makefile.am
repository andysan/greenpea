#
# SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
#
# SPDX-License-Identifier: BSD-3-Clause
#
ACLOCAL_FLAGS = -I m4
SUBDIRS = libpdp12 libpdp12utils frontend
EXTRA_DIST = pdp12-emu.spec

rpm: dist-gzip
	rpmbuild -ta $(distdir).tar.gz
