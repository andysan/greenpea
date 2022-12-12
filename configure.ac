AC_INIT([greenpea],[0.1])
AM_INIT_AUTOMAKE

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_CHECK_HEADERS([argp.h], [
  AC_SEARCH_LIBS([argp_parse], [argp], [
    have_argp=true
  ], [
    have_argp=false
  ])
], [
  have_argp=false
])

AM_CONDITIONAL([HAVE_ARGP], $have_argp)
if ! $have_argp; then
    AC_MSG_WARN([Working argp implementation not found, disabling command line tools.])
fi

AC_ARG_ENABLE(readline,
		AS_HELP_STRING([--disable-readline], [disables the use of GNU Readline]))

AC_ARG_ENABLE(sdl,
		AS_HELP_STRING([--disable-sdl], [disables the use of SDL]))

if test x$enable_sdl != xno; then
   dnl Check for SDL
   SDL_VERSION=1.2.0
   AM_PATH_SDL($SDL_VERSION,
               [ AC_MSG_NOTICE([SDL found, enabling screen emulation.]) ]
	       [ AC_DEFINE([HAVE_SDL], [], [SDL is installed and enabled.]) ],
	       [ AC_MSG_WARN([*** SDL version $SDL_VERSION not found!]) ]
	      )
fi

if test x$enable_readline != xno; then
   VL_LIB_READLINE
fi

CFLAGS="${CFLAGS} -Wall -Werror"

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile \
		pdp12-emu.spec \
		libpdp12/Makefile \
		libpdp12utils/Makefile \
		frontend/Makefile \
		frontend/console/Makefile])
AC_OUTPUT
