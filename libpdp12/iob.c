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

#include <libpdp12utils/log.h>
#include "cpu.h"
#include "iob.h"
#include "asr33.h"

static int pdp12_io(cpu_instance* cpu) {
     switch(cpu->ir) {
     case IOB_ION:
          lprintf(LOG_VERBOSE, "Enabling interrupts.\n");
          cpu_set_flag(cpu, CPU_FLAGS_ION);
          return 1;
    
     case IOB_IOF: 
          lprintf(LOG_VERBOSE, "Disabling interrupts.\n");
          cpu_clear_flag(cpu, CPU_FLAGS_ION);
          return 1;

     case IOB_LINC:
          lprintf(LOG_VERBOSE, "Switching to LINC mode.\n");
          cpu_clear_flag(cpu, CPU_FLAGS_8MODE);
          cpu_set_ifr(cpu, (cpu->ifr & 034) | (cpu->pc >> 10));
          cpu_set_pc(cpu, cpu->pc & 01777);
          return 1;

     case IOB_RIF:
     case IOB_RDF:
     case IOB_RIB:
          lprintf(LOG_ERROR, "IO instruction not implemented. Halting.\n");
          cpu_clear_flag(cpu, CPU_FLAGS_RUN);
          return 1;
    
     default:
          return 0;
     }
  
}

void iob_io(cpu_instance* cpu) {
     io_device* dev;
     int i;
  
     if(pdp12_io(cpu))
          return;
  
     if(asr33_instr(cpu))
          return;
  
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
/* 
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
