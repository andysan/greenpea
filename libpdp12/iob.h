/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _IOB_H
#define _IOB_H

#define IOB_ION  (06001)
#define IOB_IOF  (06002)
#define IOB_LINC (06141)
#define IOB_RIF  (06224)
#define IOB_RDF  (06214)
#define IOB_RIB  (06234)
#define IOB_RMF  (06244)


typedef struct {
    /* Data to be passed to all IO instructions */
    void *data;

    /* Called on IO, returns 0 if the IO-request
       was not handled. */
    int (*io)(cpu_instance_t *cpu, void *data);
    /* Called on an IO-reset. */
    void (*reset)(cpu_instance_t *cpu, void *data);
    /* Called to deallocate data and close the device. */
    void (*destroy)(void *data);
} io_device_t;

void iob_io(cpu_instance_t *cpu);
void iob_reset(cpu_instance_t *cpu);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
