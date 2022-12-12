/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _CPU_H
#define _CPU_H

typedef struct cpu_instance cpu_instance_t;

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

typedef int cpu_reg12_t;
typedef int cpu_reg6_t;
typedef int cpu_reg5_t;
typedef int cpu_reg3_t;
typedef int cpu_reg1_t;

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
} cpu_flags_t;

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
} cpu_states_t;

typedef enum {
    CPU_ESF_IO_PRESET = 1,
    CPU_ESF_DISABLE_ASR33_INT = 2,
    CPU_ESF_FAST_SAMPLE = 4,
    CPU_ESF_CHAR_SIZE = 8,
    CPU_ESF_TAPE_TRAP = 16,
    CPU_ESF_INSTRUCTION_TRAP = 32,
} cpu_esf_t;

typedef struct {
    int (*get_level)(cpu_instance_t *cpu, void *data, int level);
    void (*update_relays)(cpu_instance_t *cpu, void *data);
    /* Returns a value between -0777 and 0777 */
    int (*sample_ad)(cpu_instance_t *cpu, void *data, int n);

    void *data;
} cpu_callbacks_t;

struct cpu_instance {
    cpu_reg12_t  ac;     /* Accumulator */
    cpu_reg1_t   l;      /* Link (AC extension) */
    cpu_reg12_t  mq;     /* Multiplier Quotient */
    cpu_reg12_t  pc;     /* Program Counter */

    cpu_reg12_t  ir;     /* Instruction register */
#if 0
    cpu_reg12_t  ma;     /* Memory address register */
    cpu_reg12_t  mb;     /* Memory Buffer */
#endif

    cpu_reg5_t   ifr;    /* Instruction Field Register */
    cpu_reg5_t   ifb;    /* Instruction Field Buffer */
    cpu_reg5_t   dfr;    /* Data Field Register */
    cpu_reg12_t  sfr;    /* Save Field Register */

    cpu_reg6_t   relays; /* Register holding relay status */

    cpu_flags_t  flags;
    cpu_states_t state;

    cpu_esf_t    esf;    /* LINC special functions register */

    cpu_reg12_t  ls;     /* Left Switches */
    cpu_reg12_t  rs;     /* Right Switches */
    cpu_reg3_t   ifs;    /* Instruction Field Switches */
    cpu_reg6_t   ss;     /* Sense Switches */

    cpu_callbacks_t *callbacks;
    vr12_t *vr12;
    asr33_t *asr33;

    /* NULL terminated list of devices attached to the IO bus.
       NULL if no devices attached. */
    io_device_t **devices;

    int core[CPU_CORE_SIZE];
};

cpu_instance_t *cpu_create();
void cpu_destroy(cpu_instance_t *cpu);

void cpu_io_preset(cpu_instance_t *cpu);

void cpu_step(cpu_instance_t *cpu);

void cpu_set_ac(cpu_instance_t *cpu, int ac);
void cpu_set_pc(cpu_instance_t *cpu, int pc);
void cpu_set_mq(cpu_instance_t *cpu, int mq);
void cpu_set_l(cpu_instance_t *cpu, int l);
void cpu_set_state(cpu_instance_t *cpu, cpu_states_t s);
void cpu_set_flag(cpu_instance_t *cpu, cpu_flags_t s);
void cpu_set_relays(cpu_instance_t *cpu, int r);
void cpu_set_ifr(cpu_instance_t *cpu, int n);
void cpu_set_dfr(cpu_instance_t *cpu, int n);
void cpu_set_esf(cpu_instance_t *cpu, int n);

void cpu_clear_state(cpu_instance_t *cpu, cpu_states_t s);
void cpu_clear_flag(cpu_instance_t *cpu, cpu_flags_t s);

void cpu_write(cpu_instance_t *cpu, int ma, int mb);

int cpu_read(cpu_instance_t *cpu, int ma);

void cpu_inc_pc(cpu_instance_t *cpu);

int cpu_call_sam(cpu_instance_t *cpu, int n);
int cpu_call_ext_level(cpu_instance_t *cpu, int level);


#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
