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

#ifndef _LOG_H
#define _LOG_H

typedef enum {
     LOG_DEBUG = -20,
     LOG_VERBOSE = -10,
     LOG_NORMAL = 0,
     LOG_WARNING = 10,
     LOG_ERROR = 20
} LOG_LEVEL;

extern void (*log_sink)(int, const char *, void *);
extern LOG_LEVEL log_level;
extern void *log_data;

void lprintf(LOG_LEVEL level, const char *fmt, ...);
void log_sink_default(int level, const char *msg, void *data);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
