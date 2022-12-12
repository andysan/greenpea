#
# SPDX-FileCopyrightText: Copyright 2006-2007, 2022 Andreas Sandberg <andreas@sandberg.uk>
#
# SPDX-License-Identifier: BSD-3-Clause
#
bin_PROGRAMS =
noinst_PROGRAMS =

if HAVE_ARGP
  bin_PROGRAMS += pdp12c
  noinst_PROGRAMS += paperdump
endif

pdp12c_SOURCES =			\
	shell.c		shell.h		\
	readln.h	readln.c	\
	parser.c	parser.h	\
	pdp12c.c

pdp12c_LDADD =  					\
	$(top_builddir)/libpdp12/libpdp12.a 		\
	$(top_builddir)/libpdp12utils/libpdp12utils.a

pdp12c_LDFLAGS = $(AM_LDFLAGS) $(SDL_LIBS)
pdp12c_CFLAGS =  -I$(top_srcdir) $(AM_CFLAGS) $(SDL_CFLAGS)

paperdump_SOURCES = paperdump.c
paperdump_LDADD = 					\
	$(top_builddir)/libpdp12utils/libpdp12utils.a
paperdump_CFLAGS = -I$(top_srcdir) $(AM_CFLAGS)
