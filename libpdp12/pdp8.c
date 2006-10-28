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

/* TODO: BIG FAT TODO! Implement autoindexing */

#include <libpdp12utils/log.h>
#include "cpu.h"
#include "pdp8.h"

static const char* mnemonics[] = {"AND",
				  "TAD",
				  "ISZ",
				  "DCA",
				  "JMS",
				  "JMP",
				  "IO",
				  "OTHER"};

#define INSTRUCTION_M(mn) \
  static void instr_ ## mn (cpu_instance* cpu, int i, int eaddr)

#define CASE_M(mn)		      \
  case PDP8_OP_ ## mn:		      \
    instr_ ## mn(cpu, i, eaddr);      \
    return;

#define G(n, mn) (cpu->ir & PDP8_G ## n ## _ ## mn)

#define SKIP(c) skip = sense ? skip && (c) : skip || (c);

/*
 * Logical AND to Accumulator
 */
INSTRUCTION_M(AND) {
  int op = !i ? 
    pdp8_read_i(cpu, eaddr) :
    pdp8_read_d(cpu, eaddr);
  
  cpu_set_ac(cpu, cpu->ac & op);
}

/*
 * Two's Complement Add to Accumulator
 */
INSTRUCTION_M(TAD) {
  int op = !i ? 
    pdp8_read_i(cpu, eaddr) :
    pdp8_read_d(cpu, eaddr);
  int sum = op + cpu->ac;
  
  cpu_set_ac(cpu, sum & 07777);
  if(sum & 010000)
    cpu_set_l(cpu, ~cpu->l);
}

/*
 * Increment And Skip If Zero
 */
INSTRUCTION_M(ISZ) {
  int op = !i ? 
    pdp8_read_i(cpu, eaddr) :
    pdp8_read_d(cpu, eaddr);
  
  int sum = (op + 1) & 07777;
  
  if(sum == 0)
    pdp8_inc_pc(cpu);
  
  if(!i)
    pdp8_write_i(cpu, eaddr, sum);
  else
    pdp8_write_d(cpu, eaddr, sum);
}

/*
 * Deposit and clear
 */
INSTRUCTION_M(DCA) {
  if(!i)
    pdp8_write_i(cpu, eaddr, cpu->ac);
  else
    pdp8_write_d(cpu, eaddr, cpu->ac);
  
  cpu_set_ac(cpu, 0);
}

/*
 * Jump to Subroutine
 */
INSTRUCTION_M(JMS) {
  pdp8_write_i(cpu, eaddr, cpu->pc);
  cpu_set_pc(cpu, (eaddr + 1) & 07777);
}

/*
 * Jump
 */
INSTRUCTION_M(JMP) {
  cpu_set_pc(cpu, eaddr);
}

void pdp8_inc_pc(cpu_instance* cpu) {
  cpu_set_pc(cpu, (cpu->pc + 1) & 07777);
}

int pdp8_read_d(cpu_instance* cpu, int addr) {
  return cpu_read(cpu,
		  (addr & 07777) | 
		  ((cpu->dfr & 034) << 10));
}

void pdp8_write_d(cpu_instance* cpu, int addr, int data) {
  cpu_write(cpu,
	    (addr & 07777) | 
	    ((cpu->dfr & 034) << 10),
	    data);
}

int pdp8_read_i(cpu_instance* cpu, int addr) {
  return cpu_read(cpu,
		  (addr & 07777) | 
		  ((cpu->ifr & 034) << 10));
}

void pdp8_write_i(cpu_instance* cpu, int addr, int data) {
  cpu_write(cpu,
	    (addr & 07777) | 
	    ((cpu->ifr & 034) << 10),
	    data);
}

static void instr_mem(cpu_instance* cpu) {
  int op = cpu->ir & 07000;      /* Operation Code */
  int i = cpu->ir & 0400;        /* Indirect Addressing */
  int p = cpu->ir & 0200;        /* Memory Page */
  int addr = 
    (p ? cpu->pc & 07600 : 0) |  /* This page or page 0? */
    (cpu->ir & 0177);            /* Page Address */
  
  int eaddr = i ? 
    (op == PDP8_OP_JMP || op == PDP8_OP_JMS 
     ? pdp8_read_i(cpu, addr) : pdp8_read_d(cpu,addr))
    : addr;

  if(i && addr >= 010 && addr <= 017) {
    eaddr = (eaddr + 1) & 07777;
    if(op == PDP8_OP_JMP || op == PDP8_OP_JMS)
      pdp8_write_i(cpu, addr, eaddr);
    else
      pdp8_write_d(cpu, addr, eaddr);
  }
      
  
  
  switch(op) {
    CASE_M(AND);
    CASE_M(TAD);
    CASE_M(ISZ);
    CASE_M(DCA);
    CASE_M(JMS);
    CASE_M(JMP);
  default:
    lprintf(LOG_ERROR, "Unknown instruction in instr_mem, this shouldn't happen!\n");
    return;
  }
}

