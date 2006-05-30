
#include <stdlib.h>
#include <stdio.h>
#include <liblog/log.h>
#include "cpu.h"
#include "linc.h"

#define INSTRUCTION_D(mn) \
  static void instr_ ## mn(cpu_instance* cpu, int addr)

#define INSTRUCTION_A(mn) \
  static void instr_ ## mn(cpu_instance* cpu, int i, int a, int ia)

#define INSTRUCTION_B(mn) \
  static void instr_ ## mn (cpu_instance* cpu, int addr)

#define CASE_D(mn) \
  case LINC_OP_ ## mn: \
    instr_ ## mn (cpu, addr); \
    break;

#define CASE_A(mn) \
  case LINC_OP_ ## mn: \
    instr_ ## mn (cpu, i, a, ia); \
    break;

#define CASE_B(mn) \
  case LINC_OP_ ## mn: \
    instr_ ## mn (cpu, addr); \
    break;


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

static const char* mnemonics_a[] = {"EXT1",
				    "SET",
				    "SAM",
				    "DIS",
				    "XSK",
				    "ROL",
				    "ROR",
				    "SCR",
				    "SXL",
				    "EXT2",
				    "EXT3",
				    "TRAP2",
				    "LIF",
				    "LDF",
				    "TAPE",
				    "EXT4"};

void linc_inc_pc(cpu_instance* cpu) {
  cpu_set_pc(cpu, 
	     cpu->pc > CPU_LINC_SEG_SIZE ? 0 : cpu->pc + 1);
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
static int beta_addr(cpu_instance* cpu, int i, int b) {
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
      temp = ((temp + 1) & 01777) | (temp & 06000);
      linc_write(cpu, b, temp);
    }
    
    return temp;
  }
}

/* Sign is stored in the highest bit, 
   if op1 && op2 have the same sign and
   the results have a different sign, then
   overflow has occured. */
