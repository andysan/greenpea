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
#ifndef _TELETYPE_H
#define _TELETYPE_H

#define TELETYPE_KSF (06031)
#define TELETYPE_KCC (06032)
#define TELETYPE_KRS (06034)


#define TELETYPE_TSF (06041)
#define TELETYPE_TCF (06042)
#define TELETYPE_TPC (06044)

typedef enum {
  TELETYPE_FLAGS_KEYBOARD = 1,
  TELETYPE_FLAGS_PRINTER = 2
} teletype_flags;

typedef struct {
  int flags;
} teletype_state;

io_device* teletype_create();

#endif
