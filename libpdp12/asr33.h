/*
 * $Id: asr33.h 24 2006-06-02 16:24:53Z sandberg $
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

#ifndef _ASR33_H
#define _ASR33_H

/*
 * Interface for the ASR33 device.
 */
typedef struct asr33_s {
     /* Called when the instruction to print a character on the ASR33 is called.
      * Is responsible for setting the printer_flag when printing is done.
      */
     void (*print)(unsigned char c, void* data);
     char (*read)(void* data);
  
     int keyboard_flag : 1;
     int printer_flag : 1;
  
     void* data;
} asr33;

int asr33_instr(cpu_instance* cpu);
void asr33_reset(cpu_instance* cpu);

#endif
/* 
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
