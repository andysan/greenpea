bin_PROGRAMS = pdp12c
noinst_PROGRAMS = paperdump

pdp12c_SOURCES = pdp12c.c 				\
	shell.c shell.h					\
	readln.h readln.c				\
	parser.c parser.h

pdp12c_LDADD =  					\
	$(top_builddir)/libpdp12/libpdp12.a 		\
	$(top_builddir)/libdecutils/libdecutils.a	\
	$(top_builddir)/liblog/liblog.a

paperdump_SOURCES = paperdump.c
paperdump_LDADD = 					\
	$(top_builddir)/libdecutils/libdecutils.a	\
	$(top_builddir)/liblog/liblog.a
