/*
 * $Id$
 * Copyright (C) 2006 Andreas Sandberg <andreas@sandberg.pp.se>
 *
 * This file is a part of PDP12-emu, a free PDP12 emulator.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "log.h"

void (*log_sink)(int, const char *, void *) = &log_sink_default;
LOG_LEVEL log_level = LOG_NORMAL;
void* log_data = NULL;


void
lprintf(LOG_LEVEL level, const char *fmt, ...) {
     if (level < log_level)
          return;

     va_list ap;
     char *msg;

     va_start(ap, fmt);

     if (vasprintf(&msg, fmt, ap) >= 0) {
          log_sink(level, msg, log_data);
          free(msg);
     }

     va_end(ap);
}

void
log_sink_default(int level, const char *msg, void *data) {
     if (level >= LOG_WARNING)
          fprintf(stderr, msg);
     else
          fprintf(stdout, msg);
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
