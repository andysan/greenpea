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

#ifndef _IOB_H
#define _IOB_H

#define IOB_ION  (06001)
#define IOB_IOF  (06002)
#define IOB_LINC (06141)
#define IOB_RIF  (06224)
#define IOB_RDF  (06214)
#define IOB_RIB  (06234)
#define IOB_RMF  (06244)


typedef struct io_device_s{
     /* Data to be passed to all IO instructions */
     void *data;

     /* Called on IO, returns 0 if the IO-request
        was not handled. */
     int (*io)(cpu_instance *cpu, void *data);
     /* Called on an IO-reset. */
     void (*reset)(cpu_instance *cpu, void *data);
     /* Called to deallocate data and close the device. */
     void (*destroy)(void *data);
} io_device;

void iob_io(struct cpu_instance_s *cpu);
void iob_reset(struct cpu_instance_s *cpu);

#endif
/* 
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
