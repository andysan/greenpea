/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <libpdp12utils/log.h>
#include "cpu.h"
#include "iob.h"
#include "asr33.h"

static int
pdp12_io(cpu_instance_t *cpu) {
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
iob_io(cpu_instance_t *cpu) {
    io_device_t *dev;
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
iob_reset(cpu_instance_t *cpu) {
    io_device_t *dev;
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
