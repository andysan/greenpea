
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <liblog/log.h>

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
  cpu->dfr = 0;
  cpu->relays = 0;
  
  /* State */
  cpu->flags = 0;
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

void cpu_step(cpu_instance* cpu) {
  if(cpu->flags & CPU_FLAGS_8MODE) {
    lprintf(LOG_ERROR, "PDP-8 mode not supported yet!\n");
  } else {
    cpu->ir = linc_read(cpu, cpu->pc);
    linc_step(cpu);
  }
}

void cpu_set_ac(cpu_instance* cpu, int ac) {
  lprintf(LOG_DEBUG, "cpu_set_ac %.4o\n", ac);
  cpu->ac = ac;
}

void cpu_set_pc(cpu_instance* cpu, int pc) {
  lprintf(LOG_DEBUG, "cpu_set_pc %.4o\n", pc);
  cpu->pc = pc;
}

void cpu_set_l(cpu_instance* cpu, int l) {
  lprintf(LOG_DEBUG, "cpu_set_l %i\n", l);
  cpu->l = l;
}

void cpu_set_state(cpu_instance* cpu, cpu_states s) {
  cpu->state |= s;
}

void cpu_clear_state(cpu_instance* cpu, cpu_states s) {
  cpu->state &= ~s;
}

void cpu_set_flag(cpu_instance* cpu, cpu_flags f) {
  cpu->flags |= f;
}

void cpu_clear_flag(cpu_instance* cpu, cpu_flags f) {
  cpu->flags &= ~f;
}

void cpu_write(cpu_instance* cpu, int ma, int mb) {
  /* TODO: Do something useful after outputing error. */
  lprintf(LOG_DEBUG, "cpu_write: %.4o %.4o\n", ma, mb);
  if(ma > CPU_CORE_SIZE)
    lprintf(LOG_ERROR, "Address out of range in cpu_write.\n");
  else if(mb > 07777)
    lprintf(LOG_ERROR, "Illegal word size in cpu_write.\n");
  else
    cpu->core[ma] = mb;
}

int cpu_read(cpu_instance* cpu, int ma) {
  /* TODO: Do something useful after outputing error. */
  lprintf(LOG_DEBUG, "cpu_read: %.4o\n", ma);
  if(ma > CPU_CORE_SIZE) {
    lprintf(LOG_ERROR, "Address out of range in cpu_read.\n");
    return 0;
  } else {
    return cpu->core[ma];
  }
}

