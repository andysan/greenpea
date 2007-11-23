/*
 * $Id: vr12.h 24 2006-06-02 16:24:53Z sandberg $
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

#ifndef _VR12_H
#define _VR12_H

/*
 * Interface for the VR12 device.
 * The original display had the following properties:
 * Dimensions: 512x512 pixelse
 * Pixel distance: 0.0176" horizontal
 *                 0.0127" vertical
 * Upper left corner: (0, 0377)
 * Lower right corner: (0777, -377)
 *
 *
 * The emulator automatically translates the coordinates
 * so the upper left corner has the coordinate (0,0).
 *
 * The emulator only requires the frontend to implement the
 * dis operation, in that case, one or both, of the dsc operations
 * will be emulated using the dis instruction. Channel number 2
 * will be used incase of emulation.
 *
 * The pattern is as follows (bit 0 i lsb):
 *  5 11
 *  4 10
 *  3 9
 *  2 8
 *  1 7
 *  0 6
 *
 */
typedef struct vr12_s {
     void (*dis)(int x, int y, int channel, void* data);
     void (*dsc_half)(int x, int y, int pattern, void* data);
     void (*dsc_full)(int x, int y, int pattern, void* data);
     void * data;
} vr12;

void vr12_dis(vr12* vr12, int x, int y, int c);
void vr12_dsc(vr12* vr12, int full, int x, int y, int pattern);

#endif
/* 
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
