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
#include <libpdp12utils/log.h>
#include "cpu.h"
#include "linc.h"

#define INSTRUCTION_D(mn) \
  static void instr_ ## mn(cpu_instance* cpu, int addr)

#define INSTRUCTION_A(mn) \
  static void instr_ ## mn(cpu_instance* cpu, int i, int a, int ia)

#define INSTRUCTION_S(mn) \
  static int instr_ ## mn(cpu_instance* cpu, int a)


#define INSTRUCTION_B(mn) \
  static void instr_ ## mn (cpu_instance* cpu, int addr)

#define INSTRUCTION_HF(mn) \
  static void instr_ ## mn (cpu_instance* cpu, int hf, int addr)

#define CASE_D(mn)	      \
  case LINC_OP_ ## mn:	      \
    instr_ ## mn (cpu, addr); \
    return;

#define CASE_A(mn)		  \
  case LINC_OP_ ## mn:		  \
    instr_ ## mn (cpu, i, a, ia); \
    return;

#define CASE_OPX(n, mn)					          \
  case LINC_OPX ## n ## _ ## mn:			          \
    lprintf(LOG_VERBOSE, "%.4o: " #mn " IA: 0%o\n", cpu->pc, ia); \
    instr_ ## mn (cpu, i, a, ia);			          \
    return;

#define CASE_SKIP(mn)					       \
  case LINC_OPXS_ ## mn:				       \
    lprintf(LOG_VERBOSE, "%.4o: " #mn " I: %o\n", cpu->pc, i); \
    skip = instr_ ## mn (cpu, a);			       \
    if((i && !skip) || (!i && skip))                           \
      linc_inc_pc(cpu);                                        \
    return;

#define CASE_B(mn)	      \
  case LINC_OP_ ## mn:	      \
    instr_ ## mn (cpu, addr); \
    return;

#define CASE_HF(mn)		  \
  case LINC_OP_ ## mn:		  \
    instr_ ## mn (cpu, hf, addr); \
    return;


static const char* mnemonics_d[] = {"INVALID",
				    "ADD",
				    "STC",
				    "JMP"};

static const char* mnemonics_b[] = {"LDA",
				    "STA",
				    "ADA",
				    "ADM",
				    "LAM",
				    "MUL",
				    "LDH",
				    "STH",
				    "SHD",
				    "SAE",
				    "SRO",
				    "BCL",
				    "BSE",
				    "BCO",
				    "---",
				    "DSC"};

static const char* mnemonics_a[] = {NULL,
				    "SET",
				    "SAM",
				    "DIS",
				    "XSK",
				    "ROL",
				    "ROR",
				    "SCR",
				    "SXL",
				    NULL,
				    NULL,
				    "TRAP2",
				    "LIF",
				    "LDF",
				    NULL,
				    "TRAP3"};

void linc_inc_pc(cpu_instance* cpu) {
  cpu_set_pc(cpu, 
	     (cpu->pc & 06000) | ((cpu->pc + 1) & 01777));
}


int linc_read(cpu_instance* cpu, int addr) {
  return cpu_read(cpu,
		  (addr & 01777) | 
		  (addr & 02000 ? cpu->dfr : cpu->ifr) << 10);
}

void linc_write(cpu_instance* cpu, int addr, int data) {
  cpu_write(cpu,
	    (addr & 01777) | 
	    (addr & 02000 ? cpu->dfr : cpu->ifr) << 10,
	    data);
}

/*
 * Linc mode addressing:
 * 
 * +--------+-------------------------------+
 * | i | b  | Effective address             |
 * +---+----+-------------------------------+
 * | 0 | 00 | Contents of bits 1-11 in PC+1 |
 * | 1 | 00 | Address of PC+1               |
 * | 0 | xx | The register pointed to by B  |
 * | 1 | xx | Increment register pointed to |
 * |   |    | by B and use that address.    |
 * +---+----+-------------------------------+
 *
 * Note: If beta == 0, skip next instruction.
 *
 */
static int beta_addr(cpu_instance* cpu, int op, int i, int b) {
  int temp;
  
  if(b == 0) {
    temp = cpu->pc;
    
    if(i)
      return temp;
    else
      return linc_read(cpu, temp);
  } else {
    temp = linc_read(cpu, b);
    if(i) {
      if(op == LINC_OP_LDH ||
	 op == LINC_OP_STH ||
	 op == LINC_OP_SHD) {
	/* Bloody half word instructions! They're a bloody mess! */
	temp += LINC_ADDR_RIGHT;
	if(temp & 010000)
	  temp = ((temp + 1) & 01777) | (temp & 06000);
      } else {
	temp = ((temp + 1) & 01777) | (temp & 06000);
      }
      linc_write(cpu, b, temp);
    }
    
    return temp;
  }
}

/* Sign is stored in the highest bit, 
   if op1 && op2 have the same sign and
   the results have a different sign, then
   overflow has occured. */
static void check_overflow(cpu_instance* cpu, int op1, int op2, int res) {
  if((op1 & 04000) == (op2 & 04000) &&
     (res & 04000) != (op1 & 04000))
    cpu_set_flag(cpu, CPU_FLAGS_FLO);
  else
    cpu_clear_flag(cpu, CPU_FLAGS_FLO);
}

static void linc_set_sfr(cpu_instance* cpu) {
  cpu->sfr = (cpu->ifr << 5) | cpu->dfr;
}

static void linc_trap(cpu_instance* cpu) {
  linc_set_sfr(cpu);
  cpu_set_ifr(cpu, 0);
  linc_write(cpu, LINC_TRAP_SAVE_PC, cpu->pc);
  cpu_set_pc(cpu, LINC_TRAP_PC);
}


/*
 * Add to Accumulator (Direct Address)
 * Addition is done using 1's complement.
 */
INSTRUCTION_D(ADD) {
  int op1 = cpu->ac;
  int op2 = linc_read(cpu, addr);
  int t = op1 + op2;
  
  if(t & 010000)
    t = (t & 07777) + 1;
  
  check_overflow(cpu, op1, op2, t);
  cpu_set_ac(cpu, t);
}

/*
 * Add to Accumulator (B-class)
 * Addition is done using 1's complement.
 */
INSTRUCTION_B(ADA) {
  int op1 = cpu->ac;
  int op2 = linc_read(cpu, addr);
  int t = op1 + op2;
  
  if(t & 010000)
    t = (t & 07777) + 1;
  
  check_overflow(cpu, op1, op2, t);
  cpu_set_ac(cpu, t);
}

/*
 * Add to Memory
 * Addition is done using 1's complement.
 */
INSTRUCTION_B(ADM) {
  int op1 = cpu->ac;
  int op2 = linc_read(cpu, addr);
  int t = op1 + op2;
  
  if(t & 010000)
    t = (t & 07777) + 1;
  
  check_overflow(cpu, op1, op2, t);
  cpu_set_ac(cpu, t);
  linc_write(cpu, addr, t);
}

/*
 * Add to Memory
 */
INSTRUCTION_B(LAM) {
  int t = cpu->ac + 
    cpu->l + 
    linc_read(cpu, addr);
  
  cpu_set_l(cpu, 
	    (t & 030000) ? 1 : 0);
  
  t &= 07777;
  cpu_set_ac(cpu, t);
  linc_write(cpu, addr, t);
}

/*
 * Multiply
 */
INSTRUCTION_HF(MUL) {
  /* The multiplication is to be treated as integer
     multiplication if:
     * i = 1 && b = 0
     * The highest bit of the effective address
       is 0.
  */
  int op1 = cpu->ac;
  int op2 = linc_read(cpu, addr);
  int sign = (op1 & 04000) ^ (op2 & 04000);
  int t;
  
  if(op1 & LINC_OPERAND_NEGATIVE)
    op1 = ~op1 & 07777;
  
  if(op2 & LINC_OPERAND_NEGATIVE)
    op2 = ~op2 & 07777;
  
  t = (op1 & 03777) * (op2 & 03777);
  if(hf) {
    /* Fractional multiplication */
    cpu_set_ac(cpu,
	       (((sign ? ~t : t) >> 11) & 03777) | sign);
  } else {
    /* Integer multiplication */
    cpu_set_ac(cpu,
	       ((sign ? ~t : t) & 03777) | sign);
  }
  cpu_set_mq(cpu, (t & 03777) << 1);
  cpu_set_l(cpu, sign);
}

/*
 * Load Accumulator
 */
INSTRUCTION_B(LDA) {
  cpu_set_ac(cpu, linc_read(cpu, addr));
}

/*
 * Load Accumulator (half)
 */
INSTRUCTION_HF(LDH) {
  if(hf)
    cpu_set_ac(cpu, linc_read(cpu, addr) & 077);
  else
    cpu_set_ac(cpu, linc_read(cpu, addr) >> 6);
}


/*
 * Store and Clear
 */
INSTRUCTION_D(STC) {
  linc_write(cpu, addr, cpu->ac);
  cpu_set_ac(cpu, 0);
}


/*
 * Store Accumulator
 */
INSTRUCTION_B(STA) {
  linc_write(cpu, addr, cpu->ac);	     
}

/*
 * Store Accumulator (half)
 */
INSTRUCTION_HF(STH) {
  if(hf)
    linc_write(cpu, addr, 
	       (linc_read(cpu, addr) & 07700) | (cpu->ac & 077));
  else
    linc_write(cpu, addr, 
	       (linc_read(cpu, addr) & 077) | ((cpu->ac & 077) << 6));
}

/*
 * Rotate left
 * Rotates AC, LINK may be included as the
 * most significant bit (bit 13).
 */
INSTRUCTION_A(ROL) {
  int t = cpu->ac;
  int r;
  
  if(i) {
    /* Rotate AC including LINK */
    r = a % 13;
    if(cpu->l)
      t |= 010000;
    t = (t << r) | (t >> (13 - r));
    
    cpu_set_l(cpu, t & 010000);
  } else {
    /* Rotate AC excluding LINK */
    r = a % 12;
    t = (t << r) | (t >> (12 - r));
  }
  
  cpu_set_ac(cpu, t & 07777);
}

/*
 * Rotate right
 * The AC is rotated and data rotated "out" of
 * the AC is shifted into MQ. Link may be
 * included as the most significant bit (13) of AC.
 */
INSTRUCTION_A(ROR) {
  int t = cpu->ac;
  int m = cpu->mq;
  int r;

  if(i && cpu->l)
    t |= 010000;
  
  if(a < 12)
    m = (m >> a) | (cpu->ac  << (12 - a));
  else
    m = (m >> a) | (cpu->ac  >> (a - 12));
  
  if(i) {
    /* Rotate AC including LINK */
    r = a % 13;
    t = (t >> r) | (t << (13 - r));
    
    cpu_set_l(cpu, t & 010000);
  } else {
    /* Rotate AC excluding LINK */
    r = a % 12;
    t = (t >> r) | (t << (12 - r));
  }
  
  cpu_set_mq(cpu, m);
  cpu_set_ac(cpu, t);
}

/*
 * Scale right
 */
INSTRUCTION_A(SCR) {
  int t;
  
  t = ((cpu->ac << 12) | cpu->mq) >> a;
  
  if(cpu->ac & 04000)
    t |= ((1 << a) - 1) << (24 - a);
  
  if(i && a >= 1)
    cpu_set_l(cpu, t & 04000);
  
  cpu_set_ac(cpu, t >> 12);
  cpu_set_mq(cpu, t);
}

/*
 * Skip on external level
 */
INSTRUCTION_A(SXL) {
  int tst = 1;
  
  switch(a) {
  case 015:
    /* Key Struck */
    if(cpu->asr33)
      tst = cpu->asr33->keyboard_flag;
    else
      tst = 0;
    break;
    
  case 016:
    /* TODO: Implement LINCtape */
    /* Tape Instruction done */

  case 017:
    /* TODO: Implement LINCtape */
    /* Tape Word Complete */
    
  default:
    lprintf(LOG_WARNING, "Unknown or unimplemented level (%o) in SXL.\n", a);
    break;
  }
  
  if(i)
    tst = !tst;
  
  if(tst)
    linc_inc_pc(cpu);
}

/*
 * Set register N to contents of register Y
 */
INSTRUCTION_A(SET) {
  int t;
  if(i)
    t = linc_read(cpu, cpu->pc);
  else
    t = linc_read(cpu,
		  linc_read(cpu, cpu->pc));
  
  linc_write(cpu, a, t);
  
  linc_inc_pc(cpu);
}

/*
 * Jump
 */
INSTRUCTION_D(JMP) {
  /* TODO: Check what happens with DJR flag on a JMP 0. */
  
  /* Disable LIF interrupt inhibit if we aren't waiting
     for a new IF.
  */
  if(!(cpu->flags & CPU_FLAGS_LINC_II))
    cpu_clear_flag(cpu, CPU_FLAGS_LINC_II);
  
  if(addr != 0) {
    /* Load new instruction field if requested by LIF. */
    if(cpu->flags & CPU_FLAGS_LIF) {
      cpu_set_ifr(cpu, cpu->ifb);
      cpu_clear_flag(cpu, CPU_FLAGS_LIF);
    }
    
    if(!(cpu->flags & CPU_FLAGS_DJR))
      linc_write(cpu, 0, cpu->pc);
    
    cpu_set_pc(cpu, addr);
  } else {
    cpu_set_pc(cpu, linc_read(cpu, 0) & 01777);
  }

  cpu_clear_flag(cpu, CPU_FLAGS_DJR);
}

/*
 * Bit Clear
 */
INSTRUCTION_B(BCL) {
  cpu_set_ac(cpu, 
	     cpu->ac & ~linc_read(cpu, addr));
}

/*
 * Bit Set
 */
INSTRUCTION_B(BSE) {
  cpu_set_ac(cpu, 
	     cpu->ac | linc_read(cpu, addr));
}

/*
 * Bit Complement
 */
INSTRUCTION_B(BCO) {
  cpu_set_ac(cpu, 
	     cpu->ac ^ linc_read(cpu, addr));  
}

/*
 * Skip on AC == Y
 */
INSTRUCTION_B(SAE) {
  if(cpu->ac == linc_read(cpu, addr))
    linc_inc_pc(cpu);
}

/*
 * Right half AC unequal to specified
 * half of memory register Y.
 */
INSTRUCTION_HF(SHD) {
  int op = linc_read(cpu, addr);
  
  if(!hf)
    op = op >> 6;
  
  if((op & 077) != (cpu->ac & 077))
    linc_inc_pc(cpu);
}

/*
 * Skip and Rotate
 * Skip the next instruction if the least significant
 * bit of the operand is 0. Always rotates the operand 1 step
 * to the right.
 */
INSTRUCTION_B(SRO) {
  int op = linc_read(cpu, addr);
  
  if(!(op & 01))
    linc_inc_pc(cpu);
  
  linc_write(cpu, addr,
	     (op >> 1) | ((op & 1) << 11));
}

/*
 * Index memory register Y, skip when contents
 * of Y equal 1777.
 */
INSTRUCTION_A(XSK) {
  int t = linc_read(cpu, a);
  
  if(i) {
    t = ((t + 1) & 01777) | (t & 06000);
    linc_write(cpu, a, t);
  }
  
  if((t & 01777) == 01777)
    linc_inc_pc(cpu);
}

/*
 * Sample analog channel N.
 */
INSTRUCTION_A(SAM) {
  /* TODO: Implement FAST SAMPLE mode. */
  int v = cpu_call_sam(cpu, ia);
  int va = abs(v);
  
  if(v > 0)
    cpu->ac = v > 0777 ? 0777 : va;
  else
    cpu->ac = 07000 | ~(v < 0777 ? 0777 : va);
}

/*
 * Display point on oscilloscope.
 */
INSTRUCTION_A(DIS) {
  int t;
  
  t = linc_read(cpu, a);
  if(i) {
    t = ((t + 1) & 01777) | (t & 06000);
    linc_write(cpu, a, t);
  }
  
  vr12_dis(cpu->vr12,
	   t & 0777, 
	   (cpu->ac & 0400) ? -(~cpu->ac & 0377) : (cpu->ac & 0377), 
	   (t & 04000) ? 1 : 0);
}

/*
 * Display character on oscilloscope.
 */
INSTRUCTION_B(DSC) {
  int x;
  
  x = linc_read(cpu, LINC_DSC_ADDR_X);
  
  if(cpu->esf & CPU_ESF_CHAR_SIZE) {
    /* Full char size */
    cpu_set_ac(cpu, (cpu->ac & 07740) | 030);
    linc_write(cpu, LINC_DSC_ADDR_X,
	       ((x + 010) & 01777) | (x & 06000));
  } else {
    /* Half char size */
    cpu_set_ac(cpu, (cpu->ac & 07760) | 014);
    linc_write(cpu, LINC_DSC_ADDR_X,
	       ((x + 04) & 01777) | (x & 06000));
  }
  
  /* TODO: Check if coordinates are correctly handled */
  vr12_dsc(cpu->vr12,
	   cpu->esf & CPU_ESF_CHAR_SIZE,
	   x & 0777, 
	   (cpu->ac & 0400) ? -(~cpu->ac & 0377) : (cpu->ac & 0377),
	   linc_read(cpu, addr));
}

/*
 * Load instruction field buffer with N.
 */
INSTRUCTION_A(LIF) {
  /* TODO: Verify this instruction */
  linc_set_sfr(cpu);
  cpu->ifb = ia;
  cpu_set_flag(cpu, CPU_FLAGS_LIF);
  cpu_set_flag(cpu, CPU_FLAGS_LINC_II);
}

/*
 * Load data field register with N.
 */
INSTRUCTION_A(LDF) {
  cpu_set_dfr(cpu, ia);
}

/*
 * Halt execution
 */
INSTRUCTION_A(HLT) {
  lprintf(LOG_NORMAL, "Halted at %.4o\n", cpu->pc);
  cpu_clear_flag(cpu, CPU_FLAGS_RUN);
}

/*
 * Switch to PDP8 mode
 */
INSTRUCTION_A(PDP) {
  lprintf(LOG_VERBOSE, "Switching to PDP8 mode.\n");
  cpu_set_flag(cpu, CPU_FLAGS_8MODE);
  cpu_set_pc(cpu, (cpu->pc & 01777) | ((cpu->ifr & 03) << 10));
}

/*
 * Enable Special Function
 */
INSTRUCTION_A(ESF) {
  int esf = (cpu->ac >> 4);
  cpu_set_esf(cpu, esf);
  if(esf & CPU_ESF_IO_PRESET)
    cpu_io_preset(cpu);
}

/*
 * MQ transfer to AC.
 */
INSTRUCTION_A(QAC) {
  /* TODO: Test QAC */
  cpu_set_ac(cpu, cpu->mq >> 1);
}

/*
 * Disable Jump Return
 */
INSTRUCTION_A(DJR) {
  cpu_set_flag(cpu, CPU_FLAGS_DJR);
}

/*
 * Clear AC, MQ, LINK.
 */
INSTRUCTION_A(CLR) {
  cpu_set_ac(cpu, 0);
  cpu_set_mq(cpu, 0);
  cpu_set_l(cpu, 0);
}

/*
 * AC to Relays
 */
INSTRUCTION_A(ATR) {
  cpu_set_relays(cpu, cpu->ac & 077);
}

/*
 * Relays to AC
 */
INSTRUCTION_A(RTA) {
  cpu_set_ac(cpu, (cpu->ac & 07700) | (cpu->relays & 077));
}

/*
 * No Operation
 */
INSTRUCTION_A(NOP) {
  /* Do nothing... */
}

/*
 * Complement AC.
 */
INSTRUCTION_A(COM) {
  cpu_set_ac(cpu, ~cpu->ac);
}

/*
 * Place Special Function Flip-Flops in AC
 */
INSTRUCTION_A(SFA) {
  cpu_set_ac(cpu, cpu->esf << 4);
}

/*
 * Sens Switch N is set
 */
INSTRUCTION_S(SNS) {
  return cpu->ss & (1 << a);
}

/*
 * Accumulator Zero
 * AC is +0 or -0.
 */
INSTRUCTION_S(AZE) {
  return cpu->ac == 0 || cpu->ac == 07777;
}

/*
 * Accumulator Positive
 * Sign bit (msb of ac) is 0.
 */
INSTRUCTION_S(APO) {
  return !(cpu->ac & 04000);
}

/*
 * Link Zero
 */
INSTRUCTION_S(LZE) {
  return cpu->l == 0;
}

/*
 * LINCtape Inter-Block Zone
 */
INSTRUCTION_S(IBZ) {
  /* TODO: Implement LINC-tape */
  lprintf(LOG_ERROR, "IBZ not implemented. Halting.\n");
  cpu_clear_flag(cpu, CPU_FLAGS_RUN);
  return 0;
}

/*
 * Overflow
 */
INSTRUCTION_S(FLO) {
  return cpu->flags & CPU_FLAGS_FLO;
}

/*
 * MQ Low-Order Bit Zero
 */
INSTRUCTION_S(QLZ) {
  return !(cpu->mq & 01);
}

/*
 * Skip unconditionally 
 */
INSTRUCTION_S(SKP) {
  return 1;
}

/*
 * Execute IO instruction
 */
INSTRUCTION_A(IOB) {
  /* TODO: Check if PC should be increased before or after iob_io. */
  cpu->ir = linc_read(cpu, cpu->pc);
  iob_io(cpu);
  linc_inc_pc(cpu);
}

/*
 * Right Switches to AC
 */
INSTRUCTION_A(RSW) {
  cpu_set_ac(cpu, cpu->rs);
}

/*
 * Left Switches to AC
 */
INSTRUCTION_A(LSW) {
  cpu_set_ac(cpu, cpu->ls);
}

/* +-----+-----------------------+
 * | 0 1 | 2 3 4 5 6 7 8 9 10 11 |
 * | OP  |       ADDRESS         |
 * +-----+-----------------------+
 */
static void instr_direct(cpu_instance* cpu, int op, int addr) {
  lprintf(LOG_VERBOSE, "%.4o: %s 0%.4o\n", cpu->pc, mnemonics_d[op], addr);
  
  switch(op) {
    CASE_D(ADD);
    CASE_D(STC);
    CASE_D(JMP);
  default:
    lprintf(LOG_ERROR, "Illegal LINC instruction  in instr_direct!\n");
    cpu_clear_flag(cpu, CPU_FLAGS_RUN);
    break;
  }
}

/* +-------+---------+---+-----------+
 * | 0 1 2 | 3 4 5 6 | 7 | 8 9 10 11 |
 * | 0 0 0 | OP-code | I |  ALPHA    |
 * +-------+---------+---+-----------+
 */
static void instr_alpha(cpu_instance* cpu, int op, int i, int a) {
  int ia = (i << 4) | a;
  int skip;
  
  if(mnemonics_a[op]) {
    if(i)
      lprintf(LOG_VERBOSE, "%.4o: %s I 0%o\n", cpu->pc, mnemonics_a[op], a);
    else
      lprintf(LOG_VERBOSE, "%.4o: %s 0%o\n", cpu->pc, mnemonics_a[op], a);
  }
  
  switch(op) {
    CASE_A(SET);
    CASE_A(SAM);
    CASE_A(DIS);
    CASE_A(XSK);
    CASE_A(ROL);
    CASE_A(ROR);
    CASE_A(SCR);
    CASE_A(SXL);
    CASE_A(LIF);
    CASE_A(LDF);
  case LINC_OP_TAPE:
    if(!(cpu->esf & CPU_ESF_INSTRUCTION_TRAP &&
	 cpu->esf & CPU_ESF_TAPE_TRAP)) {
      
      lprintf(LOG_ERROR, "LINCtape not implemented. Halting.\n");
      cpu_clear_flag(cpu, CPU_FLAGS_RUN);
      return;
    }
    break;
    
  case LINC_OP_EXT1:
    switch(ia) {
      CASE_OPX(1, HLT);
      CASE_OPX(1, PDP);
      CASE_OPX(1, ESF);
      CASE_OPX(1, QAC);
      CASE_OPX(1, DJR);
      CASE_OPX(1, CLR);
      CASE_OPX(1, ATR);
      CASE_OPX(1, RTA);
      CASE_OPX(1, NOP);
      CASE_OPX(1, COM);
      CASE_OPX(1, SFA);
    default:
      break;
    }
    break;
    
  case LINC_OP_SKIP:
    /* TODO: Check if this is OK */
    if(!(a & 010)) {
      lprintf(LOG_VERBOSE, "%.4o: SNS I: %o\n", cpu->pc, i);
      skip = instr_SNS(cpu, a);
      if((i && !skip) || (!i && skip))
	linc_inc_pc(cpu);
      return;
    } else {
      switch(a) {
	CASE_SKIP(AZE);
	CASE_SKIP(APO);
	CASE_SKIP(LZE);
	CASE_SKIP(IBZ);
	CASE_SKIP(FLO);
	CASE_SKIP(QLZ);
	CASE_SKIP(SKP);
      default:
	break;
      }
    }
    break;
    
  case LINC_OP_EXT2:
    switch(ia) {
      CASE_OPX(2, IOB);
      CASE_OPX(2, RSW);
      CASE_OPX(2, LSW);
    default:
      break;
    }
    break;
    
  default:
    break;
  }
  
  if(cpu->esf & CPU_ESF_INSTRUCTION_TRAP) {
    lprintf(LOG_VERBOSE, "Instruction trap in instr_alpha.\n");
    linc_trap(cpu);
  } else {
    lprintf(LOG_ERROR, "Illegal LINC instruction  in instr_alpha!\n");
    cpu_clear_flag(cpu, CPU_FLAGS_RUN);
  }
}

/* +-------+---------+---+-----------+
 * | 0 1 2 | 3 4 5 6 | 7 | 8 9 10 11 |
 * | 0 0 1 | OP-code | I |   BETA    |
 * +-------+---------+---+-----------+
 */
static void instr_beta(cpu_instance* cpu, int op, int i, int b) {
  int addr = beta_addr(cpu, op, i, b);
  /* Should we use the right half?
   * Or in case of MUL, are we dealing with fractions?
   */
  int hf = addr & LINC_ADDR_RIGHT && !(i && b == 0);
  
  lprintf(LOG_VERBOSE, "%.4o: %s %s 0%o (%.4o)\n",
	  cpu->pc,
	  mnemonics_b[op],
	  i ? "I" : "",
	  b,
	  addr);
  
  if(b == 0)
    linc_inc_pc(cpu);
  
  switch(op) {
    CASE_B(LDA);
    CASE_B(STA);
    CASE_B(ADA);
    CASE_B(ADM);
    CASE_B(LAM);
    CASE_HF(MUL);
    CASE_HF(LDH);
    CASE_HF(STH);
    CASE_HF(SHD);
    CASE_B(SAE);
    CASE_B(SRO);
    CASE_B(BCL);
    CASE_B(BSE);
    CASE_B(BCO);
    CASE_B(DSC);
  default:
    break;
  }
  
  if(cpu->esf & CPU_ESF_INSTRUCTION_TRAP) {
    lprintf(LOG_VERBOSE, "Instruction trap in instr_beta.\n");
    linc_trap(cpu);
  } else {
    lprintf(LOG_ERROR, "Illegal LINC instruction  in instr_beta!\n");
    cpu_clear_flag(cpu, CPU_FLAGS_RUN);
  }
}

void linc_do(cpu_instance* cpu) {
  lprintf(LOG_DEBUG, "linc_step pc: 0%o ir: 0%o\n", cpu->pc, cpu->ir);
  
  switch(cpu->ir & 07000) {
  case LINC_CLASS_ALPHA:
    instr_alpha(cpu,
		(cpu->ir & 00740) >> 5,
		cpu->ir &  00020,
		cpu->ir &  00017);
    break;
    
  case LINC_CLASS_BETA:
    instr_beta(cpu,
	       (cpu->ir & 00740) >> 5,
	       cpu->ir &  00020,
	       cpu->ir &  00017);
    break;
    
  default:
    /* LINC_CLASS_DIRECT */
    instr_direct(cpu,
		 (cpu->ir & 06000) >> 10,
		 cpu->ir &  01777);
    break;
  }
}

void linc_step(cpu_instance* cpu) {
  cpu->ir = linc_read(cpu, cpu->pc & 01777);
  linc_inc_pc(cpu);
  linc_do(cpu);
}
