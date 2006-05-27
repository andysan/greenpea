
#include <stdio.h>

#include "cpu.h"
#include "pdp8.h"
#include "linc.h"

cpu_instance* cpu_create() {
  cpu_instance* cpu = malloc(sizeof(cpu_instance));
  /* TODO: Check buffer allocation */
  
  /* Registers */
  cpu->ac = 0;
  cpu->l = 0;
  cpu->pc = 0;
  cpu->mq = 0;
  cpu->ir = 0;
  cpu->ifr = 0;
  cpu->df = 0;
  cpu->relays = 0;
  
  /* State */
  cpu->flags = CPU_FLAGS_LINCMODE;
  cpu->state = CPU_STATE_B;
  
  /* Switches */
  cpu->ls = 0;
  cpu->rs = 0;
  cpu->ifs = 0;
  cpu->ss = 0;
  
  memset(cpu->core, 0, CPU_CORE_SIZE);
  
  return cpu;
}

void cpu_destroy(cpu_instance* cpu) {
  free(cpu);
}

void cpu_exec(cpu_instance* cpu) {
  cpu->ir = cpu_read(cpu, cpu->pc);
  cpu_linc_exec(cpu);
}

void cpu_set_ac(cpu_instance* cpu, int ac) {
  cpu->ac = ac;
}

void cpu_set_pc(cpu_instance* cpu, int pc) {
  cpu->pc = pc;
}

void cpu_set_ca(cpu_instance* cpu) {
  cpu->state |= CPU_STATE_CA;
}

void cpu_clear_ca(cpu_instance* cpu) {
  cpu->state |= !CPU_STATE_CA;
}

void cpu_write(cpu_instance* cpu, int ma, int mb) {
  /* TODO: Do something useful after outputing error. */
  if(ma < CPU_CORE_SIZE)
    fprintf(stderr, "Address out of range in cpu_write.\n");
  else if(mb > 07777)
    fprintf(stderr, "Illegal word size in cpu_write.\n");
  else
    cpu->core[ma] = mb;
}

int cpu_read(cpu_instance* cpu, int ma) {
  /* TODO: Do something useful after outputing error. */
  if(ma > CPU_CORE_SIZE) {
    fprintf(stderr, "Address out of range in cpu_read.\n");
    return 0;
  } else {
    return cpu->core[ma];
  }
}