void check_overflow(cpu_instance* cpu, int op1, int op2, int res) {
  if((op1 & 04000) == (op2 & 04000) &&
     (res & 04000) != (op1 & 04000))
    cpu_set_flag(cpu, CPU_FLAGS_FLO);
  else
    cpu_clear_flag(cpu, CPU_FLAGS_FLO);
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
INSTRUCTION_B(MUL) {
  /* The multiplication is to be treated as integer
     multiplication if:
     * i = 1 && b = 0
     * The highest bit of the b-register (effective address)
       is 0.
       
     It's however enough to check the highest bit of 
     the address since it can only be 1 for the
     under the desired circumstances.
     
     TODO: Verify this for i = 0 && b = 0
  */
  int op1 = cpu->ac;
  int op2 = linc_read(cpu, addr);
  int sign = (op1 & 04000) ^ (op2 & 04000);
  int t;
  
  if(op1 & LINC_OP_NEGATIVE)
    op1 = ~op1 & 07777;
  
  if(op2 & LINC_OP_NEGATIVE)
    op2 = ~op2 & 07777;
  
  t = (op1 & 03777) * (op2 & 03777);
  if(addr & LINC_ADDR_FRACTION) {
    printf("Fraction\n");
    /* Fractional multiplication */
    cpu_set_ac(cpu,
	       (((sign ? ~t : t) >> 11) & 03777) | sign);
  } else {
    printf("Integer\n");
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
INSTRUCTION_B(LDH) {
  cpu_set_ac(cpu, linc_read(cpu, addr));
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
INSTRUCTION_B(STH) {
  linc_write(cpu, addr, cpu->ac);	     
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

  if(i & cpu->l)
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
  
  cpu_set_mq(cpu, m & 07777);
  cpu_set_ac(cpu, t & 07777);
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
  cpu_set_mq(cpu, t & 07777);
}

/*
 * Skip on external level
 */
INSTRUCTION_A(SXL) {
  int tst = 1;
  
  /* TODO: Implement prewired levels */
  switch(a) {
  case 15:
    /* Key Struck */
    
  case 16:
    /* Tape Instruction done */

  case 17:
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
  
  cpu_write(cpu, a, t);
  
  linc_inc_pc(cpu);
}

/*
 * Jump
 */
INSTRUCTION_D(JMP) {
  cpu_set_pc(cpu, addr);
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
INSTRUCTION_B(SHD) {
  int op = linc_read(cpu, addr);
  if(!(addr & LINC_ADDR_RIGHT))
    op = op >> 6;
  
  if((op & 077) == (cpu->ac & 077))
    linc_inc_pc(cpu);
  
  lprintf(LOG_ERROR, "SHD instruction not implented...\n");
}

/*
 * Skip and Rotate
 * If the least significant bit of the operand is 0,
 * the operand is rotated right 1 place and the
 * next instruction is skipped.
 */
INSTRUCTION_B(SRO) {
  int op = linc_read(cpu, addr);
  
  if(!(op & 01)) {
    linc_write(cpu, addr,
	       (op >> 1) | ((op & 1) << 11));
    linc_inc_pc(cpu);
  }
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
  /* TODO: Implement scope */
}

/*
 * Display character on oscilloscope.
 */
INSTRUCTION_B(DSC) {
  /* TODO: Implement the scope... */
}

/*
 * Load instruction field buffer with N.
 */
INSTRUCTION_A(LIF) {
  /* TODO: Implement LIF */
}

/*
 * Load data field register with N.
 */
INSTRUCTION_A(LDF) {
  /* TODO: Implement LDF */
}

/*
 * LINC TAPE Instructions
 */
INSTRUCTION_A(TAPE) {
  /* TODO: Implement LINC TAPE */
  lprintf(LOG_ERROR, "Tape not implemented!\n");
}

/* +-----+-----------------------+
 * | 0 1 | 2 3 4 5 6 7 8 9 10 11 |
 * | OP  |       ADDRESS         |
 * +-----+-----------------------+
 */
static void instr_direct(cpu_instance* cpu, int op, int addr) {
  lprintf(LOG_DEBUG, "%s 0%o\n", mnemonics_d[op], addr);
  
  switch(op) {
    CASE_D(ADD);
    CASE_D(STC);
    CASE_D(JMP);
  default:
    lprintf(LOG_ERROR, "Illegal LINC instruction  in instr_direct!\n");
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
  if(i)
    lprintf(LOG_DEBUG, "%s I 0%o\n", mnemonics_a[op], a);
  else
    lprintf(LOG_DEBUG, "%s 0%o\n", mnemonics_a[op], a);
  
  
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
    CASE_A(TAPE);
  case LINC_OP_EXT1:
  case LINC_OP_EXT2:
  case LINC_OP_EXT3:
  case LINC_OP_EXT4:
    break;
  default:
    lprintf(LOG_ERROR, "Illegal LINC instruction  in instr_alpha!\n");
    break;
  }
}

/* +-------+---------+---+-----------+
 * | 0 1 2 | 3 4 5 6 | 7 | 8 9 10 11 |
 * | 0 0 1 | OP-code | I |   BETA    |
 * +-------+---------+---+-----------+
 */
static void instr_beta(cpu_instance* cpu, int op, int i, int b) {
  int addr = beta_addr(cpu, i, b);
  
  lprintf(LOG_DEBUG, "%s %s 0%o (%.4o)\n",
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
    CASE_B(MUL);
    CASE_B(LDH);
    CASE_B(STH);
    CASE_B(SHD);
    CASE_B(SAE);
    CASE_B(SRO);
    CASE_B(BCL);
    CASE_B(BSE);
    CASE_B(BCO);
    CASE_B(DSC);
  default:
    lprintf(LOG_ERROR, "Illegal LINC instruction  in instr_beta!\n");
    break;
  }
}

void linc_step(cpu_instance* cpu) {
  lprintf(LOG_DEBUG, "linc_step pc: 0%o ir: 0%o\n", cpu->pc, cpu->ir);
  
  linc_inc_pc(cpu);
  
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
