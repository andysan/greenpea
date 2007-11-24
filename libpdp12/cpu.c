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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libpdp12utils/log.h>

#include "cpu.h"
#include "pdp8.h"
#include "linc.h"

cpu_instance *
cpu_create() {
     cpu_instance *cpu = malloc(sizeof(cpu_instance));
     /* TODO: Check buffer allocation */

     /* Registers */
     cpu->ac = 0;
     cpu->l = 0;
     cpu->pc = 0200;
     cpu->mq = 0;
     cpu->ir = 0;
     cpu->ifr = 02;
     cpu->dfr = 03;
     cpu->relays = 0;
  
     /* State */
     cpu->flags = 0;
     cpu->state = CPU_STATE_B;
  
     /* Switches */
     cpu->ls = 0;
     cpu->rs = 0;
     cpu->ifs = 0;
     cpu->ss = 0;
  
     cpu->devices = NULL;
     cpu->callbacks = NULL;
     cpu->vr12 = NULL;
     cpu->asr33 = NULL;
  
     memset(cpu->core, 0, CPU_CORE_SIZE);
  
     return cpu;
}

void
cpu_destroy(cpu_instance *cpu) {
     free(cpu);
}

void
cpu_io_preset(cpu_instance *cpu) {
     /* TODO: Check what more needs to go into this operation */
     cpu->esf = CPU_ESF_CHAR_SIZE;
}

void
cpu_step(cpu_instance *cpu) {
     if (cpu->flags & CPU_FLAGS_8MODE) {
          pdp8_step(cpu);
     } else {
          linc_step(cpu);
     }
}

void
cpu_set_ac(cpu_instance *cpu, int ac) {
     lprintf(LOG_DEBUG, "cpu_set_ac %.4o\n", ac);
     cpu->ac = ac & 07777;
}

void
cpu_set_pc(cpu_instance *cpu, int pc) {
     lprintf(LOG_DEBUG, "cpu_set_pc %.4o\n", pc);
     cpu->pc = pc & 07777;
}

void
cpu_set_mq(cpu_instance *cpu, int mq) {
     lprintf(LOG_DEBUG, "cpu_set_mq %.4o\n", mq);
     cpu->mq = mq & 07777;
}

void
cpu_set_l(cpu_instance *cpu, int l) {
     lprintf(LOG_DEBUG, "cpu_set_l %i\n", l);
     cpu->l = l ? 1 : 0;
}

void
cpu_set_state(cpu_instance *cpu, cpu_states s) {
     cpu->state |= s;
}

void
cpu_clear_state(cpu_instance *cpu, cpu_states s) {
     cpu->state &= ~s;
}

void
cpu_set_flag(cpu_instance *cpu, cpu_flags f) {
     cpu->flags |= f;
}

void
cpu_clear_flag(cpu_instance *cpu, cpu_flags f) {
     cpu->flags &= ~f;
}

void
cpu_set_relays(cpu_instance *cpu, int r) {
     cpu->relays = r & 077;
     if (cpu->callbacks && cpu->callbacks->update_relays)
          cpu->callbacks->update_relays(cpu,
                                        cpu->callbacks->data);
}

void
cpu_set_ifr(cpu_instance *cpu, int n) {
     lprintf(LOG_DEBUG, "cpu_set_ifr: %.2o\n", n);
     cpu->ifr = n & 037;
}

void
cpu_set_dfr(cpu_instance *cpu, int n) {
     lprintf(LOG_DEBUG, "cpu_set_dfr: %.2o\n", n);
     cpu->dfr = n & 037;
}

void
cpu_set_esf(cpu_instance *cpu, int n) {
     lprintf(LOG_DEBUG, "cpu_set_esf: %.2o\n", n);
     cpu->esf = n & 076;
}

void
cpu_write(cpu_instance *cpu, int ma, int mb) {
     /* TODO: Do something useful after outputing error. */
     lprintf(LOG_DEBUG, "cpu_write: %.4o %.4o\n", ma, mb);
     if (ma > CPU_CORE_SIZE)
          lprintf(LOG_ERROR, "Address out of range in cpu_write.\n");
     else
          cpu->core[ma] = mb & 07777;
}

int
cpu_read(cpu_instance *cpu, int ma) {
     /* TODO: Do something useful after outputing error. */
     lprintf(LOG_DEBUG, "cpu_read: %.4o\n", ma);
     if (ma > CPU_CORE_SIZE) {
          lprintf(LOG_ERROR, "Address out of range in cpu_read.\n");
          return 0;
     } else {
          return cpu->core[ma];
     }
}

int
cpu_call_sam(cpu_instance *cpu, int n) {
     if (cpu->callbacks && cpu->callbacks->sample_ad)
          return cpu->callbacks->sample_ad(cpu,
                                           cpu->callbacks->data,
                                           n);
     else
          return 0;
}

int
cpu_call_ext_level(cpu_instance *cpu, int level) {
     if (cpu->callbacks && cpu->callbacks->get_level)
          return cpu->callbacks->get_level(cpu,
                                           cpu->callbacks->data,
                                           level);
     else
          /* Unknown levels are hardwired to one according to docs. */
          return 1;
}

void
cpu_inc_pc(cpu_instance *cpu) {
     if (cpu->flags & CPU_FLAGS_8MODE) {
          pdp8_inc_pc(cpu);
     } else {
          linc_inc_pc(cpu);
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
