bin_PROGRAMS = pdp12c
noinst_PROGRAMS = paperdump

pdp12c_SOURCES = pdp12c.c 				\
	shell.c shell.h					\
	readln.h readln.c				\
	parser.c parser.h

pdp12c_LDADD =  					\
	$(top_builddir)/libpdp12/libpdp12.a 		\
	$(top_builddir)/libpdp12utils/libpdp12utils.a

pdp12c_LDFLAGS=$(AM_LDFLAGS) $(SDL_LIBS)
pdp12c_CFLAGS=$(AM_CFLAGS) $(SDL_CFLAGS)

paperdump_SOURCES = paperdump.c
paperdump_LDADD = 					\
	$(top_builddir)/libpdp12utils/libpdp12utils.a
