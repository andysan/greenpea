
#ifndef _CPU_H
#define _CPU_H

/* Amount of memory allways availible to the CPU (in words) */
#define CPU_CORE_SIZE 32768


#define CPU_LINC_SEG_SIZE (01777)

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
  CPU_FLAGS_SKIP     = 1,   /* Skip Flip-Flop set */
  CPU_FLAGS_FLO      = 2,   /* Overflow Flip-Flop set */
  CPU_FLAGS_8MODE    = 4,   /* Processor is in PDP-8 Mode */
  CPU_FLAGS_RUN      = 8,   /* Processor is running */
  CPU_FLAGS_AUTO     = 16,  /* Auto Restart Flip-Flop is set */
  CPU_FLAGS_TRAP     = 32,  /* Instruction trap is enabled */
  CPU_FLAGS_INTPAUSE = 64,  /* An internal pause is occuring */
  CPU_FLAGS_ION      = 128, /* Program Interrupt facility enabled */
  CPU_FLAGS_IOPAUSE  = 256, /* An I/O Pause is occuring */
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

typedef struct cpu_callbacks_s cpu_callbacks;
typedef struct cpu_instance_s cpu_instance;

struct cpu_callbacks_s {
  int (*get_level)(struct cpu_instance_s* cpu, void* data, int level);
  void (*update_relays)(struct cpu_instance_s* cpu, void* data);
  /* Returns a value between -0777 and 0777 */
  int (*sample_ad)(struct cpu_instance_s* cpu, void* data, int n);
  
  void* data;
};

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
  CPU_REG5  dfr;     /* Data Field Register */
  
  CPU_REG6  relays; /* Register holding relay status */
  
  cpu_flags flags;
  cpu_flags state ;
  
  CPU_REG12 ls;     /* Left Switches */
  CPU_REG12 rs;     /* Right Switches */
  CPU_REG3  ifs;    /* Instruction Field Switches */
  CPU_REG6   ss;    /* Sense Switches */
  
  cpu_callbacks* callbacks;
  
  /* NULL terminated list of devices attached to the IO bus.
     NULL if no devices attached. */
  struct io_device_s** devices;
  
  int core[CPU_CORE_SIZE];
};

cpu_instance* cpu_create();
void cpu_destroy(cpu_instance* cpu);

void cpu_step(cpu_instance* cpu);

void cpu_set_ac(cpu_instance* cpu, int ac);
void cpu_set_pc(cpu_instance* cpu, int pc);
void cpu_set_mq(cpu_instance* cpu, int mq);
void cpu_set_l(cpu_instance* cpu, int l);
void cpu_set_state(cpu_instance* cpu, cpu_states s);
void cpu_set_flag(cpu_instance* cpu, cpu_flags s);
void cpu_set_relays(cpu_instance* cpu, int r);
void cpu_set_ifr(cpu_instance* cpu, int n);
void cpu_set_dfr(cpu_instance* cpu, int n);

void cpu_clear_state(cpu_instance* cpu, cpu_states s);
void cpu_clear_flag(cpu_instance* cpu, cpu_flags s);

void cpu_write(cpu_instance* cpu, int ma, int mb);

int cpu_read(cpu_instance* cpu, int ma);

int cpu_call_sam(cpu_instance* cpu, int n);
int cpu_call_ext_level(cpu_instance* cpu, int level);


#endif
