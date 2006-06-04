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

#ifndef _PDP8_H
#define _PDP8_H

#define PDP8_OP_AND   (00)
#define PDP8_OP_TAD   (01)
#define PDP8_OP_ISZ   (02)
#define PDP8_OP_DCA   (03)
#define PDP8_OP_JMS   (04)
#define PDP8_OP_JMP   (05)
#define PDP8_OP_IO    (06)
#define PDP8_OP_OTHER (07)

void pdp8_do(cpu_instance* cpu);
void pdp8_step(cpu_instance* cpu);
void pdp8_inc_pc(cpu_instance* cpu);

int pdp8_read_d(cpu_instance* cpu, int addr);
void pdp8_write_d(cpu_instance* cpu, int addr, int data);
int pdp8_read_i(cpu_instance* cpu, int addr);
void pdp8_write_i(cpu_instance* cpu, int addr, int data);

#endif
