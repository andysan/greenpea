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

#define PDP8_OP_AND   (00000)
#define PDP8_OP_TAD   (01000)
#define PDP8_OP_ISZ   (02000)
#define PDP8_OP_DCA   (03000)
#define PDP8_OP_JMS   (04000)
#define PDP8_OP_JMP   (05000)
#define PDP8_OP_IO    (06000)
#define PDP8_OP_OTHER (07000)

#define PDP8_G1_CLA   (00200)
#define PDP8_G1_CLL   (00100)
#define PDP8_G1_CMA   (00040)
#define PDP8_G1_CML   (00020)
#define PDP8_G1_RAR   (00010)
#define PDP8_G1_RAL   (00004)
#define PDP8_G1_ROTX2 (00002)
#define PDP8_G1_IAC   (00001)


#define PDP8_G2       (00400)
#define PDP8_G2_CLA   (00200)
#define PDP8_G2_SMA   (00100)
#define PDP8_G2_SZA   (00040)
#define PDP8_G2_SNL   (00020)
#define PDP8_G2_SENSE (00010)
#define PDP8_G2_OSR   (00004)
#define PDP8_G2_HLT   (00002)
#define PDP8_G2_EAE   (00001)

#define PDP8_EAE_CLA  (00200)
#define PDP8_EAE_MQA  (00100)
#define PDP8_EAE_SCA  (00040)
#define PDP8_EAE_MQL  (00020)

#define PDP8_EAE_SCL  (01 << 1)
#define PDP8_EAE_MUY  (02 << 1)
#define PDP8_EAE_DVI  (03 << 1)
#define PDP8_EAE_NMI  (04 << 1)
#define PDP8_EAE_SHL  (05 << 1)
#define PDP8_EAE_ASR  (06 << 1)
#define PDP8_EAE_LSR  (07 << 1)

void pdp8_do(cpu_instance* cpu);
void pdp8_step(cpu_instance* cpu);
void pdp8_inc_pc(cpu_instance* cpu);

int pdp8_read_d(cpu_instance* cpu, int addr);
void pdp8_write_d(cpu_instance* cpu, int addr, int data);
int pdp8_read_i(cpu_instance* cpu, int addr);
void pdp8_write_i(cpu_instance* cpu, int addr, int data);

#endif
