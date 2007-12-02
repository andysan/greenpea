/*
 * Copyright (c) 2006, 2007 Andreas Sandberg
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, arepermitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *        * Neither the name of the Greenpea project nor the names of its
 *          contributors may be used to endorse or promote products derived
 *          from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <libpdp12utils/log.h>
#include "cpu.h"
#include "iob.h"
#include "asr33.h"

static int
pdp12_io(cpu_instance *cpu) {
     switch (cpu->ir) {
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

void
iob_io(cpu_instance *cpu) {
     io_device* dev;
     int i;

     if (pdp12_io(cpu))
          return;

     if (asr33_instr(cpu))
          return;

     if (!cpu->devices)
          return;

     for (i = 0; cpu->devices[i]; i++) {
          dev = cpu->devices[i];
          if (dev->io(cpu, dev->data))
               return;
     }

     lprintf(LOG_ERROR, "IO-instruction (IR=%.4o PC=%.4o) not handled.\n", cpu->ir, cpu->pc);
     cpu_clear_flag(cpu, CPU_FLAGS_RUN);
}

void
iob_reset(cpu_instance *cpu) {
     io_device* dev;
     int i;

     asr33_reset(cpu);

     if (!cpu->devices)
          return;

     for (i = 0; cpu->devices[i]; i++) {
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
