/*
 * $Id: vr12.c 24 2006-06-02 16:24:53Z sandberg $
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

#include <stdlib.h>

#include <libpdp12utils/log.h>
#include "cpu.h"
#include "vr12.h"


static void vr12_dsce(int full, vr12* vr12, int x, int y, int pattern) {
  int i;
  int spacing = full ? 04 : 02;
  
  if(!vr12->dis)
    return;
  
  for(i = 0; i < 12; i++) {
    vr12->dis(i <= 5 ? x + spacing * 2 : x + spacing, 
	      y + ((i % 6) - 5) * spacing, 
	      2, 
	      vr12->data);
  }
}

void vr12_dis(vr12* vr12, int x, int y, int c) {
  if(!vr12 || !vr12->dis)
    return;

  vr12->dis(x, -y + 0377, c, vr12->data);
}

void vr12_dsc(vr12* vr12, int full, int x, int y, int pattern) {
  if(!vr12)
    return;
  
  if(full) {
    if(vr12->dsc_full)
      vr12->dsc_full(x, y, pattern, vr12->data);
    else
      vr12_dsce(full, vr12, x, y, pattern);
  } else {
    if(vr12->dsc_half)
      vr12->dsc_half(x, y, pattern, vr12->data);
    else
      vr12_dsce(full, vr12, x, y, pattern);
  }
}
