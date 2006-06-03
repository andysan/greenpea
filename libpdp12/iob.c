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

#include <liblog/log.h>
#include "cpu.h"
#include "iob.h"
#include "asr33.h"

void iob_io(cpu_instance* cpu) {
  io_device* dev;
  int i;
  
  asr33_instr(cpu);
  
  if(!cpu->devices)
    return;
  
  for(i = 0; cpu->devices[i]; i++) {
    dev = cpu->devices[i];
    if(dev->io(cpu, dev->data))
      return;
  }
  
  lprintf(LOG_ERROR, "IO-instruction (IR=%.4o PC=%.4o) not handled.\n", cpu->ir, cpu->pc);
  cpu_clear_flag(cpu, CPU_FLAGS_RUN);
}

void iob_reset(cpu_instance* cpu) {
  io_device* dev;
  int i;
  
  asr33_reset(cpu);
  
  if(!cpu->devices)
    return;
  
  for(i = 0; cpu->devices[i]; i++) {
    dev = cpu->devices[i];
    dev->reset(cpu, dev->data);
  }
}