static void instr_g1(cpu_instance* cpu) {
  int temp;
  int rot = (cpu->ir & PDP8_G1_ROTX2) ? 2 : 1;

  if(G(1, CLA))
    cpu_set_ac(cpu, 0);
  
  if(G(1, CLL))
    cpu_set_l(cpu, 0);
  
  if(G(1, CMA))
    cpu_set_ac(cpu, ~cpu->ac);
  
  if(G(1, CML))
    cpu_set_l(cpu, ~cpu->l);
  
  if(G(1, IAC)) {
    temp = cpu->ac + 1;
    cpu_set_ac(cpu, temp);
    if(temp & 010000)
      cpu_set_l(cpu, ~cpu->l);
  }
  
  if(G(1, RAR)) {
    temp = (cpu->l << 12) | cpu->ac;
    temp = (temp >> rot) | (temp << (13 - rot));
    cpu_set_ac(cpu, temp);
    cpu_set_l(cpu, temp & 010000);
  }
  
  if(G(1, RAL)) {
    temp = (cpu->l << 12) | cpu->ac;
    temp = (temp << rot) | (temp >> (13 - rot));
    cpu_set_ac(cpu, temp);
    cpu_set_l(cpu, temp & 010000);
  }
}

static void instr_g2(cpu_instance* cpu) {
  int sense = cpu->ir & PDP8_G2_SENSE;
  int skip = sense ? 1 : 0;
  
  /* SMA */
  if(G(2, SMA) && !sense)
    SKIP(cpu->ac & 04000);
  
  /* SPA */
  if(G(2, SMA) && sense)
    SKIP(~cpu->ac & 04000);
  
  /* SZA */
  if(G(2, SZA) && !sense)
    SKIP(cpu->ac == 0);
  
  /* SNA */
  if(G(2, SZA) && sense)
    SKIP(cpu->ac != 0);
  
  /* SNL */
  if(G(2, SNL) && !sense)
    SKIP(cpu->l != 0);
  
  /* SZL */
  if(G(2, SNL) && sense)
    SKIP(cpu->l == 0);
  
  if(skip)
    pdp8_inc_pc(cpu);
  
  if(G(2, CLA))
    cpu_set_ac(cpu, 0);
  
  if(G(2, OSR))
    cpu_set_ac(cpu, cpu->rs);
  
  if(G(2, HLT)) {
    cpu_clear_flag(cpu, CPU_FLAGS_RUN);
  }
  
}

static void instr_eae(cpu_instance* cpu) {
  lprintf(LOG_ERROR, "Extended Arithmetic Element (KE12) not implemented.\n");
  cpu_clear_flag(cpu, CPU_FLAGS_RUN);
}

void pdp8_do(cpu_instance* cpu) {
  int op = cpu->ir & 07000;  /* Operation Code */
  lprintf(LOG_VERBOSE, "%.4o: %s (%.4o)\n", cpu->pc, mnemonics[op >> 9], cpu->ir);
  
  switch(op) {
  case PDP8_OP_IO:
    iob_io(cpu);
    break;
      
  case PDP8_OP_OTHER:
    if(cpu->ir & PDP8_G2) {
      if(cpu->ir & PDP8_G2_EAE) {
	/* EAE */
	instr_eae(cpu);
      } else {
	/* Group 2 */
	instr_g2(cpu);
      }
    } else {
      /* Group 1 */
      instr_g1(cpu);
    }
    break;
    
  default:
    instr_mem(cpu);
    break;
  }
}

void pdp8_step(cpu_instance* cpu) {
  cpu->ir = pdp8_read_i(cpu, cpu->pc);
  pdp8_inc_pc(cpu);
  pdp8_do(cpu);
}

