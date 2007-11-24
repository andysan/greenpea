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

#ifndef _CPU_H
#define _CPU_H

typedef struct cpu_instance_s cpu_instance;

#include "iob.h"
#include "vr12.h"
#include "asr33.h"

/* Amount of memory allways availible to the CPU (in words) */
#define CPU_CORE_SIZE 32768


/* The following are register locations in the instrcution field
   of a LINC-program. Note that some registers overlap.
*/
#define CPU_LINC_IF_RET_ADDR (00)
#define CPU_LINC_IF_DSC_HORIZONTAL (01)
#define CPU_LINC_IF_BETA (01)
#define CPU_LINC_IF_ALFA (00)

/* The following LINC-registers are located in instruction segment 0
   ie C(IF) = 00.
*/
#define CPU_LINC_ABS_TRAP_RET (0140)
#define CPU_LINC_ABS_TRAP (0141)
#define CPU_LINC_ABS_INT_RET (0040)
#define CPU_LINC_ABS_INT (0040)

typedef int CPU_REG12;
typedef int CPU_REG6;
typedef int CPU_REG5;
typedef int CPU_REG3;
typedef int CPU_REG1;

typedef enum {
     CPU_FLAGS_SKIP     = 1,    /* Skip Flip-Flop set */
     CPU_FLAGS_FLO      = 2,    /* Overflow Flip-Flop set */
     CPU_FLAGS_8MODE    = 4,    /* Processor is in PDP-8 Mode */
     CPU_FLAGS_RUN      = 8,    /* Processor is running */
     CPU_FLAGS_AUTO     = 16,   /* Auto Restart Flip-Flop is set */
     CPU_FLAGS_TRAP     = 32,   /* Instruction trap is enabled */
     CPU_FLAGS_INTPAUSE = 64,   /* An internal pause is occuring */
     CPU_FLAGS_ION      = 128,  /* Program Interrupt facility enabled */
     CPU_FLAGS_IOPAUSE  = 256,  /* An I/O Pause is occuring */
     CPU_FLAGS_DJR      = 512,  /* Disable saving of JMP return for next JMP */
     CPU_FLAGS_LIF      = 1024, /* IFB -> IFR on next JMP Y (Y != 0) */
     CPU_FLAGS_LINC_II  = 2048  /* Inhibit Interrupts until next JMP in new IF */
} cpu_flags;

typedef enum {
     CPU_STATE_F   = 1,
     CPU_STATE_D   = 2,
     CPU_STATE_E   = 4,
     CPU_STATE_E2  = 8,
     CPU_STATE_INT = 16,
     CPU_STATE_WC  = 32,
     CPU_STATE_CA  = 64,
     CPU_STATE_B   = 128,
     CPU_STATE_TB  = 256
} cpu_states;

typedef enum {
     CPU_ESF_IO_PRESET = 1,
     CPU_ESF_DISABLE_ASR33_INT = 2,
     CPU_ESF_FAST_SAMPLE = 4,
     CPU_ESF_CHAR_SIZE = 8,
     CPU_ESF_TAPE_TRAP = 16,
     CPU_ESF_INSTRUCTION_TRAP = 32,
} cpu_esf;

typedef struct cpu_callbacks_s {
     int (*get_level)(cpu_instance *cpu, void *data, int level);
     void (*update_relays)(cpu_instance *cpu, void *data);
     /* Returns a value between -0777 and 0777 */
     int (*sample_ad)(cpu_instance *cpu, void *data, int n);
  
     void *data;
} cpu_callbacks;

struct cpu_instance_s {
     CPU_REG12 ac;     /* Accumulator */
     CPU_REG1  l;      /* Link (AC extension) */
     CPU_REG12 mq;     /* Multiplier Quotient */
     CPU_REG12 pc;     /* Program Counter */

     CPU_REG12 ir;     /* Instruction register */
#if 0
     CPU_REG12 ma;     /* Memory address register */
     CPU_REG12 mb;     /* Memory Buffer */
#endif

     CPU_REG5  ifr;    /* Instruction Field Register */
     CPU_REG5  ifb;    /* Instruction Field Buffer */
     CPU_REG5  dfr;    /* Data Field Register */
     CPU_REG12 sfr;    /* Save Field Register */
  
     CPU_REG6  relays; /* Register holding relay status */
  
     cpu_flags flags;
     cpu_states state ;
  
     cpu_esf esf;      /* LINC special functions register */
  
     CPU_REG12 ls;     /* Left Switches */
     CPU_REG12 rs;     /* Right Switches */
     CPU_REG3  ifs;    /* Instruction Field Switches */
     CPU_REG6   ss;    /* Sense Switches */
  
     cpu_callbacks* callbacks;
     vr12 *vr12;
     asr33 *asr33;
  
     /* NULL terminated list of devices attached to the IO bus.
        NULL if no devices attached. */
     struct io_device_s **devices;
  
     int core[CPU_CORE_SIZE];
};

cpu_instance* cpu_create();
void cpu_destroy(cpu_instance *cpu);

void cpu_io_preset(cpu_instance *cpu);

void cpu_step(cpu_instance *cpu);

void cpu_set_ac(cpu_instance *cpu, int ac);
void cpu_set_pc(cpu_instance *cpu, int pc);
void cpu_set_mq(cpu_instance *cpu, int mq);
void cpu_set_l(cpu_instance *cpu, int l);
void cpu_set_state(cpu_instance *cpu, cpu_states s);
void cpu_set_flag(cpu_instance *cpu, cpu_flags s);
void cpu_set_relays(cpu_instance *cpu, int r);
void cpu_set_ifr(cpu_instance *cpu, int n);
void cpu_set_dfr(cpu_instance *cpu, int n);
void cpu_set_esf(cpu_instance *cpu, int n);

void cpu_clear_state(cpu_instance *cpu, cpu_states s);
void cpu_clear_flag(cpu_instance *cpu, cpu_flags s);

void cpu_write(cpu_instance *cpu, int ma, int mb);

int cpu_read(cpu_instance *cpu, int ma);

void cpu_inc_pc(cpu_instance *cpu);

int cpu_call_sam(cpu_instance *cpu, int n);
int cpu_call_ext_level(cpu_instance *cpu, int level);


#endif
/* 
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
