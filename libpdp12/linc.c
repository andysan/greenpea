
#include <stdio.h>
#include <liblog/log.h>
#include "cpu.h"
#include "linc.h"

#define INSTRUCTION_D(mn) \
  static void instr_ ## mn(cpu_instance* cpu, int addr)

#define INSTRUCTION_A(mn) \
  static void instr_ ## mn(cpu_instance* cpu, int i, int a)

#define INSTRUCTION_B(mn) \
  static void instr_ ## mn (cpu_instance* cpu, int i, int b)

#define CASE_D(mn) \
  case LINC_OP_ ## mn: \
    instr_ ## mn (cpu, addr); \
    break;

#define CASE_A(mn) \
  case LINC_OP_ ## mn: \
    instr_ ## mn (cpu, i, a); \
    break;

#define CASE_B(mn) \
  case LINC_OP_ ## mn: \
    instr_ ## mn (cpu, i, b); \
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
				    "EXT2",
				    "EXT3",
				    "EXT4",
				    "---",
				    "LIF",
				    "LDF",
				    "EXT5",
				    "---"};

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
    linc_inc_pc(cpu);
    
    if(i)
      return temp;
    else
      return linc_read(cpu, temp) & 03777;
  } else {
    temp = linc_read(cpu, b);
    if(i) {
      temp = ((temp + 1) & 01777) | (temp & 06000);
      linc_write(cpu, b, temp);
    }
    
    return temp & 03777;
  }
}

static int beta_read(cpu_instance* cpu, int i, int b) {
  return linc_read(cpu, 
		  beta_addr(cpu, i, b));
}

static void beta_write(cpu_instance* cpu, int i, int b, int data) {
  linc_write(cpu, 
	    beta_addr(cpu, i, b),
	    data);
}




/*
 * Add to Accumulator (Direct Address)
 * Addition is done using 1's complement.
 */
INSTRUCTION_D(ADD) {
  int t;
  t = cpu->ac + linc_read(cpu, addr);
  if(t & 010000)
    t = t & 07777 + 1;
  cpu_set_ac(cpu, t);
}

/*
 * Add to Accumulator (B-class)
 * Addition is done using 1's complement.
 */
INSTRUCTION_B(ADA) {
  int t;
  t = cpu->ac + beta_read(cpu, i, b);
  if(t & 010000)
    t = t & 07777 + 1;
  cpu_set_ac(cpu, t);
}

/*
 * Add to Memory
 * Addition is done using 1's complement.
 */
INSTRUCTION_B(ADM) {
  int t;
  t = cpu->ac + beta_read(cpu, i, b);
  if(t & 010000)
    t = t & 07777 + 1;
  cpu_set_ac(cpu, t);
  beta_write(cpu, i, b, t);
}

/*
 * Add to Memory
 */
INSTRUCTION_B(LAM) {

}

/*
 * Multiply
 */
INSTRUCTION_B(MUL) {

}

/*
 * Load Accumulator
 */
INSTRUCTION_B(LDA) {
  cpu_set_ac(cpu, beta_read(cpu, i, b));
}

/*
 * Load Accumulator (half)
 */
INSTRUCTION_B(LDH) {
  cpu_set_ac(cpu, beta_read(cpu, i, b));
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
  beta_write(cpu, i, b, cpu->ac);	     
}

/*
 * Store Accumulator (half)
 */
INSTRUCTION_B(STH) {
  beta_write(cpu, i, b, cpu->ac);	     
}

/*
 * Rotate left
 */
INSTRUCTION_A(ROL) {

}

/*
 * Rotate right
 */
INSTRUCTION_A(ROR) {

}

/*
 * Scale right
 */
INSTRUCTION_A(SCR) {

}


/*
 * Misc A instructions class 1
 * htl, esf, qac, djr, clr, atr, rta, nop, com, sfa
 */
INSTRUCTION_A(EXT1) {

}

/*
 * Set register N to contents of register Y
 */
INSTRUCTION_A(SET) {

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

}

/*
 * Bit Set
 */
INSTRUCTION_B(BSE) {

}

/*
 * Bit Complement
 */
INSTRUCTION_B(BCO) {

}

/*
 * Skip on AC == Y
 */
INSTRUCTION_B(SAE) {

}

/*
 * Right half AC unequal to specified
 * half of memory register Y.
 */
INSTRUCTION_B(SHD) {

}

/*
 * Misc A instructions class 3
 * SNS, SKP, AZE, APO, LZE, IBZ, FLO, QLZ
 */
INSTRUCTION_A(EXT3) {
}

/*
 * Misc A instructions class 2
 * SXL, KST
 */
INSTRUCTION_A(EXT2) {
}

/*
 * Rotate memory register one place; then
 * if bit 0 of Y equals 0, skip next.
 */
INSTRUCTION_B(SRO) {
}

/*
 * Index memory register Y, skip when contents
 * of Y equal 1777.
 */
INSTRUCTION_A(XSK) {

}

/*
 * Sample analog channel N.
 */
INSTRUCTION_A(SAM) {

}

/*
 * Display point on oscilloscope.
 */
INSTRUCTION_A(DIS) {
  
}

/*
 * Display character on oscilloscope.
 */
INSTRUCTION_B(DSC) {

}

/*
 * Misc A instructions class 4
 * IOB, RSW, LSW
 */
INSTRUCTION_A(EXT4) {

}

/*
 * Load instruction field buffer with N.
 */
INSTRUCTION_A(LIF) {

}

/*
 * Load data field register with N.
 */
INSTRUCTION_A(LDF) {

}

/*
 * Misc A instructions class 5
 * Mainly LINC TAPE
 */
INSTRUCTION_A(EXT5) {

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
  if(i)
    lprintf(LOG_DEBUG, "%s I 0%o\n", mnemonics_a[op], a);
  else
    lprintf(LOG_DEBUG, "%s 0%o\n", mnemonics_a[op], a);
  

  switch(op) {
    CASE_A(EXT1);
    CASE_A(SET);
    CASE_A(SAM);
    CASE_A(DIS);
    CASE_A(XSK);
    CASE_A(ROL);
    CASE_A(ROR);
    CASE_A(SCR);
    CASE_A(EXT2);
    CASE_A(EXT3);
    CASE_A(EXT4);
    CASE_A(LIF);
    CASE_A(LDF);
    CASE_A(EXT5);
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
  if(i)
    lprintf(LOG_DEBUG, "%s I 0%o\n", mnemonics_b[op], b);
  else
    lprintf(LOG_DEBUG, "%s 0%o\n", mnemonics_b[op], b);
  
  
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

void linc_exec(cpu_instance* cpu) {
  lprintf(LOG_DEBUG, "linc_exec pc: 0%o ir: 0%o\n", cpu->pc, cpu->ir);
  
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
