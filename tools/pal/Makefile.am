SUBDIRS = . test

bin_PROGRAMS = pal

CPPFLAGS=-I$(abs_srcdir) -I$(abs_top_srcdir)

EXTRA_DIST=c99.l c99.y

pal_SOURCES = \
	asl.c asl.h \
	pal.c pal.h \
	pdp8.c pdp8.h \
	symbols.c symbols.h

nodist_pal_SOURCES = \
	pal_l.c pal_l.h pal_y.c pal_y.h

BUILT_SOURCES = \
	pal_l.c pal_l.h pal_y.c pal_y.h

CLEANFILES = \
	pal_l.c pal_l.h pal_y.c pal_y.h

pal_l.c pal_l.h: pal.l Makefile
	flex $<

pal_y.c: pal.y Makefile
	bison $<
